/** @file

  This file used for catch based tests. It is the main() stub.

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

#include <cstring>
#include <cctype>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "HttpConfig.h"
#include <ts/BitSetListInit.h>

namespace HF  = HttpForwarded;
namespace HFO = HF::Option;

using OBS = std::bitset<HFO::Num>;

using OBSIL = ts::BitSetListInit<HFO::Num>;

namespace
{
// Alternate upper/lower case and pad front and back with a space.
class XS
{
private:
  std::string s;

public:
  XS(const char *in)
  {
    bool upper{true};
    for (; *in; ++in) {
      if (islower(*in)) {
        s += upper ? toupper(*in) : *in;
        upper = !upper;

      } else if (isupper(*in)) {
        s += upper ? *in : tolower(*in);
        upper = !upper;

      } else {
        s += *in;
      }
    }
  }

  operator ts::string_view() const { return ts::string_view(s.c_str()); }
};

void
test(const char *spec, const char *reqErr, OBS bS)
{
  std::string error{"cheese"};

  REQUIRE(bS == HF::optStrToBitset(XS(spec), error));
  REQUIRE(error == reqErr);
}

} // end annonymous namespace

TEST_CASE("Forwarded", "[FWD]")
{
  std::string error;

  test(" none ", "", OBS());

  test("  ", "\"Forwarded\" configuration: option invalid", OBS());

  test(" : ", "\"Forwarded\" configuration: option invalid", OBS());

  test(" | ", "\"Forwarded\" configuration: option invalid", OBS());

  test(" by = ip ", "", OBSIL{HFO::ByIp});

  test(" by = unknown ", "", OBSIL{HFO::ByUnknown});

  test(" by = servername ", "", OBSIL{HFO::ByServerName});

  test(" by = uuid ", "", OBSIL{HFO::ByUuid});

  test(" for ", "", OBSIL{HFO::For});

  test(" proto ", "", OBSIL{HFO::Proto});

  test(" host ", "", OBSIL{HFO::Host});

  test(" connection ", "", OBSIL{HFO::Connection});

  test(" proto : by = uuid | for ", "", OBSIL{HFO::Proto, HFO::ByUuid, HFO::For});

#undef X
#define X(S) " by = ip " S " by = unknown " S " by = servername " S " by = uuid " S " for " S " proto " S " host " S " connection "

  test(X(":"), "", OBS().set());

  test(X("|"), "", OBS().set());

  test(X("|") "|" X(":"), "", OBS().set());

  test(X("|") " : abcd", "\"Forwarded\" configuration: option invalid", OBS());

  test(X("|") " : for = abcd", "\"Forwarded\" configuration: use of = in an option that is not \"by\"", OBS());

  test(X("|") " : by = abcd", "\"Forwarded\" configuration: \"by\" option invalid", OBS());

  test(X("|") " : by ", "\"Forwarded\" configuration: \"by\" option must be followed by =", OBS());
}
