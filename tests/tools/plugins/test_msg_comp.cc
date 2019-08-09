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
Regression test code for TS CPP API header HttpMsgComp.h.  The code assumes there will only be one active transaction at a time.
*/

#include <fstream>
#include <cstdlib>
#include <string_view>
#include <string>

#include <ts/ts.h>

#include <tscpp/api/HttpMsgComp.h>

// TSReleaseAssert() doesn't seem to produce any logging output for a debug build, so do both kinds of assert.
//
#if defined(__OPTIMIZE__)

#define ALWAYS_ASSERT(EXPR) \
  {                         \
    TSReleaseAssert(EXPR);  \
  }

#else

#define ALWAYS_ASSERT(EXPR) \
  {                         \
    TSAssert(EXPR);         \
  }

#endif

namespace
{
#define PINAME "msg_comp"
char PIName[] = PINAME;

// NOTE:  It's important to flush this after writing so that a gold test using this plugin can examine the log before TS
// terminates.
//
std::fstream logFile;

using atscppapi::txnRemapFromUrlStringGet;
using atscppapi::txnRemapToUrlStringGet;
using atscppapi::txnEffectiveUrlStringGet;
using atscppapi::MsgBase;
using atscppapi::MimeField;
using atscppapi::TxnClientReq;
using atscppapi::TxnClientResp;
using atscppapi::TxnServerReq;
using atscppapi::TxnServerResp;

void
dumpMimeField(const MimeField &fld)
{
  logFile << fld.nameGet() << ": ";

  int nVals = fld.valuesCount();
  std::string all;

  if (nVals > 0) {
    all += fld.valGet(0);

    for (int i = 1; i < nVals; ++i) {
      all += ", ";
      all += fld.valGet(i);
    }
  }
  ALWAYS_ASSERT(fld.valuesGet() == all);
  logFile << all << std::endl;
}

bool
sameMimeField(const MimeField &f1, const MimeField &f2)
{
  if (&f1.msg() != &f2.msg()) {
    return false;
  }

#if 0

  if (&f1.loc() != &f2.loc()) {
    return false;
  }

#else

  // Presumably as some sort of homage to Satan, trafficserver seems to make duplicate copies of the same Mime header
  // within a message.
  //
  int nVals = f1.valuesCount();
  if (nVals != f2.valuesCount()) {
    return false;
  }
  for (int i = 0; i < nVals; ++i) {
    if (f2.valGet(i) != f2.valGet(i)) {
      return false;
    }
  }

#endif

  return true;
}

void
dumpMsg(MsgBase &msg, std::string_view msgName)
{
  int nFlds = msg.mimeFieldsCount();

  logFile << std::endl << msgName << ':' << std::endl;

  if (nFlds) {
    MimeField iterFld(msg, 0);

    for (int i = 0; i < nFlds; ++i) {
      MimeField currFld(msg, i);

      ALWAYS_ASSERT(sameMimeField(currFld, iterFld));
      iterFld = iterFld.next();

      MimeField fFld(msg, currFld.nameGet());
      ALWAYS_ASSERT(fFld.valid());

      dumpMimeField(currFld);
    }
    ALWAYS_ASSERT(!iterFld.valid());
  }
}

void
doCrap(MsgBase &msg, bool add)
{
  MimeField f(msg, "x-crap");

  ALWAYS_ASSERT(f.valuesCount() == 3);
  ALWAYS_ASSERT(f.valGet(0) == "one");
  ALWAYS_ASSERT(f.valGet(1) == "two");
  ALWAYS_ASSERT(f.valGet(2) == "three");

  MimeField fd(f.nextDup());

  ALWAYS_ASSERT(fd.valuesCount() == 1);
  ALWAYS_ASSERT(fd.valGet(0) == "four");

  if (add) {
    fd.valInsert(0, "Three-And-A-Half");

    ALWAYS_ASSERT(fd.valuesCount() == 2);

    fd.valAppend("five");
    fd.valSet(1, "cuatro");

    ALWAYS_ASSERT(fd.valuesCount() == 3);
    ALWAYS_ASSERT(fd.valGet(0) == "Three-And-A-Half");
    ALWAYS_ASSERT(fd.valGet(1) == "cuatro");
    ALWAYS_ASSERT(fd.valGet(2) == "five");

    MimeField ld = MimeField::lastDup(fd.msg(), "X-Crap");
    ALWAYS_ASSERT(sameMimeField(fd, ld));
  }
}

int
globalContFunc(TSCont, TSEvent event, void *eventData)
{
  logFile << "Global: event=" << TSHttpEventNameLookup(event) << std::endl;

  TSDebug(PIName, "Global: event=%s(%d) eventData=%p", TSHttpEventNameLookup(event), event, eventData);

  switch (event) {
  case TS_EVENT_HTTP_SEND_RESPONSE_HDR: {
    auto txn = static_cast<TSHttpTxn>(eventData);

    logFile << std::endl;
    logFile << "Remap From URL: " << txnRemapFromUrlStringGet(txn).asStringView() << std::endl;
    logFile << "Remap To   URL: " << txnRemapToUrlStringGet(txn).asStringView() << std::endl;
    logFile << "Effective  URL: " << txnEffectiveUrlStringGet(txn).asStringView() << std::endl;

    // This block ensures destruction of objects before the Transaction reenable call.
    {
      TxnClientReq clientReq(txn);
      sameMimeField(MimeField(clientReq, "Host"), MimeField::lastDup(clientReq, "Host"));
      doCrap(clientReq, false);
      dumpMsg(clientReq, "Client Request");

      TxnClientResp clientResp(txn);
      dumpMsg(clientResp, "Client Response");

      TxnServerReq serverReq(txn);
      doCrap(serverReq, true);
      dumpMsg(serverReq, "Server Request");

      TxnServerResp serverResp(txn);
      dumpMsg(serverResp, "Server Response");
    }

    TSHttpTxnReenable(txn, TS_EVENT_HTTP_CONTINUE);
  } break;

  default: {
    ALWAYS_ASSERT(false)
  } break;

  } // end switch

  return 0;
}

TSCont gCont;

} // end anonymous namespace

void
TSPluginInit(int argc, const char *argv[])
{
  TSPluginRegistrationInfo info;

  info.plugin_name   = PIName;
  info.vendor_name   = "Apache Software Foundation";
  info.support_email = "dev@trafficserver.apache.org";

  if (TSPluginRegister(&info) != TS_SUCCESS) {
    TSError(PINAME ": Plugin registration failed");

    return;
  }

  const char *fileSpec = std::getenv("OUTPUT_FILE");

  if (nullptr == fileSpec) {
    TSError(PINAME ": Environment variable OUTPUT_FILE not found.");

    return;
  }

  // Disable output buffering for logFile, so that explicit flushing is not necessary.
  logFile.rdbuf()->pubsetbuf(nullptr, 0);

  logFile.open(fileSpec, std::ios::out);
  if (!logFile.is_open()) {
    TSError(PINAME ": could not open log file \"%s\"", fileSpec);

    return;
  }

  gCont = TSContCreate(globalContFunc, nullptr);

  // Setup the global hook
  TSHttpHookAdd(TS_HTTP_SEND_RESPONSE_HDR_HOOK, gCont);
}
