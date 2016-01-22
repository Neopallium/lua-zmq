-- Copyright (c) 2010 Aleksey Yeschenko <aleksey@yeschenko.com>
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

if not arg[3] then
    print("usage: lua local_thr.lua <bind-to> <message-size> <message-count>")
    os.exit()
end

local bind_to = arg[1]
local message_size = tonumber(arg[2])
local message_count = tonumber(arg[3])

local zmq = require"zmq"

local ctx = zmq.init(1)
local s = assert(ctx:socket(zmq.PULL))
assert(s:bind(bind_to))

print(string.format("message size: %i [B]", message_size))
print(string.format("message count: %i", message_count))

local msg
msg = zmq.zmq_msg_t()
assert(s:recv_msg(msg))

local timer = zmq.stopwatch_start()

for i = 1, message_count - 1 do
	assert(s:recv_msg(msg))
	assert(msg:size() == message_size, "Invalid message size")
end

local elapsed = timer:stop()

s:close()
ctx:term()

if elapsed == 0 then elapsed = 1 end

local throughput = message_count / (elapsed / 1000000)
local megabits = throughput * message_size * 8 / 1000000

print(string.format("mean throughput: %i [msg/s]", throughput))
print(string.format("mean throughput: %.3f [Mb/s]", megabits))

