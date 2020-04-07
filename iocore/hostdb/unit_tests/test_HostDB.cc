/** @file

  Catch based unit tests for IOBuffer

  @section license License

  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <future>
#include <chrono>

#include <I_EventSystem.h>
#include <I_HostDBProcessor.h>
#include <RecordsConfig.h>
#include <tscore/I_Layout.h>

#include "P_HostDB.h"

namespace
{
constexpr int HOSTDB_TEST_MAX_OUTSTANDING = 20;
constexpr int HOSTDB_TEST_LENGTH          = 200;

struct HostDBTestReverse : public Continuation {
  int outstanding = 0;
  int total       = 0;
  std::ranlux48 randu;

  int
  mainEvent(int event, void *e)
  {
    if (event == EVENT_HOST_DB_LOOKUP) {
      HostDBInfo *i = static_cast<HostDBInfo *>(e);
      if (i) {
        std::printf("HostDBTestReverse: reversed %s\n", i->hostname());
      }
      outstanding--;
    }
    while (outstanding < HOSTDB_TEST_MAX_OUTSTANDING && total < HOSTDB_TEST_LENGTH) {
      IpEndpoint ip;
      ip.assign(IpAddr(static_cast<in_addr_t>(randu())));
      outstanding++;
      total++;
      if (!(outstanding % 100)) {
        std::printf("HostDBTestReverse: %d\n", total);
      }
      hostDBProcessor.getbyaddr_re(this, &ip.sa);
    }
    if (!outstanding) {
      std::printf("HostDBTestReverse: done\n");
      _prom.set_value();
      delete this;
    }
    return EVENT_CONT;
  }
  HostDBTestReverse(std::promise<void> &prom) : Continuation(new_ProxyMutex()), _prom(prom)
  {
    SET_HANDLER(&HostDBTestReverse::mainEvent);
    randu.seed(std::chrono::system_clock::now().time_since_epoch().count());
  }

private:
  std::promise<void> &_prom;
};

} // end anonymous namespace

TEST_CASE("HostDBTests", "[hostdb][tests]")
{
  std::promise<void> prom;
  auto fut = prom.get_future();
  eventProcessor.schedule_imm(new HostDBTestReverse(prom), ET_CALL);
  REQUIRE(std::future_status::ready == fut.wait_for(std::chrono::seconds(3)));
}

#if 0  // TEMP
/// Pair of IP address and host name from a host file.
struct HostFilePair {
  using self = HostFilePair;
  IpAddr ip;
  const char *name;
};

struct HostDBFileContinuation : public Continuation {
  using self = HostDBFileContinuation;
  using Keys = std::vector<CryptoHash>;

  int idx          = 0;       ///< Working index.
  const char *name = nullptr; ///< Host name (just for debugging)
  Keys *keys       = nullptr; ///< Entries from file.
  CryptoHash hash;            ///< Key for entry.
  ats_scoped_str path;        ///< Used to keep the host file name around.

  HostDBFileContinuation() : Continuation(nullptr) {}
  /// Finish update
  static void finish(Keys *keys ///< Valid keys from update.
  );
  /// Clean up this instance.
  void destroy();
};

ClassAllocator<HostDBFileContinuation> hostDBFileContAllocator("hostDBFileContAllocator");

void
HostDBFileContinuation::destroy()
{
  this->~HostDBFileContinuation();
  hostDBFileContAllocator.free(this);
}

// Host file processing globals.

// We can't allow more than one update to be
// proceeding at a time in any case so we might as well make these
// globals.
int HostDBFileUpdateActive = 0;

static void
ParseHostLine(Ptr<RefCountedHostsFileMap> &map, char *l)
{
  Tokenizer elts(" \t");
  int n_elts = elts.Initialize(l, SHARE_TOKS);

  // Elements should be the address then a list of host names.
  // Don't use RecHttpLoadIp because the address *must* be literal.
  IpAddr ip;
  if (n_elts > 1 && 0 == ip.load(elts[0])) {
    for (int i = 1; i < n_elts; ++i) {
      ts::ConstBuffer name(elts[i], strlen(elts[i]));
      // If we don't have an entry already (host files only support single IPs for a given name)
      if (map->hosts_file_map.find(name) == map->hosts_file_map.end()) {
        map->hosts_file_map[name] = ip;
      }
    }
  }
}

void
ParseHostFile(const char *path, unsigned int hostdb_hostfile_check_interval_parse)
{
  Ptr<RefCountedHostsFileMap> parsed_hosts_file_ptr;

  // Test and set for update in progress.
  if (0 != ink_atomic_swap(&HostDBFileUpdateActive, 1)) {
    Debug("hostdb", "Skipped load of host file because update already in progress");
    return;
  }
  Debug("hostdb", "Loading host file '%s'", path);

  if (*path) {
    ats_scoped_fd fd(open(path, O_RDONLY));
    if (fd >= 0) {
      struct stat info;
      if (0 == fstat(fd, &info)) {
        // +1 in case no terminating newline
        int64_t size = info.st_size + 1;

        parsed_hosts_file_ptr                 = new RefCountedHostsFileMap;
        parsed_hosts_file_ptr->next_sync_time = ink_time() + hostdb_hostfile_check_interval_parse;
        parsed_hosts_file_ptr->HostFileText   = static_cast<char *>(ats_malloc(size));
        if (parsed_hosts_file_ptr->HostFileText) {
          char *base = parsed_hosts_file_ptr->HostFileText;
          char *limit;

          size   = read(fd, parsed_hosts_file_ptr->HostFileText, info.st_size);
          limit  = parsed_hosts_file_ptr->HostFileText + size;
          *limit = 0;

          // We need to get a list of all name/addr pairs so that we can
          // group names for round robin records. Also note that the
          // pairs have pointer back in to the text storage for the file
          // so we need to keep that until we're done with @a pairs.
          while (base < limit) {
            char *spot = strchr(base, '\n');

            // terminate the line.
            if (nullptr == spot) {
              spot = limit; // no trailing EOL, grab remaining
            } else {
              *spot = 0;
            }

            while (base < spot && isspace(*base)) {
              ++base; // skip leading ws
            }
            if (*base != '#' && base < spot) { // non-empty non-comment line
              ParseHostLine(parsed_hosts_file_ptr, base);
            }
            base = spot + 1;
          }

          hostdb_hostfile_update_timestamp = hostdb_current_interval;
        }
      }
    }
  }

  // Swap the pointer
  if (parsed_hosts_file_ptr != nullptr) {
    hostDB.hosts_file_ptr = parsed_hosts_file_ptr;
  }
  // Mark this one as completed, so we can allow another update to happen
  HostDBFileUpdateActive = 0;
}

// Take a started hostDB and fill it up and make sure it doesn't explode
// TEMP struct HostDBRegressionContinuation;

struct HostDBRegressionContinuation : public Continuation {
  int hosts;
  const char **hostnames;
  RegressionTest *test;
  int type;
  int *status;

  int success;
  int failure;
  int outstanding;
  int i;

  int
  mainEvent(int event, HostDBInfo *r)
  {
    (void)event;

    if (event == EVENT_INTERVAL) {
      std::printf("hosts=%d success=%d failure=%d outstanding=%d i=%d\n", hosts, success, failure, outstanding, i);
    }
    if (event == EVENT_HOST_DB_LOOKUP) {
      --outstanding;
      // since this is a lookup done, data is either hostdbInfo or nullptr
      if (r) {
        std::printf("hostdbinfo r=%x\n", r);
        std::printf("hostdbinfo hostname=%s\n", r->perm_hostname());
        std::printf("hostdbinfo rr %x\n", r->rr());
        // If RR, print all of the enclosed records
        if (r->rr()) {
          std::printf("hostdbinfo good=%d\n", r->rr()->good);
          for (int x = 0; x < r->rr()->good; x++) {
            ip_port_text_buffer ip_buf;
            ats_ip_ntop(r->rr()->info(x).ip(), ip_buf, sizeof(ip_buf));
            std::printf("hostdbinfo RR%d ip=%s\n", x, ip_buf);
          }
        } else { // Otherwise, just the one will do
          ip_port_text_buffer ip_buf;
          ats_ip_ntop(r->ip(), ip_buf, sizeof(ip_buf));
          std::printf("hostdbinfo A ip=%s\n", ip_buf);
        }
        ++success;
      } else {
        ++failure;
      }
    }

    if (i < hosts) {
      hostDBProcessor.getbyname_re(this, hostnames[i++], 0);
      return EVENT_CONT;
    } else {
      std::printf("HostDBTestRR: %d outstanding %d success %d failure\n", outstanding, success, failure);
      if (success == hosts) {
        *status = REGRESSION_TEST_PASSED;
      } else {
        *status = REGRESSION_TEST_FAILED;
      }
      return EVENT_DONE;
    }
  }

  HostDBRegressionContinuation(int ahosts, const char **ahostnames, RegressionTest *t, int atype, int *astatus)
    : Continuation(new_ProxyMutex()),
      hosts(ahosts),
      hostnames(ahostnames),
      test(t),
      type(atype),
      status(astatus),
      success(0),
      failure(0),
      i(0)
  {
    outstanding = ahosts;
    SET_HANDLER(&HostDBRegressionContinuation::mainEvent);
  }
};

static const char *dns_test_hosts[] = {
  "www.apple.com", "www.ibm.com", "www.microsoft.com",
  "www.coke.com", // RR record
  "4.2.2.2",      // An IP-- since we don't expect resolution
  "127.0.0.1",    // loopback since it has some special handling
};

TEST_CASE("HostDBProcessor", "[hostdb][proc]]")
{
  eventProcessor.schedule_in(new HostDBRegressionContinuation(6, dns_test_hosts, t, atype, pstatus), HRTIME_SECONDS(1));
}
#endif // TEMP

unsigned const TEST_THREADS{1};

struct EventProcessorListener : Catch::TestEventListenerBase {
  using TestEventListenerBase::TestEventListenerBase;

  void
  testRunStarting(Catch::TestRunInfo const &testRunInfo) override
  {
    Layout::create();
#if 0 // TEMP
    init_diags("", nullptr);
#endif
    RecProcessInit(RECM_STAND_ALONE);

    LibRecordsConfigInit();

    ink_hostdb_init(HOSTDB_MODULE_PUBLIC_VERSION);

    ink_event_system_init(EVENT_SYSTEM_MODULE_PUBLIC_VERSION);
    eventProcessor.start(TEST_THREADS);

    EThread *main_thread = new EThread;
    main_thread->set_specific();
  }
};

CATCH_REGISTER_LISTENER(EventProcessorListener);

// Stubs for linking.

#include <InkAPIInternal.h>
#include <HttpSessionManager.h>
#include <HttpBodyFactory.h>
#include <DiagsConfig.h>
#include <ts/InkAPIPrivateIOCore.h>

void
initialize_thread_for_http_sessions(EThread *, int)
{
  ink_assert(false);
}

void
APIHooks::append(INKContInternal *cont)
{
}

int
APIHook::invoke(int, void *) const
{
  ink_assert(false);
  return 0;
}

APIHook *
APIHook::next() const
{
  ink_assert(false);
  return nullptr;
}

APIHook *
APIHooks::head() const
{
  return nullptr;
}

void
APIHooks::clear()
{
}

HttpHookState::HttpHookState() {}

void
HttpHookState::init(TSHttpHookID id, HttpAPIHooks const *global, HttpAPIHooks const *ssn, HttpAPIHooks const *txn)
{
}

APIHook const *
HttpHookState::getNext()
{
  return nullptr;
}

void
ConfigUpdateCbTable::invoke(const char * /* name ATS_UNUSED */)
{
  ink_release_assert(false);
}

