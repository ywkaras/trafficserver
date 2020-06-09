/* Licensed to the Apache Software Foundation (ASF) under one
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

// Unit Test for API: TSHttpTxnCachedReqGet
//                    TSHttpTxnCachedRespGet
//                    TSHttpAltInfoClientReqGet
//                    TSHttpAltInfoCachedReqGet
//                    TSHttpAltInfoCachedRespGet
//                    TSHttpAltInfoQualitySet
//
namespace AltInfoTest
{
Logger log;

TSCont cont{nullptr};

struct ContData {
  bool good{true};
  void
  test(bool result)
  {
    good = good && result;
  }
};

int
contFunc(TSCont contp, TSEvent event, void *event_data)
{
  TSReleaseAssert(event_data != nullptr);

  auto txn = static_cast<TSHttpTxn>(event_data);

  auto test_id = GetTxnID(txn);
  if ((test_id != TxnID::ALT_INFO1) && (test_id != TxnID::ALT_INFO2) && (test_id != TxnID::ALT_INFO3)) {
    TSHttpTxnReenable(txn, TS_EVENT_HTTP_CONTINUE);
    return 0;
  }

  TSReleaseAssert(contp == cont);

  auto data = static_cast<ContData *>(TSContDataGet(contp));

  switch (event) {
  case TS_EVENT_HTTP_READ_REQUEST_HDR: {
    TSHttpTxnHookAdd(txn, TS_HTTP_TXN_CLOSE_HOOK, cont);
    TSSkipRemappingSet(txn, 1);
  } break;

  case TS_EVENT_HTTP_SELECT_ALT: {
    if (test_id != TxnID::ALT_INFO3) {
      log("SELET_ALT triggered for wrong test (%d)", static_cast<int>(test_id));

      // } else {
      // TEMP data->test(checkHttpTxnReqOrResp(log, txn, TSHttpTxnCachedReqGet, "cached request", 2));
    }
  } break;

  case TS_EVENT_HTTP_TXN_CLOSE: {
    if (TxnID::ALT_INFO3 == test_id) {
      log(data->good ? "Alt Info test ok" : "Alt Info test failed");
    }
    log.flush();
  } break;

  default: {
    TSError("Unexpected event %d", event);
    TSReleaseAssert(false);
  } break;
  } // end switch

  TSHttpTxnReenable(txn, TS_EVENT_HTTP_CONTINUE);
  return 0;
}

void
init()
{
  log.open(Run_dir_path + "/AltInfoTest.tlog");

  cont = TSContCreate(contFunc, nullptr);

  auto data = static_cast<ContData *>(TSmalloc(sizeof(ContData)));

  ::new (data) ContData;

  TSContDataSet(cont, data);

  TSHttpHookAdd(TS_HTTP_READ_REQUEST_HDR_HOOK, cont);
}

void
cleanup()
{
  TSfree(TSContDataGet(cont));

  TSContDestroy(cont);

  log.close();
}

} // namespace AltInfoTest
