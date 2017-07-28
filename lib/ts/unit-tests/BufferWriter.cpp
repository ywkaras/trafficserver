/** @file

    Unit tests for BufferWriter.h.

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

#include "BufferWriter.h"

#include "catch.hpp"

#include "string_view.h"

#include <cstring>

#define BEGIN_TEST(NAME, TAG) \
TEST_CASE(NAME, TAG) { \
auto theTest = [&] () -> bool

#define END_TEST \
; REQUIRE(theTest()); }

namespace
{

ts::string_view three[] = {"a", "", "bcd"};

}

BEGIN_TEST("BufferWriter::_pushBack(StringView)", "[BWPB]")
{
  class X : public ts::BufferWriter
  {
    size_t i, j;

  public:
    bool good;

    X() : i(0), j(0), good(true) {}

    void
    _pushBack(char c) override
    {
      while (j == three[i].size()) {
        ++i;
        j = 0;
      }

      if ((i >= 3) or (c != three[i][j])) {
        good = false;
      }

      ++j;
    }

    bool
    error() const override
    {
      return (false);
    }
  };

  X x;

  x.sV(three[0]).sV(three[1]).sV(three[2]);

  return (x.good);
}
END_TEST

namespace
{
template <size_t N> using BW = ts::LocalBufferWriter<N>;
}

BEGIN_TEST("Concrete Buffer Writer", "[BWC]")
{
  BW<1> bw;

  if ((bw.capacity() != 1) or (bw.size() != 0) or bw.error() or (bw.auxCapacity() != 1)) {
    return false;
  }

  bw.c('#');

  if ((bw.capacity() != 1) or (bw.size() != 1) or bw.error() or (bw.auxCapacity() != 0)) {
    return false;
  }

  if (bw.view() != "#") {
    return false;
  }

  bw.c('#');

  if (!bw.error()) {
    return (false);
  }

  bw.resize(1);

  if ((bw.capacity() != 1) or (bw.size() != 1) or bw.error() or (bw.auxCapacity() != 0)) {
    return false;
  }

  if (bw.view() != "#") {
    return false;
  }

  return true;
}
END_TEST

namespace
{

template <class BWType>
bool twice(BWType &bw)
{
  if ((bw.capacity() != 20) or (bw.size() != 0) or bw.error() or (bw.auxCapacity() != 20)) {
    return false;
  }

  bw.c('T');

  if ((bw.capacity() != 20) or (bw.size() != 1) or bw.error() or (bw.auxCapacity() != 19)) {
    return false;
  }

  if (bw.view() != "T") {
    return false;
  }

  bw.l("he").c(' ').nT("quick").c(' ').l("brown");

  if ((bw.capacity() != 20) or bw.error() or (bw.auxCapacity() != (21 - sizeof("The quick brown")))) {
    return false;
  }

  if (bw.view() != "The quick brown") {
    return false;
  }

  bw.resize(0);

  bw.sV("The", 3).c(' ').sV("quick", 5).c(' ').sV(ts::string_view("brown", 5));

  if ((bw.capacity() != 20) or bw.error() or (bw.auxCapacity() != (21 - sizeof("The quick brown")))) {
    return false;
  }

  if (bw.view() != "The quick brown") {
    return false;
  }

  std::strcpy(bw.auxBuffer(), " fox");
  bw.auxWrite(sizeof(" fox") - 1);

  if (bw.error()) {
    return false;
  }

  if (bw.view() != "The quick brown fox") {
    return false;
  }

  bw.c('x');

  if (bw.error()) {
    return false;
  }

  bw.c('x');

  if (!bw.error()) {
    return false;
  }

  bw.c('x');

  if (!bw.error()) {
    return false;
  }

  bw.resize(sizeof("The quick brown fox") - 1);

  if (bw.error()) {
    return false;
  }

  if (bw.view() != "The quick brown fox") {
    return false;
  }

  return true;
}

} // end anonymous namespace

BEGIN_TEST("Concrete Buffer Writer 2", "[BWC2]")
{
  BW<20> bw;

  twice(bw);

  char space[21];

  space[20] = '!';

  ts::FixedBufferWriter fbw(space, 20);

  twice(fbw);

  if (space[20] != '!') {
    return false;
  }

  ts::LocalBufferWriter<20> bw2(bw), bw3;

  if (bw2.view() != "The quick brown fox") {
    return false;
  }

  bw3 = bw2;

  if (bw3.view() != "The quick brown fox") {
    return false;
  }

  return true;
}
END_TEST
