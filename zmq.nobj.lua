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
use_globals = false,
hide_meta_info = true,
luajit_ffi = true,

sys_include "string.h",
include "zmq.h",

ffi_load {
"zmq", -- default lib name.
Windows = "libzmq", -- lib name for on windows.
},

c_source[[
/*
 * This wrapper function is to make the EAGAIN/ETERM error messages more like
 * what is returned by LuaSocket.
 */
static const char *get_zmq_strerror() {
	int err = zmq_errno();
	switch(err) {
	case EAGAIN:
		return "timeout";
		break;
	case EINTR:
		return "interrupted";
		break;
#if defined(ETERM)
	case ETERM:
		return "closed";
		break;
#endif
	default:
		break;
	}
	return zmq_strerror(err);
}

]],

-- export helper function 'get_zmq_strerror' to FFI code.
ffi_export_function "const char *" "get_zmq_strerror" "()",
ffi_source[[
local C_get_zmq_strerror = get_zmq_strerror
-- make nicer wrapper for exported error function.
local function get_zmq_strerror()
	return ffi.string(C_get_zmq_strerror())
end
]],

--
-- Module constants
--
constants {
MAX_VSM_SIZE   = 30,

-- message types
DELIMITER      = 31,
VSM            = 32,

-- message flags
MSG_MORE       = 1,
MSG_SHARED     = 128,

-- socket types
PAIR           = 0,
PUB            = 1,
SUB            = 2,
REQ            = 3,
REP            = 4,
XREQ           = 5,
XREP           = 6,
PULL           = 7,
PUSH           = 8,

-- socket options
HWM            = 1,
SWAP           = 3,
AFFINITY       = 4,
IDENTITY       = 5,
SUBSCRIBE      = 6,
UNSUBSCRIBE    = 7,
RATE           = 8,
RECOVERY_IVL   = 9,
MCAST_LOOP     = 10,
SNDBUF         = 11,
RCVBUF         = 12,
RCVMORE        = 13,
FD             = 14,
EVENTS         = 15,
TYPE           = 16,
LINGER         = 17,
RECONNECT_IVL  = 18,
BACKLOG        = 19,

-- send/recv flags
NOBLOCK        = 1,
SNDMORE        = 2,

-- poll events
POLLIN         = 1,
POLLOUT        = 2,
POLLERR        = 4,

-- devices
STREAMER       = 1,
FORWARDER      = 2,
QUEUE          = 3,
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
		${ctx} = ffi.cast('void *', ${ptr});
	elseif p_type == 'cdata' then
		${ctx} = ${ptr};
	else
		return error("expected lightuserdata/cdata<void *>");
	end
]],
},
c_function "device" {
	c_call "ZMQ_Error" "zmq_device"
		{ "int", "device", "ZMQ_Socket *", "insock", "ZMQ_Socket *", "outsock" },
},

--
-- zmq_utils.h
--
include "zmq_utils.h",
c_function "stopwatch_start" {
	c_call "!ZMQ_StopWatch *" "zmq_stopwatch_start" {},
},
c_function "sleep" {
	c_call "void" "zmq_sleep" { "int", "seconds_" },
},
--
-- This dump function is for getting a copy of the FFI-based bindings code and is
-- only for debugging.
--
c_function "dump_ffi" {
	var_out{ "const char *", "ffi_code", has_length = true, },
	c_source[[
	${ffi_code} = ${module_c_name}_ffi_lua_code;
	${ffi_code_len} = sizeof(${module_c_name}_ffi_lua_code) - 1;
]],
},
}