HttpAPIHooks *http_global_hooks        = nullptr;
SslAPIHooks *ssl_hooks                 = nullptr;
LifecycleAPIHooks *lifecycle_hooks     = nullptr;
ConfigUpdateCbTable *global_config_cbs = nullptr;

HttpBodyFactory *body_factory = nullptr;

intmax_t
ts::svtoi(TextView src, TextView *out, int base)
{
  intmax_t zret = 0;

  if (out) {
    out->clear();
  }
  if (!(0 <= base && base <= 36)) {
    return 0;
  }
  if (src.ltrim_if(&isspace) && src) {
    const char *start = src.data();
    int8_t v;
    bool neg = false;
    if ('-' == *src) {
      ++src;
      neg = true;
    }
    // If base is 0, it wasn't specified - check for standard base prefixes
    if (0 == base) {
      base = 10;
      if ('0' == *src) {
        ++src;
        base = 8;
        if (src && ('x' == *src || 'X' == *src)) {
          ++src;
          base = 16;
        }
      }
    }

    // For performance in common cases, use the templated conversion.
    switch (base) {
    case 8:
      zret = svto_radix<8>(src);
      break;
    case 10:
      zret = svto_radix<10>(src);
      break;
    case 16:
      zret = svto_radix<16>(src);
      break;
    default:
      while (src.size() && (0 <= (v = svtoi_convert[static_cast<unsigned char>(*src)])) && v < base) {
        auto n = zret * base + v;
        if (n < zret) {
          zret = std::numeric_limits<uintmax_t>::max();
          break; // overflow, stop parsing.
        }
        zret = n;
        ++src;
      }
      break;
    }

    if (out && (src.data() > (neg ? start + 1 : start))) {
      out->assign(start, src.data());
    }

    if (neg) {
      zret = -zret;
    }
  }
  return zret;
}

