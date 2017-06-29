/** @file

  Unit tests for AppendStr.h.

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

#define __LIB_TS_TEST_APPENDSTR_CC__

namespace
{
bool bad;
}

#undef ink_assert

inline void ink_assert(bool b)
{
  bad = !b;
}

#include "AppendStr.h"

#include <iostream>
#include <cstdlib>

using namespace std;
using namespace util;

void check(bool b)
{
  if (!b)
  {
    cout << "FAIL\n";
    exit(1);
  }
  bad = false;
}

using AS = AppendStr<5>;

int main()
{
  AS as;

  check(as.strLen() == 0);

  as.add("argh");
  check(!bad);
  check(strcmp(as, "argh") == 0);
  check(as.strLen() == 4);

  AS as2{as};

  as2.add('x');
  check(bad);

  as2 = as;
  as2.add("x");
  check(bad);

  as2 = as;
  as2.add("xxx");
  check(bad);

  as.clear();
  check(as.strLen() == 0);

  for (int i = 1; i <= 4; ++i) {
    as.add('x');
    check(!bad);
    check(as.strLen() == i);
    check(memcmp(as.str(), "xxxx", i) == 0);
  }
  check(strcmp(as.str(), "xxxx") == 0);

  as.clear();
  as.add('a', "rg", "h");
  check(!bad);
  check(strcmp(as, "argh") == 0);
  check(as.strLen() == 4);

  as.clear();
  as.add("a", "rg", 'h');
  check(!bad);
  check(strcmp(as, "argh") == 0);
  check(as.strLen() == 4);

  as.clear();
  as.add("ar");
  check(as.left() == 3);
  strncpy(as.end(), "gh", as.left());
  as.newEnd();
  check(!bad);
  check(strcmp(as, "argh") == 0);
  check(as.strLen() == 4);

  cout << "SUCCESS\n";

  return(0);
}
