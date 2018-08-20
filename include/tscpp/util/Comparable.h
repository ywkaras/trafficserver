/** @file

   Given a class T with a compare function that returns ordering information, create the standard
   comparison operators.

   @section license License

   Licensed to the Apache Software Foundation (ASF) under one or more contributor license
   agreements.  See the NOTICE file distributed with this work for additional information regarding
   copyright ownership.  The ASF licenses this file to you under the Apache License, Version 2.0
   (the "License"); you may not use this file except in compliance with the License.  You may obtain
   a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software distributed under the License
   is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
   or implied. See the License for the specific language governing permissions and limitations under
   the License.
 */

#pragma once

#include <type_traits>
#include <utility>
#include "tscore/ts_meta.h"

namespace ts
{
template <typename T, typename U = T> struct ComparablePolicy {
};

namespace detail
{
  template <typename T, typename U>
  auto
  ComparableFunction(T const &lhs, U const &rhs, meta::CaseTag<0> const &) -> decltype(rhs.cmp(lhs), int())
  {
    return static_cast<int>(rhs.cmp(lhs)) * -1;
  }

  template <typename T, typename U>
  auto
  ComparableFunction(T const &lhs, U const &rhs, meta::CaseTag<1> const &) -> decltype(lhs.cmp(rhs), int())
  {
    return static_cast<int>(lhs.cmp(rhs));
  }

  template <typename T, typename U>
  auto
  ComparableFunction(T const &lhs, U const &rhs, meta::CaseTag<2> const &) -> decltype(cmp(rhs, lhs), int())
  {
    return static_cast<int>(cmp(rhs, lhs)) * -1;
  }

  template <typename T, typename U>
  auto
  ComparableFunction(T const &lhs, U const &rhs, meta::CaseTag<3> const &) -> decltype(cmp(lhs, rhs), int())
  {
    return static_cast<int>(cmp(lhs, rhs));
  }

  template <typename T, typename U>
  auto
  ComparableFunction(T const &lhs, U const &rhs, meta::CaseTag<4> const &) -> decltype(ComparablePolicy<T, U>()(lhs, rhs), bool())
  {
    return static_cast<int>(ComparablePolicy<T, U>()(lhs, rhs));
  }

} // namespace detail
/** Create standard comparison operators given a compare function.
 *
 * @tparam T Comparable type.
 * @tparam U Other type for comparison.
 *
 * To successfully use this mixin, there are two requirements.
 * - There must be a comparison function that returns an int in the standard ternary compare style.
 * - The class must inherit from this mixin.
 *
 * The standard ternary compare must return an @c int which is
 * - negative if @a lhs is smaller than @a rhs
 * - 0 if @a lhs is equal to @a rhs
 * - positive if @a lhs is greater than @a rhs
 *
 * By default this must be either a free function named cmp or a method @c T::cmp.
 *
 * There are two normal use cases for the mixin. The first is to provide self comparisons. In
 * that the inheritance would be
 * @code
 * class T : public ts::Comparable<T>
 * @endcode
 *
 * The other use case is to provide comparisons to a different type, usually because there is an
 * instance of that type in @a T. For instance, if @a T contains a @c std::string it could be useful
 * to define a @c cmp method against @c std::string_view and then provide the comparison operators.
 * E.g.
 *
 * @code
 * class T : public ts::Comparable<std::string_view> {
 *   std::string text;
 *   int cmp(std::string_view that) { return strcmp(text, that); }
 *   // ...
 * };
 *
 * void func(T const& thing) {
 *   std::string_view name { ... };
 *   if (name != thing) { ... } // != works because of Comparable
 *   // ...
 * }
 * @endcode
 *
 * will enable the 6 comparison operators for @a T.
 */

struct Comparable {
};

template <typename T>
auto
operator==(T &&lhs, T &&rhs) ->
  typename std::enable_if<std::is_base_of<Comparable, typename std::decay<T>::type>::value, bool>::type
{
  return 0 == detail::ComparableFunction(std::forward<T>(lhs), std::forward<T>(rhs), meta::CaseArg);
}

template <typename T, typename U>
auto
operator==(T const &lhs, U const &rhs) -> typename std::enable_if<
  !std::is_same<T, U>::value && (std::is_base_of<Comparable, T>::value || std::is_base_of<Comparable, U>::value), bool>::type
{
  return 0 == detail::ComparableFunction(lhs, rhs, meta::CaseArg);
}

template <typename T>
auto
operator!=(T const &lhs, T const &rhs) -> typename std::enable_if<std::is_base_of<Comparable, T>::value, bool>::type
{
  return 0 != detail::ComparableFunction(lhs, rhs, meta::CaseArg);
}

template <typename T, typename U>
auto
operator!=(T const &lhs, U const &rhs) -> typename std::enable_if<
  !std::is_same<T, U>::value && (std::is_base_of<Comparable, T>::value || std::is_base_of<Comparable, U>::value), bool>::type
{
  return 0 != detail::ComparableFunction(lhs, rhs, meta::CaseArg);
}

template <typename T>
auto
operator<(T const &lhs, T const &rhs) -> typename std::enable_if<std::is_base_of<Comparable, T>::value, bool>::type
{
  return detail::ComparableFunction(lhs, rhs, meta::CaseArg) < 0;
}

template <typename T, typename U>
auto
operator<(T const &lhs, U const &rhs) -> typename std::enable_if<
  !std::is_same<T, U>::value && (std::is_base_of<Comparable, T>::value || std::is_base_of<Comparable, U>::value), bool>::type
{
  return detail::ComparableFunction(lhs, rhs, meta::CaseArg) < 0;
}

template <typename T>
auto
operator<=(T const &lhs, T const &rhs) -> typename std::enable_if<std::is_base_of<Comparable, T>::value, bool>::type
{
  return detail::ComparableFunction(lhs, rhs, meta::CaseArg) <= 0;
}

template <typename T, typename U>
auto
operator<=(T const &lhs, U const &rhs) -> typename std::enable_if<
  !std::is_same<T, U>::value && (std::is_base_of<Comparable, T>::value || std::is_base_of<Comparable, U>::value), bool>::type
{
  return detail::ComparableFunction(lhs, rhs, meta::CaseArg) <= 0;
}

template <typename T>
auto
operator>(T const &lhs, T const &rhs) -> typename std::enable_if<std::is_base_of<Comparable, T>::value, bool>::type
{
  return detail::ComparableFunction(lhs, rhs, meta::CaseArg) > 0;
}

template <typename T, typename U>
auto
operator>(T const &lhs, U const &rhs) -> typename std::enable_if<
  !std::is_same<T, U>::value && (std::is_base_of<Comparable, T>::value || std::is_base_of<Comparable, U>::value), bool>::type
{
  return detail::ComparableFunction(lhs, rhs, meta::CaseArg) > 0;
}

template <typename T>
auto
operator>=(T const &lhs, T const &rhs) -> typename std::enable_if<std::is_base_of<Comparable, T>::value, bool>::type
{
  return detail::ComparableFunction(lhs, rhs, meta::CaseArg) >= 0;
}

template <typename T, typename U>
auto
operator>=(T const &lhs, U const &rhs) -> typename std::enable_if<
  !std::is_same<T, U>::value && (std::is_base_of<Comparable, T>::value || std::is_base_of<Comparable, U>::value), bool>::type
{
  return detail::ComparableFunction(lhs, rhs, meta::CaseArg) >= 0;
}

} // end namespace ts
