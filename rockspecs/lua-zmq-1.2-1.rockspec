package = "lua-zmq"
version = "1.2-1"
source = {
	url = "git://github.com/Neopallium/lua-zmq.git",
	branch = "v1.2",
}
description = {
	summary = "Lua bindings to zeromq2, with LuaJIT2 FFI support.",
	homepage = "http://github.com/Neopallium/lua-zmq",
	license = "MIT/X11",
}
dependencies = {
	"lua >= 5.1, < 5.5",
}
external_dependencies = {
	ZEROMQ = {
		header = "zmq.h",
		library = "zmq",
	}
}
build = {
	type = "builtin",
	modules = {
		zmq = {
			sources = {"src/pre_generated-zmq.nobj.c"},
			incdirs = "$(ZEROMQ_INCDIR)",
			libdirs = "$(ZEROMQ_LIBDIR)",
			libraries = {"zmq"},
		},
	},
	install = {
		lua = {
			['zmq.poller'] = "src/poller.lua",
		}
	}
}
