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

object "ZMQ_Ctx" {
	sys_include"string.h",
	error_on_null = "get_zmq_strerror()",
	c_source [[
typedef struct ZMQ_Ctx ZMQ_Ctx;
]],
	destructor "term" {
		c_method_call "ZMQ_Error"  "zmq_term" {}
	},
	method "lightuserdata" {
		var_out{ "void *", "ptr" },
		c_source[[
	${ptr} = ${this};
]]
	},
	method "socket" {
		c_method_call "!ZMQ_Socket *"  "zmq_socket" { "int", "type"}
	},
	method "set" {
		if_defs = { "VERSION_3_2" },
		c_method_call "int" "zmq_ctx_set" { "int", "flag", "int", "value" } 
	},
	method "get" {
		if_defs = { "VERSION_3_2" },
		c_method_call "int" "zmq_ctx_get" { "int", "flag" }
	},
}

