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
Test vague linkage and work-around.
'''
ts = Test.MakeATSProcess("ts", select_ports=True, enable_tls=True)

ts.Disk.records_config.update({
    'proxy.config.diags.debug.enabled': 1,
    'proxy.config.diags.debug.tags': 'vague_linkage1|vague_linkage2',
})

'''
tr = Test.AddTestRun()
tr.Processes.Default.Command = (
    "cp " + Test.TestDirectory + "/vague_linkage.h " + Test.RunDirectory 
)
tr.Processes.Default.ReturnCode = 0
'''

def one_plugin(name):
    plugin_filespec = Test.RunDirectory + "/" + name + ".so"

    ts.Disk.plugin_config.AddLine(plugin_filespec)

    tr = Test.AddTestRun()
    tr.Processes.Default.Command = (
        "cp " + Test.TestDirectory + "/" + name + ".cc " + Test.RunDirectory +
        " ; export CC=c++ && " +
        Test.Variables.BINDIR + "/tsxs " +
        "-L " + ts.Variables.LIBDIR +
        " -c " + Test.RunDirectory + "/" + name + ".cc "
        " -o " + plugin_filespec
    )
    tr.Processes.Default.ReturnCode = 0

one_plugin("vague_linkage1")
one_plugin("vague_linkage2")

tr = Test.AddTestRun()
# Probe TS cleartext port to check if ready.
tr.Processes.Default.StartBefore(Test.Processes.ts, ready=When.PortOpen(ts.Variables.port))
tr.Processes.Default.Command = 'echo "TS ready"'
tr.StillRunningAfter = ts
tr.Processes.Default.ReturnCode = 0
