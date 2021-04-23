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
    "body": "xxx\n"
}
server = Test.MakeOriginServer("server")
server.addResponse("sessionlog.json", request_header, response_header)

dns = Test.MakeDNServer("dns")

# Define next hop trafficserver instances.
#
num_nh = 8
ts_nh = []
for i in range(num_nh):
    ts = Test.MakeATSProcess("ts_nh" + str(i))
    ts.Disk.records_config.update({
        'proxy.config.diags.debug.enabled': 1,
        'proxy.config.diags.debug.tags': 'http|dns',
        'proxy.config.dns.nameservers': f"127.0.0.1:{dns.Variables.Port}",
        'proxy.config.dns.resolv_conf': "NULL",
    })
    ts.Disk.remap_config.AddLine(
        f"map / http://127.0.0.1:{server.Variables.Port}"
    )
    ts_nh.append(ts)

#ts = Test.MakeATSProcess("ts", block_for_debug=True)
ts = Test.MakeATSProcess("ts")

ts.Disk.records_config.update({
    'proxy.config.diags.debug.enabled': 1,
    'proxy.config.diags.debug.tags': 'http|dns|parent|next_hop|host_statuses|hostdb',
    'proxy.config.dns.nameservers': f"127.0.0.1:{dns.Variables.Port}", # Only nameservers if resolv_conf NULL.
    'proxy.config.dns.resolv_conf': "NULL", # This defaults to /etc/resvolv.conf (OS namesevers) if not NULL.
    'proxy.config.http.cache.http': 0,
    'proxy.config.http.uncacheable_requests_bypass_parent': 0,
    'proxy.config.http.no_dns_just_forward_to_parent': 0, # Setting this to 1 seems to imply that parent.config
                                                          # has to exist.
    'proxy.config.http.parent_proxy.mark_down_hostdb': 0,
    'proxy.config.http.parent_proxy.self_detect': 0,
})

if True:
    ts.Disk.File(ts.Variables.CONFIGDIR + "/strategies.yaml", id="strategies", typename="ats:config")
    s = ts.Disk.strategies
    s.AddLine("groups:")
    s.AddLine("  - &g1")
    for i in range(num_nh):
        s.AddLine(f"    - host: foo{i}")
        dns.addRecords(records={f"foo{i}": ["127.0.0.1"]})
        s.AddLine( "      protocol:")
        s.AddLine( "        - scheme: http")
        s.AddLine(f"          port: {ts_nh[i].Variables.port}")
        # The health check URL does not seem to be optional, the host will never be marked up without it.
        s.AddLine(f"          health_check_url: http://foo{i}:{ts_nh[i].Variables.port}")
        s.AddLine( "      weight: 1.0")
    s.AddLine( "")
    s.AddLine("strategies:")
    s.AddLine("  - strategy: the-strategy")
    s.AddLine("    policy: consistent_hash")
    s.AddLine("    hash_key: path")
    s.AddLine("    go_direct: false")
    s.AddLine("    parent_is_proxy: true")
    s.AddLine("    ignore_self_detect: true")
    s.AddLine("    groups:")
    s.AddLine("      - *g1")
    s.AddLine("    scheme: http")
    #s.AddLine("    fallover:")
    #s.AddLine("      max_simple_retries: 2")
    #s.AddLine("      ring_mode: exhaust_ring")
    #s.AddLine("      response_codes:")
    #s.AddLine("        - 404")
    #s.AddLine("      health_check:")
    #s.AddLine("        - passive")

ts.Disk.remap_config.AddLine(
    "map http://dummy.com http://not_used @strategy=the-strategy"
)
dns.addRecords(records={"not_used": ["127.0.0.1"]})

# TEMP
# Configure comparison with gold file at end of test.
#
# Test.Disk.File(os.path.join(ts.Variables.LOGDIR, 'test_all_headers.log.san'),
#               exists=True, content='gold/test_all_headers.gold')

tr = Test.AddTestRun()
tr.Processes.Default.StartBefore(server)
tr.Processes.Default.StartBefore(dns)
for i in range(num_nh):
    tr.Processes.Default.StartBefore(ts_nh[i])
tr.Processes.Default.StartBefore(Test.Processes.ts)
tr.Processes.Default.Command = 'echo start TS, dummy TS, server and next hop TSes'
tr.Processes.Default.ReturnCode = 0

tr = Test.AddTestRun()
tr.Processes.Default.Command = (
    f'curl --verbose --proxy 127.0.0.1:{ts.Variables.port} http://dummy.com/obj'
)
tr.Processes.Default.ReturnCode = 0
