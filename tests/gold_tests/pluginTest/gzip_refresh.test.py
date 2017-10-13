'''
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
Test.Summary = '''
ARGH
'''
# need Curl
Test.SkipUnless(
    Condition.HasProgram("curl", "Curl need to be installed on system for this test to work")
)
Test.ContinueOnFail = True
# Define default ATS
ts = Test.MakeATSProcess("ts")
server = Test.MakeOriginServer("server")

request_header = {"headers": "GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n", "timestamp": "1469733493.993", "body": ""}
# expected response from the origin server
response_header = {
    "headers": "HTTP/1.1 200 OK\r\nConnection: close\r\nCache-Control: max-age=1,stale-while-revalidate=1000\r\n\r\n",
    "timestamp": "1469733493.993", "body": ""}

# add response to the server dictionary
server.addResponse("sessionfile.log", request_header, response_header)

ts.Disk.records_config.update({
    'proxy.config.diags.debug.enabled': 1, # TEMP
    'proxy.config.http.server_ports': 'ipv4:{0}'.format(ts.Variables.port)
    # 'proxy.config.diags.debug.tags': 'http.*|dns',
})

ts.Disk.logging_config.AddLines(
    '''custom = format {
  Format = "%<cqtq> %<ttms> %<chi> %<crc>/%<pssc> %<psql> %<cqhm> %<cquc> %<caun> %<phr>/%<pqsn> %<psct>"
}

log.ascii {
  Format = custom,
  Filename = 'gzip_refresh'
}'''.split("\n")
)

ts.Disk.remap_config.AddLine(
    'map http://127.0.0.1:{0} http://www.example.com'.format(ts.Variables.port)
)

tr = Test.AddTestRun()
tr.Processes.Default.ReturnCode = 0
tr.Processes.Default.Command = 'curl --compressed "http://127.0.0.1:{0}" --verbose'.format(ts.Variables.port)
tr.Processes.Default.StartBefore(server)
tr.Processes.Default.StartBefore(Test.Processes.ts)
tr.StillRunningAfter = server

tr = Test.AddTestRun()
tr.Processes.Default.ReturnCode = 0
tr.Processes.Default.Command = 'curl --compressed "http://127.0.0.1:{0}" --verbose'.format(ts.Variables.port)
tr.StillRunningAfter = server
tr.DelayStart = 5

# Wait for log entries to be generated.
tr = Test.AddTestRun()
tr.Processes.Default.ReturnCode = 1 # 0 TEMP force failture so _sandbox won't go away
tr.Processes.Default.Command = 'echo WAIT'
tr.DelayStart = 10
