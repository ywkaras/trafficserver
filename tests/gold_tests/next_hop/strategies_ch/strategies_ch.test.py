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

Test.Summary = '''
Test next hop selection using strategies.yaml with consistent hashing.
'''

import socket

this_ip = socket.gethostbyname(socket.gethostname())

# Define MicroServer.
#
request_header = {
    "headers":
        "GET /obj HTTP/1.1\r\n"
        "Host: does.not.matter\r\n" # But cannot be omitted.
        "\r\n",
    "timestamp": "1469733493.993",
    "body": ""
}
response_header = {
    "headers":
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Cache-control: max-age=85000\r\n"
        "\r\n",
    "timestamp": "1469733493.993",
    "body": "xxx"
}
server = Test.MakeOriginServer("server")
server.addResponse("sessionlog.json", request_header, response_header)

# Define next hop trafficserver instances.
#
num_nh = 8
ts_nh = []
for i in range(num_nh):
    ts = Test.MakeATSProcess("ts_nh" + str(i))
    ts.Disk.remap_config.AddLine(
        f"map / http://127.0.0.1:{server.Variables.Port}"
    )
    ts.Disk.records_config.update({
        'proxy.config.diags.debug.enabled': 1,
        'proxy.config.diags.debug.tags': 'http|dns',
    })
    ts_nh.append(ts)

ts = Test.MakeATSProcess("ts")

ts.Disk.records_config.update({
    'proxy.config.diags.debug.enabled': 1,
    'proxy.config.diags.debug.tags': 'http|dns|next_hop|host_statuses',
    'proxy.config.http.cache.http': 1, # Next hop selection doesn't work with non-cacheable content.
})

ts.Disk.File(ts.Variables.CONFIGDIR + "/strategies.yaml", id="strategies", typename="ats:config")
s = ts.Disk.strategies
s.AddLine("groups:")
s.AddLine("  - &g1")
for i in range(num_nh):
    s.AddLine(f"    - n{i}:")
    s.AddLine(f"      host: {this_ip}")
    s.AddLine( "      protocol:")
    s.AddLine( "        - scheme: http")
    s.AddLine(f"        - port: {ts_nh[i].Variables.port}")
    # TEMP s.AddLine(f"        - health_check_url: http://{this_ip}:{ts_nh[i].Variables.port}")
    s.AddLine( "      weight: 1.0")
    s.AddLine( "")
s.AddLine("strategies:")
s.AddLine("  - strategy: the-strategy")
s.AddLine("    policy: consistent_hash")
s.AddLine("    hash_key: cache_key")
s.AddLine("    go_direct: false")
s.AddLine("    ignore_self_detect: true")
s.AddLine("    groups:")
s.AddLine("      - *g1")
s.AddLine("    scheme: http")

ts.Disk.remap_config.AddLine(
    # f"map http://dummy.com http://127.0.0.1:{server.Variables.Port}"
    "map http://dummy.com http://wrong.com"
    + " @strategy=the-strategy"
)

# Configure comparison with gold file at end of test.
#
# Test.Disk.File(os.path.join(ts.Variables.LOGDIR, 'test_all_headers.log.san'),
#               exists=True, content='gold/test_all_headers.gold')

tr = Test.AddTestRun()
tr.Processes.Default.StartBefore(server)
for i in range(num_nh):
    tr.Processes.Default.StartBefore(ts_nh[i])
tr.Processes.Default.StartBefore(Test.Processes.ts)
tr.Processes.Default.Command = 'echo start TS, server and next hop TSes'
tr.Processes.Default.ReturnCode = 0

tr = Test.AddTestRun()
tr.Processes.Default.Command = (
    f'curl --verbose --proxy 127.0.0.1:{ts.Variables.port} http://dummy.com/obj'
)
tr.Processes.Default.ReturnCode = 0
