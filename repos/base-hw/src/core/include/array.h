/*
 * \brief   Array helper class
 * \author  Stefan Kalkowski
 * \date    2016-09-30
 */

/*
 * Copyright (C) 2016 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <base/log.h>

namespace Genode { template <typename, unsigned> class Array; }


template <typename T, unsigned MAX>
class Genode::Array
{
	private:

		unsigned _count = 0;
		T        _objs[MAX];

		void _init(unsigned i, T obj) { _objs[i] = obj; }

		template <typename ... TAIL>
		void _init(unsigned i, T obj, TAIL ... tail)
		{
			_objs[i] = obj;
			_init(i+1, tail...);
		}

	public:

		Array() {}

		template<typename ... ARGS>
		Array(ARGS ... args) : _count(sizeof...(ARGS))
		{
			static_assert(sizeof...(ARGS) <= MAX, "Array too small!");
			_init(0, args...);
		}

		void add(T const & obj)
		{
			if (_count == MAX) error("Array too small!");
			else               _objs[_count++] = obj;
		}

		template <typename FUNC>
		void for_each(FUNC f) const {
			for (unsigned i = 0; i < _count; i++) f(_objs[i]); }

		unsigned count() const { return _count; }
};

#endif /* _ARRAY_H_ */