void
HostStatus::setHostStatus(const char *name, HostStatus_t status, const unsigned int down_time, const unsigned int reason)
{
}

HostStatRec *
HostStatus::getHostStatus(const char *name)
{
  return nullptr;
}

void
HostStatus::createHostStat(const char *name, const char *data)
{
}

HostStatus::HostStatus() {}

HostStatus::~HostStatus() {}

int auto_clear_hostdb_flag = 0;
bool ts_is_draining        = false;

void
INKVConnInternal::do_io_close(int error)
{
}

void
INKVConnInternal::do_io_shutdown(ShutdownHowTo_t howto)
{
}

VIO *
INKVConnInternal::do_io_write(Continuation *c, int64_t nbytes, IOBufferReader *buf, bool owner)
{
  return nullptr;
}

VIO *
INKVConnInternal::do_io_read(Continuation *c, int64_t nbytes, MIOBuffer *buf)
{
  return nullptr;
}

void
INKVConnInternal::destroy()
{
}

void
INKVConnInternal::free()
{
}

void
INKVConnInternal::clear()
{
}

void
INKVConnInternal::reenable(VIO * /* vio ATS_UNUSED */)
{
}

bool
INKVConnInternal::get_data(int id, void *data)
{
  return false;
}

bool
INKVConnInternal::set_data(int id, void *data)
{
  return false;
}

