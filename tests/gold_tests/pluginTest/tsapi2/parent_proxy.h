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

namespace ParentProxyTest
{
enum TxnID { PARENT_PROXY, PARENT_PROXY_FAIL, SIZE };

Logger log;

TSCont cont{nullptr};

void *test_data;

// The continuatation data will be an array of two instances of this class.  The first is for the successful
// transaction, the second is for the transaction that should fail.
//
struct ContData {
  TxnID txn_id{PARENT_PROXY};
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
  if (TSContDataGet(contp) != test_data) {
    // Ignore events for global hooks for other tests.
    //
    reenable(event, event_data);
    return 0;
  }

  TSReleaseAssert(contp == cont);

  auto data = static_cast<ContData *>(test_data);

  TSReleaseAssert(event_data != nullptr);

  auto txn = static_cast<TSHttpTxn>(event_data);

  switch (event) {
  case TS_EVENT_HTTP_READ_REQUEST_HDR: {
    data->test(checkHttpTxnReqOrResp(log, txn, TSHttpTxnClientReqGet, "client request", 11));

    // Since we chose a request format with an invalid hostname, it won't get sent to the userver unless we set
    // a parent proxy.
    TSHttpTxnParentProxySet(txn, "127.0.0.1", TxnID::PARENT_PROXY_FAIL == data->txn_id ? Mute_server_port : Server_port);

    TSHttpTxnHookAdd(txn, TS_HTTP_SEND_RESPONSE_HDR_HOOK, cont);
    TSHttpTxnHookAdd(txn, TS_HTTP_TXN_CLOSE_HOOK, cont);

    TSSkipRemappingSet(txn, 1);
  } break;

  case TS_EVENT_HTTP_SEND_RESPONSE_HDR: {
    if (TxnID::PARENT_PROXY_FAIL == data->txn_id) {
      data->test(checkHttpTxnReqOrResp(log, txn, TSHttpTxnClientRespGet, "response to client", -1, TS_HTTP_STATUS_BAD_GATEWAY));
    } else {
      data->test(checkHttpTxnReqOrResp(log, txn, TSHttpTxnClientRespGet, "response to client", 11, TS_HTTP_STATUS_OK));
    }
  } break;

  case TS_EVENT_HTTP_TXN_CLOSE: {
    incrementEnum(data->txn_id);
    if (TxnID::SIZE == data->txn_id) {
      log(data->good ? "parent proxy test ok" : "parent proxy test failed");
      log.flush();
    }
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
  log.open(Run_dir_path + "/ParentProxyTest.tlog");

  cont = TSContCreate(contFunc, nullptr);

  test_data = TSmalloc(sizeof(ContData));

  ::new (test_data) ContData;

  TSContDataSet(cont, test_data);

  TSHttpHookAdd(TS_HTTP_READ_REQUEST_HDR_HOOK, cont);
}

void
cleanup()
{
  TSfree(test_data);

  TSContDestroy(cont);

  log.close();
}

} // namespace ParentProxyTest
