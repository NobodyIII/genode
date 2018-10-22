
#pragma once


// Genode includes
#include <base/attached_rom_dataspace.h>
#include <base/heap.h>

// stdcxx includes
#include <vector>
#include <algorithm>


namespace QToolButton_demo
{
	template <typename T>
	class Button_factory;

	using Genode::Env;
	using Genode::Heap;

	using std::vector;
}


template <typename T>
class QToolButton_demo::Button_factory
{
	private:

		// unfortunately classes derived from Qt classes can't be stored as
		// reference or objects this is because the copy constructor of Qt
		// classes is deleted
		typedef vector<T*> Item_vector;

		Env&         _env;
		Heap         _heap { _env.ram(), _env.rm() };

		Item_vector  _buttons;

	public:

		Button_factory(Env& env) :
			_env { env }
		{ }

		virtual ~Button_factory()
		{
			// free the memory of every slice button
			for (auto& element : _buttons) {
				destroy(_heap, element);
			}
		}

		T* lookup(const Component_name& name)
		{
			auto it = std::find_if(_buttons.begin(),
			                       _buttons.end(),
			                       [&name] (const Launcher_button* btn)->bool { return (btn->name() == name); });
			return (it == _buttons.end()) ? nullptr : *it;
		}

		T* insert(const Component_name& name)
		{
			T* button = lookup(name);

			if (!button) {

				// Qt classes can't be copied
				button = new (_heap) T(name);
				_buttons.push_back(button);
			}

			return button;
		}

		void remove(const Component_name& name)
		{
			auto it = _buttons.find(name);
			if (_buttons.end() != it) {
				// store pointer to dynamically allocated object
				T* button = *it;

				// remove element from container
				_buttons.erase(it);

				// free the memory
				destroy(_heap, button);
			}
		}
};
