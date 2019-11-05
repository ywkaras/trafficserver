/** @file

   Allow a notification to a Watched instance to in turn notify Watcher instances.

   @section license License

   Licensed to the Apache Software Foundation (ASF) under one
   or more contributor license agreements.  See the NOTICE file
   distributed with this work for additional information
   regarding copyright ownership.  The ASF licenses this file
   to you under the Apache License, Version 2.0 (the
   "License"); you may not use this file except in compliance
   with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#pragma once

#include <list>
#include <mutex>

#include <tscore/ink_assert.h>

namespace ts
{
class Watcher;

class Watched
{
public:
  void iAmDying();

private:
  friend class Watcher;

  using WList = std::list<Watcher *>;

  // Allow for instances of this class to be in classes using ClassAllocator.

  bool _watchers_initialized = false;

  WList &
  _watchers()
  {
    if (!_watchers_initialized) {
      new (_raw_watchers) WList;
      _watchers_initialized = true;
    }
    return *reinterpret_cast<WList *>(_raw_watchers);
  }

  alignas(WList) char _raw_watchers[sizeof(WList)];

  static std::mutex &
  _mtx()
  {
    static std::mutex m;
    return m;
  }
};

class Watcher
{
public:
  void
  watch(Watched &watched)
  {
    std::lock_guard<std::mutex> lg(Watched::_mtx());

    ink_assert(!_watched);

    watched._watchers().push_back(this);
    _self = watched._watchers().end();
    --_self;
    _watched      = &watched;
    _watched_died = false;
  }

  void
  iAmDying()
  {
    std::lock_guard<std::mutex> lg(Watched::_mtx());

    if (_watched) {
      _watched->_watchers().erase(_self);
      _watched = nullptr;
    }
  }

  bool
  watchedDied() const
  {
    return _watched_died;
  }

private:
  friend class Watched;

  bool _watched_died = false;
  Watched *_watched  = nullptr;
  Watched::WList::iterator _self;
};

inline void
Watched::iAmDying()
{
  std::lock_guard<std::mutex> lg(_mtx());

  if (_watchers_initialized) {
    for (auto watcher : _watchers()) {
      watcher->_watched_died = true;
      watcher->_watched      = nullptr;
    }

    _watchers().~WList();
  }
}

} // end namespace ts
