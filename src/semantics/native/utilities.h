/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#ifndef READYLOG_UTILITIES_H_
#define READYLOG_UTILITIES_H_

#include <model/gologpp.h>
#include <model/expressions.h>
#include <type_traits>

namespace gologpp {


template<class T> inline
T &access(T *ptr)
{ return *ptr; }

template<class T> inline
T &access(const unique_ptr<T> &ptr)
{ return *ptr; }

template<class T> inline
const Expression &access(const SafeExprOwner<T> &ptr)
{ return *ptr; }

template<class T> inline
typename std::enable_if<!std::is_pointer<T>::value, T>::type
&access(T &&ptr)
{ return std::forward<T>(ptr); }



} // namespace gologpp

#endif // READYLOG_UTILITIES_H_
