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

namespace
{

ts::string_view three[] = {"a", "", "bcd"};

}

TEST_CASE("BufferWriter::_pushBack(StringView)", "[BWPB]")
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

  REQUIRE(x.good);
}

namespace
{
template <size_t N> using BW = ts::LocalBufferWriter<N>;
}

TEST_CASE("Concrete Buffer Writer", "[BWC]")
{
  BW<1> bw;

  REQUIRE(!((bw.capacity() != 1) or (bw.size() != 0) or bw.error() or (bw.auxCapacity() != 1)));

  bw.c('#');

  REQUIRE(!((bw.capacity() != 1) or (bw.size() != 1) or bw.error() or (bw.auxCapacity() != 0)));

  REQUIRE(bw.view() == "#");

  bw.c('#');

  REQUIRE(bw.error());

  bw.resize(1);

  REQUIRE(!((bw.capacity() != 1) or (bw.size() != 1) or bw.error() or (bw.auxCapacity() != 0)));

  REQUIRE(bw.view() == "#");
}

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

  bw.l("he").c(' ').cstr("quick").c(' ').l("brown");

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

TEST_CASE("Concrete Buffer Writer 2", "[BWC2]")
{
  BW<20> bw;

  REQUIRE(twice(bw));

  char space[21];

  space[20] = '!';

  ts::FixedBufferWriter fbw(space, 20);

  REQUIRE(twice(fbw));

  REQUIRE(space[20] == '!');

  ts::LocalBufferWriter<20> bw2(bw), bw3;

  REQUIRE(bw2.view() == "The quick brown fox");

  bw3 = bw2;

  REQUIRE(bw3.view() == "The quick brown fox");
}

namespace
{

template <class BW>
bool
testDiscard(BW &bw)
{
  if (bw.size() != 0) {
    return false;
  }

  bw.c('T');

  if (bw.size() != 1) {
    return false;
  }

  bw.l("he").c(' ').cstr("quick").c(' ').l("brown");

  if (bw.size() != (sizeof("The quick brown") - 1)) {
    return false;
  }

  bw.resize(0);

  bw.sV("The", 3).c(' ').sV("quick", 5).c(' ').sV(ts::string_view("brown", 5));

  if  (bw.size() != (sizeof("The quick brown") - 1)) {
    return false;
  }

  bw.auxWrite(sizeof(" fox") - 1);

  if (bw.size() != (sizeof("The quick brown fox") - 1)) {
    return false;
  }

  bw.resize(sizeof("The quick brown fox") - 1);

  if (bw.size() != (sizeof("The quick brown fox") - 1)) {
    return false;
  }

  return true;
}

} // end anonymous namespace

TEST_CASE("Discard Buffer Writer", "[BWD]")
{
  char scratch[1] = { '!' };
  ts::FixedBufferWriter fbw(scratch, 0);

  REQUIRE(testDiscard(fbw));

  // Make sure no actual writing.
  //
  REQUIRE(scratch[0] == '!');

  ts::LocalBufferWriter<0> lbw;

  char *cheat = lbw.auxBuffer();

  *cheat = '!';

  REQUIRE(testDiscard(lbw));

  // Make sure no actual writing.
  //
  REQUIRE(*cheat == '!');

  ts::LocalBufferWriter<0> bw2(lbw), bw3;

  REQUIRE(bw2.size() == (sizeof("The quick brown fox") - 1));

  bw3 = bw2;

  REQUIRE(bw3.size() == (sizeof("The quick brown fox") - 1));
}

TEST_CASE("Buffer Writer << operator", "[BW<<]")
{
  ts::LocalBufferWriter<50> bw;

  bw << "The" << ' ' << "quick" << ' ' << "brown fox";

  REQUIRE(bw.view() == "The quick brown fox");
}

namespace
{

void w12(ts::BufferWriter &bw, const char *s1, const char *s2)
{
  bw.cstr(s1).c(' ').cstr(s2);
}

class W34
{
private:
  const char *s3;

public:
  W34(const char *s3_) : s3(s3_) { }

  void
  operator () (ts::BufferWriter &bw, const char *s4)
  {
    bw.c(' ').cstr(s3).c(' ').cstr(s4);
  }
};

template <class BW>
void
testWriteMbr(BW &bw)
{
  size_t sz;

  bw.write(w12, "one", "two").saveSize(sz).write(W34("three"), "four").resizeIfError(sz);
}

} // end anonymous namespace

TEST_CASE("Buffer Writer write() / resizeIfError()", "[BWWRIE]")
{
  ts::LocalBufferWriter<50> bw;

  testWriteMbr(bw);

  REQUIRE(bw.view() == "one two three four");

  ts::LocalBufferWriter<15> bw2;

  testWriteMbr(bw2);

  REQUIRE(bw2.view() == "one two");
}
