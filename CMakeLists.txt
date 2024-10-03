#
# Lua bindings for 0MQ
#
cmake_minimum_required(VERSION 3.18)

project(lua-zmq C)

set(BUILD_SHARED_LIBS TRUE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(INSTALL_LMOD ${CMAKE_INSTALL_PREFIX}/share/lua/ CACHE PATH
				"Directory to install Lua source modules (configure lua via LUA_PATH)")
set(INSTALL_CMOD ${CMAKE_INSTALL_PREFIX}/lib/lua/ CACHE PATH
				"Directory to install Lua binary modules (configure lua via LUA_CPATH)")
set(ZMQ_PATH "" CACHE PATH
				"Directory to libzmq. (by default use pkg-config to detect path)")

set(COMMON_CFLAGS "${CFLAGS}")
set(COMMON_LDFLAGS)
set(COMMON_LIBS)

## Lua 5.x
include(FindLua)
if(NOT ${LUA_FOUND})
    message(FATAL_ERROR "The FindLua module could not find lua :-(")
endif()
set(COMMON_LIBS "${COMMON_LIBS};${LUA_LIBRARIES}")

if(WIN32)
	set(COMMON_CFLAGS "${COMMON_CFLAGS} -I${LUA_INCLUDE_DIR}")
	set(COMMON_LDFLAGS "${COMMON_LDFLAGS} ${LUA_LIBRARY}")
	if(NOT MSVC)
		set(COMMON_LDFLAGS "${COMMON_LDFLAGS} -Wl,--export-all-symbols")
	endif()
endif()
## MAC OSX needs extra linker flags
if(APPLE)
	set(COMMON_LDFLAGS "${COMMON_LDFLAGS} -undefined dynamic_lookup")
endif()

## LibZMQ
if(WIN32)
	## need ZMQ_PATH
	if(IS_DIRECTORY "${ZMQ_PATH}")
	else()
		message(FATAL_ERROR "Please set the ZMQ_PATH CMake variable.")
	endif()
endif()
if(IS_DIRECTORY ${ZMQ_PATH})
	set(COMMON_CFLAGS "${COMMON_CFLAGS} -I${ZMQ_PATH}/include")
	if(MSVC)
		set(COMMON_LIBS "${COMMON_LIBS};libzmq")
	else()
		set(COMMON_LDFLAGS "${COMMON_LDFLAGS} -L${ZMQ_PATH}/lib")
		set(COMMON_LIBS "${COMMON_LIBS};zmq")
	endif()
	link_directories(${ZMQ_PATH}/lib)
else()
	## fallback to using pkg-config
	include(FindPkgConfig)
	pkg_search_module(ZMQ REQUIRED libzmq)
	set(COMMON_CFLAGS "${COMMON_CFLAGS} ${ZMQ_CFLAGS}")
	set(COMMON_LDFLAGS "${COMMON_LDFLAGS} ${ZMQ_LDFLAGS}")
	set(COMMON_LIBS "${COMMON_LIBS};${ZMQ_LIBRARIES}")
endif()

## LuaNativeObjects
include(LuaNativeObjects)
include(CustomMacros)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${LUA_INCLUDE_DIR})

## LuaZMQ
set(LUA_ZMQ_SRC
	zmq.nobj.lua
)

## LuaZMQ Lua source modules
set(LUA_ZMQ_SRC_LUA_MODS
	src/poller.lua
	src/threads.lua
)

if(${USE_PRE_GENERATED_BINDINGS})
	set(LUA_ZMQ_SRC src/pre_generated-zmq.nobj.c)
else()
	# Generate Lua bindings.
	GenLuaNativeObjects(LUA_ZMQ_SRC)
endif()

add_library(lua-zmq MODULE ${LUA_ZMQ_SRC})
target_link_libraries(lua-zmq ${COMMON_LIBS})
set_target_properties(lua-zmq PROPERTIES PREFIX "")
add_target_properties(lua-zmq COMPILE_FLAGS "${COMMON_CFLAGS}")
add_target_properties(lua-zmq LINK_FLAGS "${LD_FLAGS} ${COMMON_LDFLAGS}")
set_target_properties(lua-zmq PROPERTIES OUTPUT_NAME zmq)

install(TARGETS lua-zmq
        DESTINATION "${INSTALL_CMOD}")

install(FILES ${LUA_ZMQ_SRC_LUA_MODS}
        DESTINATION "${INSTALL_LMOD}/zmq")

