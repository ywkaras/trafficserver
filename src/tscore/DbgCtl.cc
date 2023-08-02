/** @file

  Implementation file for DbgCtl class.

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

#include <mutex>
#include <set>
#include <cstring>
#include <atomic>

#include "swoc/bwf_ip.h"

#include <tscore/ink_assert.h>
#include <tscore/Diags.h>
#include <tscore/Regex.h>

// The resistry of fast debug controllers has a ugly implementation to handle the whole-program initialization
// and destruction order problem with C++.
//
class DbgCtl::_RegistryAccessor
{
private:
  struct TagCmp {
    bool
    operator()(TSDbgCtl const &a, TSDbgCtl const &b) const
    {
      return std::strcmp(a.tag, b.tag) < 0;
    }
  };

public:
  using Set = std::set<TSDbgCtl, TagCmp>;

  class Registry
  {
  public:
    Set set;

  private:
    Registry() = default;

    // Mutex must be locked before this is called.
    //
    ~Registry()
    {
      for (auto &ctl : set) {
        delete[] ctl.tag;
      }
      _mtx.unlock();
    }

    std::mutex _mtx;

    friend class DbgCtl::_RegistryAccessor;
  };

  _RegistryAccessor()
  {
    // Work-around for issue:  https://github.com/apache/trafficserver/issues/10129
    //
    // Calling member functions of the Regex class (used by the Diags class) can trigger the initialization of a
    // thread_local variable.  Initialization of any thread_local variable seems to lock a global mutex in the C/C++
    // runtime environment.  This same global mutex is locked while a shared library is being dynamically loaded.  It
    // is kept locked during the non-local dynamic initialization for the shared library.  This initialization may
    // include DbgCtl member function calls that create instances of this class, and thus lock the Registry mutex.
    // This leads to a possible deadlock scenario:
    // 1.  A shared libary is loading, so the global mutex is locked.
    // 2.  In another thread, a DbgCtl member function call is made, which calls this constructor, and
    //     locks the Registry mutex.
    // 3.  The first thread blocks trying to lock the Registry mutex.
    // 4.  The second thread calls a Diags member function, needs to initialize a thread_local variable,
    //     and therefore blocks on the global mutex.  Deadlock.
    //
    // The work-around here is to make sure, in each thread, the Regex thread_local variable is initialized
    // before any locks of the Registry mutex.
    if (thread_local bool done{false}; !done) {
      Regex r;
      static_cast<void>(r.compile("dummy"));
      done = true;
    }

    if (!_registry_instance) {
      Registry *expected{nullptr};
      Registry *r{new Registry};
      if (!_registry_instance.compare_exchange_strong(expected, r)) {
        r->_mtx.lock();
        delete r;
      }
    }
    _registry_instance.load()->_mtx.lock();
    _mtx_is_locked = true;
  }

  ~_RegistryAccessor()
  {
    if (_mtx_is_locked) {
      _registry_instance.load()->_mtx.unlock();
    }
  }

  // This is not static so it can't be called with the registry mutex is unlocked.  It should not be called
  // after registry is deleted.
  //
  Registry &
  data()
  {
    return *_registry_instance;
  }

  void
  delete_registry()
  {
    auto r = _registry_instance.load();
    ink_assert(r != nullptr);
    _registry_instance = nullptr;
    delete r;
    _mtx_is_locked = false;
  }

  // Reference count of references to Registry.
  //
  inline static std::atomic<unsigned> registry_reference_count{0};

private:
  bool _mtx_is_locked{false};

  inline static std::atomic<Registry *> _registry_instance{nullptr};
};

TSDbgCtl const *
DbgCtl::_new_reference(char const *tag)
{
  ink_assert(tag != nullptr);

  TSDbgCtl ctl;

  ctl.tag = tag;

  // DbgCtl instances may be declared as static objects in the destructors of objects not destoyed till program exit.
  // So, we must handle the case where the construction of such instances of DbgCtl overlaps with the destruction of
  // other instances of DbgCtl.  That is why it is important to make sure the reference count is non-zero before
  // constructing _RegistryAccessor.  The _RegistryAccessor constructor is thereby able to assume that, if it creates
  // the Registry, the new Registry will not be destroyed before the mutex in the new Registry is locked.

  ++_RegistryAccessor::registry_reference_count;

  _RegistryAccessor ra;

  auto &d{ra.data()};

  if (auto it = d.set.find(ctl); it != d.set.end()) {
    return &*it;
  }

  auto sz = std::strlen(tag);

  ink_assert(sz > 0);

  {
    char *t = new char[sz + 1]; // Deleted by ~Registry().
    std::memcpy(t, tag, sz + 1);
    ctl.tag = t;
  }
  ctl.on = diags() && diags()->tag_activated(tag, DiagsTagType_Debug);

  auto res = d.set.insert(ctl);

  ink_assert(res.second);

  return &*res.first;
}

void
DbgCtl::_rm_reference()
{
  _RegistryAccessor ra;

  ink_assert(ra.registry_reference_count != 0);

  --ra.registry_reference_count;

  if (0 == ra.registry_reference_count) {
    ra.delete_registry();
  }
}

void
DbgCtl::update()
{
  ink_release_assert(diags() != nullptr);

  _RegistryAccessor ra;

  if (!ra.registry_reference_count) {
    return;
  }

  auto &d{ra.data()};

  for (auto &i : d.set) {
    const_cast<char volatile &>(i.on) = diags()->tag_activated(i.tag, DiagsTagType_Debug);
  }
}
