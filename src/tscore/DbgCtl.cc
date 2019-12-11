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
#include <atomic>
#include <set>
#include <cstring>

#include <tscore/Diags.h>

// The resistry of fast debug controllers has a ugly implementation to handle the whole-program initialization
// order problem with C++.
//
class DbgCtl::_RegistryAccessor
{
private:
  struct TagCmp {
    bool
    operator()(TSFDbgCtl const &a, TSFDbgCtl const &b) const
    {
      return std::strcmp(a.tag, b.tag) < 0;
    }
  };

public:
  _RegistryAccessor() : _lg(data().mtx) {}

  using Set = std::set<TSFDbgCtl, TagCmp>;

  struct Data {
    std::mutex mtx;
    Set set;
    bool output_enabled;
  };

  Data &
  data()
  {
    static Data d;
    return d;
  }

private:
  std::lock_guard<std::mutex> _lg;
};

TSFDbgCtl const *
DbgCtl::_get_ptr(char const *tag)
{
  ink_assert(tag != nullptr);

  TSFDbgCtl ctl;

  ctl.tag = tag;

  _RegistryAccessor ra;

  auto &d{ra.data()};

  if (auto it = d.set.find(ctl); it != d.set.end()) {
    return &*it;
  }

  auto sz = std::strlen(tag);

  ink_assert(sz > 0);

  {
    char *t = new char[sz + 1];
    std::memcpy(t, tag, sz + 1);
    ctl.tag = t;
  }
  ctl.on = d.output_enabled && diags()->tag_activated(tag, DiagsTagType_Debug);

  auto res = d.set.insert(ctl);

  return &*res.first;
}

void
DbgCtl::update()
{
  ink_release_assert(diags() != nullptr);

  int conf     = diags()->config.enabled(DiagsTagType_Debug);
  bool enabled = (1 == conf) || (3 == conf);

  _RegistryAccessor ra;

  auto &d{ra.data()};

  d.output_enabled = enabled;

  for (auto &i : d.set) {
    const_cast<char volatile &>(i.on) = enabled && diags()->tag_activated(i.tag, DiagsTagType_Debug);
  }
}
