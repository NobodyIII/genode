/*
* \brief  Fifo file system
* \author Ben Larson
* \date   2018-03-01
*/

/*
* Copyright (C) 2018 Genode Labs GmbH
*
* This file is part of the Genode OS framework, which is distributed
* under the terms of the GNU Affero General Public License version 3.
*/

#ifndef _INCLUDE__VFS__FIFO_FILE_SYSTEM_H_
#define _INCLUDE__VFS__FIFO_FILE_SYSTEM_H_

#include <vfs/single_file_system.h>
#include <base/signal.h>
#include <base/registry.h>
#include <base/env.h>


namespace Vfs {
	using namespace Genode;
	
	class Fifo_file_system;
}

class Vfs::Fifo_file_system : public Single_file_system
{
	private:

		enum { DEFAULT_BUFFER_SIZE = 4096 };
		
		class Byte_ring_buffer
		{
		private:
			size_t const _buffer_size;
			size_t _read_pos = 0;
			size_t _read_avail = 0;
			
			/* initialize to null to make compiler happy */
			char * _buffer = 0;
			
			size_t _buffer_end() { return _buffer_size - 1; }
			
			size_t _write_pos()
			{
				size_t write_pos = _read_pos + _read_avail;
				if (write_pos > _buffer_end())
					write_pos -= _buffer_size;
				
				return write_pos;
			}
			
		public:
			Byte_ring_buffer(Genode::Allocator &alloc, size_t buffer_size = DEFAULT_BUFFER_SIZE):
				_buffer_size(buffer_size)
			{
				_buffer = new (alloc) char[buffer_size];
			}
			
			size_t read_avail() { return _read_avail; }
			size_t write_avail() { return _buffer_size - _read_avail; }
			
			size_t read(char *dst, size_t count)
			{
				if (count > _read_avail)
					count = _read_avail;
				
				size_t bytes_before_end = _buffer_size - _read_pos;
				if (count <= bytes_before_end)
					memcpy(dst, &_buffer[_read_pos], count);
				else {
					/* loop around */
					memcpy(dst, &_buffer[_read_pos], bytes_before_end);
					memcpy(&dst[bytes_before_end], _buffer, count - bytes_before_end);
				}
				
				return count;
			}
			
			size_t write(const char *src, size_t count)
			{
				if (count > write_avail())
					count = write_avail();
				
				size_t bytes_before_end = _buffer_size - _write_pos();
				if (count <= bytes_before_end)
					memcpy(&_buffer[_write_pos()], src, count);
				else {
					/* loop around */
					memcpy(&_buffer[_write_pos()], src, bytes_before_end);
					memcpy(_buffer, src + bytes_before_end, count - bytes_before_end);
				}
				
				return count;
			}
		};
		
		
		Env              &_env;

		Byte_ring_buffer  _fifo_buf;

		Vfs_handle        _read_handle;
		Vfs_handle        _write_handle;

		Signal_context_capability _read_ready_sigh;
		Signal_context_capability _write_ready_sigh;

// 		typedef Genode::Registered<Vfs_handle>      Registered_handle;
// 		typedef Genode::Registry<Registered_handle> Handle_registry;

// 		Handle_registry _handle_registry { };

		void _unblock_read()
		{
			if (_read_ready_sigh.valid())
				Signal_transmitter(_read_ready_sigh).submit();
		}

		void _unblock_write()
		{
			if (_write_ready_sigh.valid())
				Signal_transmitter(_write_ready_sigh).submit();
		}

	public:

		Fifo_file_system(Env &env,
						Allocator &alloc,
						Xml_node config,
						Io_response_handler &)
		:
			Single_file_system(NODE_TYPE_CHAR_DEVICE, name(), config),
// 			_label(config.attribute_value("label", Label())),
			_env(env), _fifo_buf(alloc), _read_handle(*this, *this, alloc, 0),
			_write_handle(*this, *this, alloc, 0) { }

		static const char *name()   { return "fifo"; }
		char const *type() override { return "fifo"; }

		Open_result open(char const  *path, unsigned mode,
						Vfs_handle **out_handle,
						Allocator   &alloc) override
		{
			if (!_single_file(path))
				return OPEN_ERR_UNACCESSIBLE;

			if (mode = OPEN_MODE_RDONLY) {
				*out_handle = &_read_handle;
				return OPEN_OK;
			else if (mode == OPEN_MODE_WRONLY) {
				*out_handle = &_write_handle;
				return OPEN_OK;
			} else {
				return OPEN_ERR_NO_PERM;
			}
		}

		/********************************
		** File I/O service interface **
		********************************/

		Write_result write(Vfs_handle *, char const *buf, file_size buf_size,
						file_size &out_count) override
		{
			if (handle != &_write_handle)
				return WRITE_ERR_INVALID;

			bool unblock = !_fifo_buf.read_avail();

			if (_fifo_buf.write_avail()) {
				out_count = _fifo_buf.write(buf, buf_size);

				if (unblock)
					_unblock_read();

				return WRITE_OK;
			} else {
				return WRITE_ERR_WOULD_BLOCK;
			}
		}

		Read_result complete_read(Vfs_handle *, char *dst, file_size count,
								file_size &out_count)
		{
			if (handle != &_read_handle)
				return READ_ERR_INVALID;

			bool unblock = !_fifo_buf.write_avail();

			if (_fifo_buf.read_avail()) {
				out_count = _fifo_buf.read(dst, count);

				if (unblock)
					_unblock_write();

				return READ_OK;
			} else {
				return READ_QUEUED;
			}
		}


		bool read_ready(Vfs_handle *) override
		{
			return _fifo_buf.read_avail();
		}

		Ftruncate_result ftruncate(Vfs_handle *, file_size) override
		{
			return FTRUNCATE_OK;
		}

		bool check_unblock(Vfs_handle *, bool rd, bool wr, bool) override
		{
			if (rd && (_fifo_buf.read_avail() > 0))
				return true;

			if (wr && (_fifo_buf.write_avail() > 0))
				return true;

			return false;
		}

		void register_write_ready_sigh(Vfs_handle *handle, Signal_context_capability sigh) override
		{
			if (handle != &_write_handle)
				return;

			_write_ready_sigh = sigh;
		}

		void register_read_ready_sigh(Vfs_handle *handle, Signal_context_capability sigh) override
		{
			if (handle != &_read_handle)
				return;

			_read_ready_sigh = sigh;
		}
};

#endif /* _INCLUDE__VFS__TERMINAL_FILE_SYSTEM_H_ */
