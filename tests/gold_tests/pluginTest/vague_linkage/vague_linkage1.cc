/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
TEMP
*/

#include <ts/ts.h>

// TSReleaseAssert() doesn't seem to produce any logging output for a debug build, so do this to assert in release
// and debug builds.
//
#if defined(__OPTIMIZE__)
#define ALWAYS_ASSERT TSReleaseAssert
#else
#define ALWAYS_ASSERT TSAssert
#endif

#include <tscpp/util/test_vague_linkage.h>

namespace
{
#define PINAME "vague_linkage1"
char PIName[] = PINAME;
} // end anonymous namespace

void
TSPluginInit(int argc, const char *argv[])
{
  TSDebug(PIName, "TSPluginInit()");

  TSPluginRegistrationInfo info;

  info.plugin_name   = PIName;
  info.vendor_name   = "Apache Software Foundation";
  info.support_email = "dev@trafficserver.apache.org";

  if (TSPluginRegister(&info) != TS_SUCCESS) {
    TSError(PINAME ": Plugin registration failed");

    return;
  }

  ++vague_linkage::dflt;
  ++vague_linkage::inited;
  ++vague_linkage::dflt_f();
  ++vague_linkage::inited_f();
}