void
INKVConnInternal::do_io_transform(VConnection *vc)
{
}

void
INKContInternal::handle_event_count(int event)
{
}

void
INKVConnInternal::retry(unsigned int delay)
{
}

INKContInternal::INKContInternal(TSEventFunc funcp, TSMutex mutexp) : DummyVConnection(reinterpret_cast<ProxyMutex *>(mutexp)) {}

INKContInternal::INKContInternal() : DummyVConnection(nullptr) {}

void
INKContInternal::destroy()
{
}

void
INKContInternal::clear()
{
}

void
INKContInternal::free()
{
}

INKVConnInternal::INKVConnInternal() : INKContInternal() {}

INKVConnInternal::INKVConnInternal(TSEventFunc funcp, TSMutex mutexp) : INKContInternal(funcp, mutexp) {}

namespace test_HostDB_dummy
{
// The linker can seem to grok that libproxy depends on HdrToken.o in libhdrs.  This definition is needed to cause
// the linker to pull in HdrToken.o .
//
int dummy_ = hdrtoken_num_wks;

// The linker can seem to grok that libproxy depends on HTTP.o in libhdrs.  This definition is needed to cause
// the linker to pull in HTTP.o .
//
void (*dummy2_)() = http_init;

} // end namespace test_HostDB_dummy

#if 0 // TEMP

int auto_clear_hostdb_flag = 0;

ConfigUpdateCbTable *global_config_cbs = nullptr;

void
ConfigUpdateCbTable::invoke(const char * /* name ATS_UNUSED */)
{
  ink_release_assert(false);
}

#endif

// End stubs for linking.
