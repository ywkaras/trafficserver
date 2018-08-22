'''
Test effects on caching of accept-encoding header normalization.
'''
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

import os
import subprocess

Test.Summary = '''
Test effects on caching of accept-encoding header normalization.
'''

Test.SkipUnless(
    Condition.HasProgram("curl", "Curl need to be installed on system for this test to work")
)

server = Test.MakeOriginServer("server", options={'--load': os.path.join(Test.TestDirectory, 'normalize_ae2_obs.py')})

request_header = {"headers": "GET /argh HTTP/1.1\r\nHost: doesnotmatter\r\n\r\n", "timestamp": "1469733493.993", "body": ""}
body = '12345678'
body = body + body # 16
body = body + body # 32
body = body + body # 64
body = body + body # 128
response_header = {
  "headers": ("HTTP/1.1 200 OK\r\n" +
              "Cache-Control: max-age=32000000\r\n" +
              "Vary: Accept-Encoding\r\n" +
              "\r\n"),
  "timestamp": "1469733493.993",
  "body": body
}
server.addResponse("sessionlog.json", request_header, response_header)

ts = Test.MakeATSProcess("ts")

ts.Disk.records_config.update({
    'proxy.config.diags.debug.enabled': 1,
    'proxy.config.diags.debug.tags': 'http|cache',
    'proxy.config.http.cache.http': 1,
    'proxy.config.http.server_ports': 'ipv4:{}'.format(ts.Variables.port)
})
ts.Disk.remap_config.AddLine(
    'map http://www.ae-0.com http://127.0.0.1:{0}'.format(server.Variables.Port) +
    ' @plugin=conf_remap.so @pparam=proxy.config.http.normalize_ae=0'
)
ts.Disk.remap_config.AddLine(
    'map http://www.ae-1.com http://127.0.0.1:{0}'.format(server.Variables.Port) +
    ' @plugin=conf_remap.so @pparam=proxy.config.http.normalize_ae=1'
)

# set up to check the output after the tests have run.
#
log_id = Test.Disk.File("obs.log")
log_id.Content = "obs.gold"
log_id = Test.Disk.File("curl.log")
log_id.Content = "curl.gold"

tr = Test.AddTestRun()

# wait for the micro server
tr.Processes.Default.StartBefore(server)

# wait for the micro server
# delay on readiness of port
tr.Processes.Default.StartBefore(ts)

tr.Processes.Default.Command = 'echo start stuff'
tr.Processes.Default.ReturnCode = 0

def sendCurl(suffix):
    tr = Test.AddTestRun()

    baseCurl = 'curl --verbose --ipv4 --http1.1 --proxy localhost:{} '.format(ts.Variables.port)
    cmd = baseCurl + suffix

    tr.Processes.Default.Command = (
        "echo >> {0}/curl_long.log ; echo '{1}' >> {0}/curl_long.log ; {1} >/dev/null 2>> {0}/curl_long.log".format(
            Test.RunDirectory, cmd))
    tr.Processes.Default.ReturnCode = 0

# sendCurl('--header "N: 1" --header "Accept-Encoding: deflate" http://www.ae-0.com/argh')
sendCurl('--header "N: 2" --header "Accept-Encoding: deflate" http://www.ae-0.com/argh')
sendCurl('--header "N: 2a" --header "Accept-Encoding: AlanWhatTheHell" http://www.ae-0.com/argh')
sendCurl('--header "N: 3" http://www.ae-0.com/argh')

sendCurl('--header "N: 4" --header "Accept-Encoding: deflate" http://www.ae-1.com/argh')
#sendCurl('--header "N: 5" --header "Accept-Encoding: deflate" http://www.ae-1.com/argh')
#sendCurl('--header "N: 6" http://www.ae-1.com/argh')

tr = test.AddTestRun()

tr.Processes.Default.Command = "grep '^[<>] ' {0}/curl_long.log > {0}/curl.log".format(Test.RunDirectory)
tr.Processes.Default.ReturnCode = 0
