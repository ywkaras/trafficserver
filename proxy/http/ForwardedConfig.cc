/** @file

  Configuration of Forwarded HTTP header option.

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

#include <bitset>
#include <string>
#include <cctype>

#include <ts/string_view.h>
#include <ts/MemView.h>

#include <HttpConfig.h>

namespace HttpForwarded
{
OptionBitSet
optStrToBitset(ts::string_view optConfigStr, std::string &error)
{
  using SV = ts::StringView;

  const SV Delimiters(":|");

  using OBS = OptionBitSet;

  auto isSp = [](char c) -> bool { return std::isspace(c); };

  OBS optBS;

  // Convert to TS StringView to be able to use parsing members.
  //
  SV oCS(optConfigStr.data(), optConfigStr.size());

  if (strcasecmp(oCS.trim(isSp), SV("none")) == 0) {
    error.clear();

    return OBS();
  }

  do {
    SV optStr = oCS.extractPrefix(Delimiters);
    optStr.ltrim(isSp);

    SV eqRHS = optStr;

    SV eqLHS = eqRHS.extractPrefix('=');

    if (eqLHS != optStr) {
      // There's an equal sign in the option.

      if (strcasecmp(eqLHS.rtrim(isSp), SV("by")) != 0) {
        error = "\"Forwarded\" configuration: use of = in an option that is not \"by\"";
        return OBS();
      }

      eqRHS.trim(isSp);

      if (strcasecmp(eqRHS, SV("ip")) == 0) {
        optBS.set(Option::ByIp);

      } else if (strcasecmp(eqRHS, SV("unknown")) == 0) {
        optBS.set(Option::ByUnknown);

      } else if (strcasecmp(eqRHS, SV("servername")) == 0) {
        optBS.set(Option::ByServerName);

      } else if (strcasecmp(eqRHS, SV("uuid")) == 0) {
        optBS.set(Option::ByUuid);

      } else {
        error = "\"Forwarded\" configuration: \"by\" option invalid";
        return OBS();
      }
    } else { // No equal sign in option.

      optStr.rtrim(isSp);

      if (strcasecmp(optStr, SV("for")) == 0) {
        optBS.set(Option::For);

      } else if (strcasecmp(optStr, SV("proto")) == 0) {
        optBS.set(Option::Proto);

      } else if (strcasecmp(optStr, SV("host")) == 0) {
        optBS.set(Option::Host);

      } else if (strcasecmp(optStr, SV("connection")) == 0) {
        optBS.set(Option::Connection);

      } else if (strcasecmp(optStr, SV("by")) == 0) {
        error = "\"Forwarded\" configuration: \"by\" option must be followed by =";
        return OBS();

      } else {
        error = "\"Forwarded\" configuration: option invalid";
        return OBS();
      }
    }
  } while (oCS);

  error.clear();

  return optBS;

} // end optStrToBitset()

} // end namespace HttpForwarded
