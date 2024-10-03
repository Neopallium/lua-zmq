package = "lua-zmq-threads"
version = "1.2-1"
source = {
	url = "git://github.com/Neopallium/lua-zmq.git",
	branch = "v1.2",
}
description = {
	summary = "Lua bindings to zeromq2, with LuaJIT2 FFI support.",
	homepage = "http://github.com/Neopallium/lua-zmq",
	license = "MIT/X11"
}
dependencies = {
	"lua-zmq >= 1.2-1",
	"lua-llthreads >= 1.3-1",
}
build = {
	type = "none",
	install = {
		lua = {
			['zmq.threads'] = "src/threads.lua",
		}
	}
}
