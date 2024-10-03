-- Copyright (c) 2011 by Robert G. Jakabosky <bobby@sharedrealm.com>
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.

-- make generated variable nicer.
set_variable_format "%s%d"

c_module "zmq" {
-- module settings.
module_globals = true, -- support old code that doesn't do: local zmq = require"zmq"
use_globals = false,
hide_meta_info = true,
luajit_ffi = true,
-- needed for functions exported from module.
luajit_ffi_load_cmodule = true,

ffi_load {
"zmq", -- default lib name.
Windows = "libzmq", -- lib name for on windows.
},

sys_include "string.h",
include "zmq.h",

c_source "typedefs" [[
/* detect zmq version */
#define VERSION_2_0 1
#define VERSION_2_1 0
#define VERSION_2_2 0
#define VERSION_3_0 0
#define VERSION_3_2 0
#define VERSION_4_0 0
#if defined(ZMQ_VERSION_MAJOR)
#  if (ZMQ_VERSION_MAJOR == 2) && (ZMQ_VERSION_MINOR == 2)
#    undef VERSION_2_2
#    define VERSION_2_2 1
#    undef VERSION_2_1
#    define VERSION_2_1 1
#  endif
#  if (ZMQ_VERSION_MAJOR == 2) && (ZMQ_VERSION_MINOR == 1)
#    undef VERSION_2_1
#    define VERSION_2_1 1
#  endif
#  if (ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR == 3)
#    undef VERSION_2_0
#    define VERSION_2_0 0
#    undef VERSION_3_2
#    define VERSION_3_2 1
#    undef VERSION_3_0
#    define VERSION_3_0 1
#  endif
#  if (ZMQ_VERSION_MAJOR == 3) && (ZMQ_VERSION_MINOR == 2)
#    undef VERSION_2_0
#    define VERSION_2_0 0
#    undef VERSION_3_2
#    define VERSION_3_2 1
#    undef VERSION_3_0
#    define VERSION_3_0 1
#  endif
#  if (ZMQ_VERSION_MAJOR == 3)
#    undef VERSION_2_0
#    define VERSION_2_0 0
#    undef VERSION_3_0
#    define VERSION_3_0 1
#  endif
#  if (ZMQ_VERSION_MAJOR == 4)
#    undef VERSION_2_0
#    define VERSION_2_0 0
#    undef VERSION_3_2
#    define VERSION_3_2 0
#    undef VERSION_3_0
#    define VERSION_3_0 0
#    undef VERSION_4_0
#    define VERSION_4_0 1
#  endif
#endif

/* make sure ZMQ_DONTWAIT & ZMQ_NOBLOCK are both defined. */
#ifndef ZMQ_DONTWAIT
#  define ZMQ_DONTWAIT     ZMQ_NOBLOCK
#endif
#ifndef ZMQ_NOBLOCK
#  define ZMQ_NOBLOCK      ZMQ_DONTWAIT
#endif

/* make sure DEALER/ROUTER & XREQ/XREP are all defined. */
#ifndef ZMQ_DEALER
#  define ZMQ_DEALER ZMQ_XREQ
#endif
#ifndef ZMQ_ROUTER
#  define ZMQ_ROUTER ZMQ_XREP
#endif
#ifndef ZMQ_XREQ
#  define ZMQ_XREQ ZMQ_DEALER
#endif
#ifndef ZMQ_XREP
#  define ZMQ_XREP ZMQ_ROUTER
#endif

#if VERSION_2_0
#  define ZMQ_POLL_MSEC    1000 // zmq_poll is usec
#elif VERSION_3_0 || VERSION_4_0
#  define ZMQ_POLL_MSEC    1    // zmq_poll is msec
#  ifndef ZMQ_HWM
#    define ZMQ_HWM        1    // backwards compatibility
#  endif
#endif
]],

--
-- Module constants
--
export_definitions {
MAX_VSM_SIZE      = "ZMQ_MAX_VSM_SIZE",

-- context settings
MAX_SOCKETS	      = "ZMQ_MAX_SOCKETS",
IO_THREADS        = "ZMQ_IO_THREADS",

-- message types
DELIMITER         = "ZMQ_DELIMITER",
VSM               = "ZMQ_VSM",

-- message flags
MSG_MORE          = "ZMQ_MSG_MORE",
MSG_SHARED        = "ZMQ_MSG_SHARED",

-- socket types
PAIR              = "ZMQ_PAIR",
PUB               = "ZMQ_PUB",
SUB               = "ZMQ_SUB",
REQ               = "ZMQ_REQ",
REP               = "ZMQ_REP",
PULL              = "ZMQ_PULL",
PUSH              = "ZMQ_PUSH",

DEALER            = "ZMQ_DEALER",
ROUTER            = "ZMQ_ROUTER",
XREQ              = "ZMQ_XREQ",
XREP              = "ZMQ_XREP",

-- new 3.1 socket types
XPUB              = "ZMQ_XPUB",
XSUB              = "ZMQ_XSUB",

-- socket options
HWM               = "ZMQ_HWM",
SWAP              = "ZMQ_SWAP",
AFFINITY          = "ZMQ_AFFINITY",
IDENTITY          = "ZMQ_IDENTITY",
SUBSCRIBE         = "ZMQ_SUBSCRIBE",
UNSUBSCRIBE       = "ZMQ_UNSUBSCRIBE",
RATE              = "ZMQ_RATE",
RECOVERY_IVL      = "ZMQ_RECOVERY_IVL",
MCAST_LOOP        = "ZMQ_MCAST_LOOP",
SNDBUF            = "ZMQ_SNDBUF",
RCVBUF            = "ZMQ_RCVBUF",
RCVMORE           = "ZMQ_RCVMORE",
FD                = "ZMQ_FD",
EVENTS            = "ZMQ_EVENTS",
TYPE              = "ZMQ_TYPE",
LINGER            = "ZMQ_LINGER",
RECONNECT_IVL     = "ZMQ_RECONNECT_IVL",
RECONNECT_IVL_MSEC= "ZMQ_RECONNECT_IVL_MSEC",
BACKLOG           = "ZMQ_BACKLOG",
RECONNECT_IVL_MAX = "ZMQ_RECONNECT_IVL_MAX",
MAXMSGSIZE        = "ZMQ_MAXMSGSIZE",
SNDHWM            = "ZMQ_SNDHWM",
RCVHWM            = "ZMQ_RCVHWM",
MULTICAST_HOPS    = "ZMQ_MULTICAST_HOPS",
RCVTIMEO          = "ZMQ_RCVTIMEO",
SNDTIMEO          = "ZMQ_SNDTIMEO",
RCVLABEL          = "ZMQ_RCVLABEL",
LAST_ENDPOINT     = "ZMQ_LAST_ENDPOINT",
ROUTER_MANDATORY  = "ZMQ_ROUTER_MANDATORY",
TCP_KEEPALIVE     = "ZMQ_TCP_KEEPALIVE",
TCP_KEEPALIVE_CNT = "ZMQ_TCP_KEEPALIVE_CNT",
TCP_KEEPALIVE_IDLE= "ZMQ_TCP_KEEPALIVE_IDLE",
TCP_KEEPALIVE_INTVL= "ZMQ_TCP_KEEPALIVE_INTVL",
TCP_ACCEPT_FILTER = "ZMQ_TCP_ACCEPT_FILTER",
IMMEDIATE         = "ZMQ_IMMEDIATE",
XPUB_VERBOSE      = "ZMQ_XPUB_VERBOSE",
ROUTER_RAW        = "ZMQ_ROUTER_RAW",
IPV6              = "ZMQ_IPV6",
MECHANISM         = "ZMQ_MECHANISM",
PLAIN_SERVER      = "ZMQ_PLAIN_SERVER",
PLAIN_USERNAME    = "ZMQ_PLAIN_USERNAME",
PLAIN_PASSWORD    = "ZMQ_PLAIN_PASSWORD",
CURVE_SERVER      = "ZMQ_CURVE_SERVER",
CURVE_PUBLICKEY   = "ZMQ_CURVE_PUBLICKEY",
CURVE_SECRETKEY   = "ZMQ_CURVE_SECRETKEY",
CURVE_SERVERKEY   = "ZMQ_CURVE_SERVERKEY",
PROBE_ROUTER      = "ZMQ_PROBE_ROUTER",
REQ_CORRELATE     = "ZMQ_REQ_CORRELATE",
REQ_RELAXED       = "ZMQ_REQ_RELAXED",
CONFLATE          = "ZMQ_CONFLATE",
ZAP_DOMAIN        = "ZMQ_ZAP_DOMAIN",

-- send/recv flags
NOBLOCK           = "ZMQ_NOBLOCK",
DONTWAIT          = "ZMQ_DONTWAIT",
SNDMORE           = "ZMQ_SNDMORE",
SNDLABEL          = "ZMQ_SNDLABEL",

-- Security mechanisms
NULL              = "ZMQ_NULL",
PLAIN             = "ZMQ_PLAIN",
CURVE             = "ZMQ_CURVE",

-- poll events
POLLIN            = "ZMQ_POLLIN",
POLLOUT           = "ZMQ_POLLOUT",
POLLERR           = "ZMQ_POLLERR",

-- poll milliseconds.
POLL_MSEC         = "ZMQ_POLL_MSEC",

-- Socket Monitor events.
EVENT_CONNECTED   = "ZMQ_EVENT_CONNECTED",
EVENT_CONNECT_DELAYED = "ZMQ_EVENT_CONNECT_DELAYED",
EVENT_CONNECT_RETRIED = "ZMQ_EVENT_CONNECT_RETRIED",

EVENT_LISTENING   = "ZMQ_EVENT_LISTENING",
EVENT_BIND_FAILED = "ZMQ_EVENT_BIND_FAILED",

EVENT_ACCEPTED    = "ZMQ_EVENT_ACCEPTED",
EVENT_ACCEPT_FAILED= "ZMQ_EVENT_ACCEPT_FAILED",

EVENT_CLOSED      = "ZMQ_EVENT_CLOSED",
EVENT_CLOSE_FAILED= "ZMQ_EVENT_CLOSE_FAILED",
EVENT_DISCONNECTED= "ZMQ_EVENT_DISCONNECTED",
EVENT_MONITOR_STOPPED = "ZMQ_EVENT_MONITOR_STOPPED",

EVENT_ALL         = "ZMQ_EVENT_ALL",

-- devices
STREAMER          = "ZMQ_STREAMER",
FORWARDER         = "ZMQ_FORWARDER",
QUEUE             = "ZMQ_QUEUE",
},

subfiles {
"src/error.nobj.lua",
"src/msg.nobj.lua",
"src/socket.nobj.lua",
"src/poller.nobj.lua",
"src/ctx.nobj.lua",
"src/stopwatch.nobj.lua",
},

--
-- Module static functions
--
c_function "version" {
	var_out{ "<any>", "ver" },
	c_source[[
	int major, minor, patch;
	zmq_version(&(major), &(minor), &(patch));

	/* return version as a table: { major, minor, patch } */
	lua_createtable(L, 3, 0);
	lua_pushinteger(L, major);
	lua_rawseti(L, -2, 1);
	lua_pushinteger(L, minor);
	lua_rawseti(L, -2, 2);
	lua_pushinteger(L, patch);
	lua_rawseti(L, -2, 3);
]],
},
c_function "init" {
	var_in{ "int", "io_threads?", default = "1" },
	c_call "!ZMQ_Ctx *" "zmq_init" { "int", "io_threads" },
},
c_function "init_ctx" {
	var_in{ "<any>", "ptr" },
	var_out{ "ZMQ_Ctx *", "ctx" },
	c_source[[
	if(lua_isuserdata(L, ${ptr::idx})) {
		${ctx} = lua_touserdata(L, ${ptr::idx});
	} else {
		return luaL_argerror(L, ${ptr::idx}, "expected lightuserdata");
	}
]],
	ffi_source[[
	local p_type = type(${ptr})
	if p_type == 'userdata' then
		${ctx} = ffi.cast('ZMQ_Ctx *', ${ptr});
	elseif p_type == 'cdata' and ffi.istype('void *', ${ptr}) then
		${ctx} = ffi.cast('ZMQ_Ctx *', ${ptr});
	else
		return error("expected lightuserdata/cdata<void *>");
	end
]],
},
c_function "device" { if_defs = { "VERSION_2_0", "VERSION_3_2" },
	c_call "ZMQ_Error" "zmq_device"
		{ "int", "device", "ZMQ_Socket *", "insock", "ZMQ_Socket *", "outsock" },
},
c_function "proxy" { if_defs = "VERSION_3_2",
	c_call "ZMQ_Error" "zmq_proxy"
		{ "ZMQ_Socket *", "frontend", "ZMQ_Socket *", "backend", "ZMQ_Socket *", "capture?" },
},

--
-- utils
--
c_function "stopwatch_start" {
	c_call "!ZMQ_StopWatch *" "zmq_stopwatch_start" {},
},
c_function "sleep" {
	c_call "void" "zmq_sleep" { "int", "seconds_" },
},
}

