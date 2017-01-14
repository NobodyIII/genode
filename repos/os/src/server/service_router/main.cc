/*
 * \brief  Service router
 * \author Ben Larson
 * \date   2016-03-15
 */
 
#include <base/env.h>
#include <base/component.h>
#include <base/heap.h>
#include <base/sleep.h>
#include <parent/parent.h>
#include <root/component.h>
#include <base/rpc_server.h>
#include <root/root.h>
#include <os/config.h>
#include <input/root.h>
#include <base/service.h>
#include <util/arg_string.h>
#include <os/session_policy.h>


using namespace Genode;

namespace Service_router {

	struct Root : Genode::Root
	{
		Root() { }
	};

	struct Forwarded_capability : Id_space<Parent::Client>::Element, Session_capability, List<Forwarded_capability>::Element
	{
		Parent::Client client;
		Forwarded_capability(Env &env, Parent::Service_name &service, Root::Session_args const &args, Affinity const &affinity):
			Id_space<Parent::Client>::Element(client, env.id_space()),
			Session_capability(env.session(service, id(), args, affinity)) {
			}
	};

	class Service_source : public Rpc_object<Service_router::Root>, public List<Service_source>::Element /* may need to add ram quota handling */
	{
	private:
		List<Forwarded_capability> _caps;
		Env &_env;
		Allocator &_alloc;
		char _name[64];
		Parent::Service_name *_service_name;

		Forwarded_capability *_lookup_cap(Session_capability session)
		{
			log("_lookup_cap()");
			for (Forwarded_capability *cap = _caps.first(); cap; cap = cap->next()) {
				if (*cap == session)
					return cap;
			}
			error("cap lookup failed");
			return 0;
		}

	public:
		Session_capability session(Session_args const &args, Affinity const &affinity) override
		{
			char args_buf[Parent::Session_args::MAX_SIZE];
			strncpy(args_buf, args.string(), sizeof(args_buf));
			Session_label in_label = label_from_args(args_buf);
			try {
				Session_policy policy(in_label);

				for (Xml_node service_node = policy.sub_node("service");;
					 service_node = service_node.next("service")) {

					char name_buf[64];
					service_node.attribute("name").value(name_buf, sizeof(name_buf));
					if (strcmp(name_buf,_name,64)) continue;
					
					char source_buf[64];
					service_node.attribute("source").value(source_buf, sizeof(source_buf));
					
					Arg_string::set_arg_string(args_buf, 64, "label", source_buf);
					Session_args new_args(args_buf);

					Forwarded_capability *cap = new (_alloc) Forwarded_capability(
						_env, *_service_name, new_args, affinity);
					_caps.insert(cap);

					if (!cap->valid())
						error("invalid cap");
					return *cap;
				}
			} catch (Xml_node::Nonexistent_sub_node) {
				error("rejecting session request; policy for '", in_label.string(), "' has no route for service ", Cstring(_name));
				throw Root::Unavailable();
			} catch (Session_policy::No_policy_defined) {
				error("rejecting session request; no matching policy for ", in_label.string());
				throw Root::Unavailable();
			}
		}

		void upgrade(Session_capability session, Upgrade_args const &args) override
		{
			log("upgrade()");
			_env.parent().upgrade(_lookup_cap(session)->id(), args);
		}

		void close(Session_capability session) override
		{
			log("close()");
			Forwarded_capability *cap = _lookup_cap(session);
			_caps.remove(cap);
			_env.parent().close(cap->id());
			destroy(_alloc, cap);
		}

		Service_source(Env &env, Allocator &alloc, char *name): _env(env), _alloc(alloc) {
			strncpy(_name,name,64);
			_service_name = new (alloc) Parent::Service_name(_name);
		}
	};
	struct Main;
}

struct Service_router::Main
{
	Env &env;
	Entrypoint &ep;
	Sliced_heap heap;

	List<Service_source> service_sources;

	void handle_config_update()
	{
		Genode::config()->reload();

		for (Service_source *src;
		     (src = service_sources.first()); ) {
			service_sources.remove(src);
			destroy(heap, src);
		}
		
		try {
			for (Xml_node service_node = config()->xml_node().sub_node("service");;
			     service_node = service_node.next("service")) {

				char name_buf[64];
				service_node.attribute("name").value(name_buf, sizeof(name_buf));

				Service_source *src = new (heap) Service_source(env, heap, name_buf);
				Root_capability root_cap=ep.manage(*src);
				env.parent().announce(name_buf, root_cap);
				service_sources.insert(src);
			}
		} catch (Xml_node::Nonexistent_sub_node) { }

	}

	Signal_handler<Main> config_update_dispatcher =
		{ ep, *this, &Main::handle_config_update };

	Main(Env &env) : env(env), ep(env.ep()), heap(env.ram(), env.rm())
	{
		handle_config_update();

		Genode::config()->sigh(config_update_dispatcher);
	}

};

void Component::construct(Env &env) { static Service_router::Main srv(env); }
