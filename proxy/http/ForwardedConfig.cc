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

namespace
{
class BadOptionsErrMsg
{
public:
  // Construct with referece to string that will contain error message.
  //
  BadOptionsErrMsg(std::string &err) : _err(err), _count(0) {}

  // Add a bad option.
  //
  void
  add(ts::StringView badOpt)
  {
    if (_count == 0) {
      _err   = _quote(badOpt);
      _count = 1;
    } else if (_count == 1) {
      _saveLast = badOpt;
      _count    = 2;
    } else {
      _err += ", " + _quote(_saveLast);
      _saveLast = badOpt;
      ++_count;
    }
  }

  bool
  empty() const
  {
    return _count == 0;
  }

  void
  done()
  {
    if (_count == 0) {
      _err.clear();
    } else if (_count == 1) {
      _err += " is a bad option.";
    } else {
      _err += " and " + _quote(_saveLast) + " are bad options.";
    }
  }

private:
  std::string
  _quote(ts::StringView sv)
  {
    return std::string("\"") + std::string(sv.begin(), sv.size()) + "\"";
  }

  std::string &_err;

  ts::StringView _saveLast;

  int _count;
};

// Compare a StringView to a nul-termimated string, converting the StringView to lower case and ignoring whitespace in it.
//
bool
eqIgnoreCaseWs(ts::StringView sv, const char *target)
{
  const char *s = sv.begin();

  std::size_t skip = 0;
  std::size_t i    = 0;

  while ((i + skip) < sv.size()) {
    if (std::isspace(s[i + skip])) {
      ++skip;
    } else if (std::tolower(s[i + skip]) != target[i]) {
      return false;
    } else {
      ++i;
    }
  }

  return target[i] == '\0';
}

} // end anonymous namespace

namespace HttpForwarded
{
OptionBitSet
optStrToBitset(ts::string_view optConfigStr, std::string &error)
{
  const ts::StringView Delimiters(":|");

  OptionBitSet optBS;

  // Convert to TS StringView to be able to use parsing members.
  //
  ts::StringView oCS(optConfigStr.data(), optConfigStr.size());

  error.clear();

  if (eqIgnoreCaseWs(oCS, "none")) {
    return OptionBitSet();
  }

  BadOptionsErrMsg em(error);

  do {
    ts::StringView optStr = oCS.extractPrefix(Delimiters);

#undef X
#define X(OPT_NAME, OPT_C_STRING)             \
  if (eqIgnoreCaseWs(optStr, OPT_C_STRING)) { \
    optBS.set(Option::OPT_NAME);              \
  }

    // clang-format off
    //
    X(For, "for") else 
    X(ByIp, "by=ip") else 
    X(ByUnknown, "by=unknown") else 
    X(ByServerName, "by=servername") else 
    X(ByUuid, "by=uuid") else 
    X(Proto, "proto") else 
    X(Host, "host") else 
    X(ConnectionCompact, "connection=compact") else 
    X(ConnectionStd, "connection=standard") else 
    X(ConnectionStd, "connection=std") else 
    X(ConnectionFull, "connection=full") else
    {
      em.add(optStr);
    }
    // clang-format on

  } while (oCS);

  em.done();

  if (!error.empty()) {
    error = std::string("\"Forwarded\" configuration: ") + error;

    return OptionBitSet();
  }

  return optBS;

} // end optStrToBitset()

} // end namespace HttpForwarded
