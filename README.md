About
=====

Lua bindings to zeromq2.  Check out the [ZeroMQ Guide with Lua examples](http://zguide.zeromq.org/lua:all).

API
===

See [API.md](https://github.com/Neopallium/lua-zmq/blob/master/API.md) and
[ØMQ docs](http://api.zero.mq/2-1-3:_start).

Requirements
============

* ZeroMQ version 2.1.x.
* Might work with some 2.0.x versions (2.0.6 and lower are not supported).

For Ubuntu 10.10 users:

* The version of ZeroMQ (2.0.6beta) that comes with Ubuntu 10.10 will not work with these bindings.  Please upgrade to [version 2.1](http://fanf42.blogspot.com/2011/02/installing-zeromq-and-java-bindings-on.html).

Installation
============

It is recommended to either compile Lua with the "-pthread" flag or preload libpthread.so on Linux when using this module ([see this glibc bug report](http://sourceware.org/bugzilla/show_bug.cgi?id=10652)):

	$ LD_PRELOAD=/lib/libpthread.so lua


Release 1.0
-----------

lua-zmq:

	$ sudo luarocks install lua-zmq

lua-zmq-threads:

	$ sudo luarocks install lua-llthreads
	$ sudo luarocks install lua-zmq-threads


Latest Git revision
-------------------

With LuaRocks 2.0.4.1:

	$ sudo luarocks install https://github.com/Neopallium/lua-zmq/raw/master/rockspecs/lua-zmq-scm-1.rockspec

For threads support:

	$ sudo luarocks install https://github.com/Neopallium/lua-llthreads/raw/master/rockspecs/lua-llthreads-scm-0.rockspec
	$ sudo luarocks install https://github.com/Neopallium/lua-zmq/raw/master/rockspecs/lua-zmq-threads-scm-0.rockspec

With CMake:

	$ git clone git://github.com/Neopallium/lua-zmq.git
	$ cd lua-zmq ; mkdir build ; cd build
	$ cmake ..
	$ make
	$ sudo make install

Throughput benchmark
====================

	Throughput benchmark using the tcp transport over localhost:
	message size: 30 [B]
	message count: 100000000
	
	Using send/recv functions running under Lua 5.1.4:
	mean throughput: 1577407 [msg/s]
	mean throughput: 378.578 [Mb/s]
	
	Using send/recv functions running under LuaJIT2 (git HEAD):
	mean throughput: 5112158 [msg/s]
	mean throughput: 1226.918 [Mb/s]
	
	Using send_msg/recv_msg functions running under LuaJIT2 (git HEAD):
	mean throughput: 6160911 [msg/s]
	mean throughput: 1478.619 [Mb/s]
	
	C++ code:
	mean throughput: 6241452 [msg/s]
	mean throughput: 1497.948 [Mb/s]


Running benchmarks
==================

When running the benchmarks you will need run two different scripts (one 'local' and one 'remote').  Both scripts can be run on the same computer or on different computers.  Make sure to start the 'local' script first.

Throughput benchmark:

	# first start local script
	$ luajit-2 perf/local_thr.lua "tcp://lo:5555" 30 1000000
	
	# then in another window start remote script
	$ luajit-2 perf/remote_thr.lua "tcp://localhost:5555" 30 1000000

Latency benchmark:

	# first start local script
	$ luajit-2 perf/local_lat.lua "tcp://lo:5555" 1 100000
	
	# then in another window start remote script
	$ luajit-2 perf/remote_lat.lua "tcp://localhost:5555" 1 100000


