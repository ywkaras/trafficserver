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

#include <fstream>
#include <cstdlib>

#include <ts/ts.h>
#include <tscpp/api/Continuation.h>

using atscppapi::Continuation;

namespace
{
#define PINAME "test_hooks"
char PIName[] = PINAME;

// NOTE:  It's important to flush this after writing so that a gold test using this plugin can examine the log before TS
// terminates.
//
std::fstream logFile;

class GlobalCont : public Continuation
{
public:
  GlobalCont() : Continuation() {}

  GlobalCont(Mutex m) : Continuation(m) {}

private:
  int _run(TSEvent event, void *edata) override;
};

int
GlobalCont::_run(TSEvent event, void *edata)
{
  logFile << "Global: event=" << TSHttpEventNameLookup(event) << '(' << event << ')' << std::endl;

  TSDebug(PIName, "Global: event=%s(%d) edata=%p", TSHttpEventNameLookup(event), event, edata);

  if (TS_EVENT_HTTP_READ_REQUEST_HDR == event) {
    auto txn = static_cast<TSHttpTxn>(edata);

    TSDebug(PIName, "Global: ssn=%p", TSHttpTxnSsnGet(txn));

    TSHttpTxnReenable(txn, TS_EVENT_HTTP_CONTINUE);
  }

  return 0;
}

GlobalCont gCont;

} // end anonymous namespace

void
TSPluginInit(int argc, const char *argv[])
{
  TSPluginRegistrationInfo info;

  info.plugin_name   = PIName;
  info.vendor_name   = (char *)"Apache Software Foundation";
  info.support_email = (char *)"dev@trafficserver.apache.org";

  if (TSPluginRegister(&info) != TS_SUCCESS) {
    TSError(PINAME ": Plugin registration failed");

    return;
  }

  const char *fileSpec = std::getenv("OUTPUT_FILE");

  if (nullptr == fileSpec) {
    TSError(PINAME ": Environment variable OUTPUT_FILE not found.");

    return;
  }

  logFile.open(fileSpec, std::ios::out);
  if (!logFile.is_open()) {
    TSError(PINAME ": could not open log file \"%s\"", fileSpec);

    return;
  }

  gCont = GlobalCont(nullptr);

  // Setup the global hook
  TSHttpHookAdd(TS_HTTP_READ_REQUEST_HDR_HOOK, gCont.asTSCont());
}
