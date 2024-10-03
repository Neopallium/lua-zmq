/***********************************************************************************************
************************************************************************************************
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!! Warning this file was generated from a set of *.nobj.lua definition files !!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
************************************************************************************************
***********************************************************************************************/

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/* some Lua 5.0 compatibility support. */
#if !defined(lua_pushliteral)
#define lua_pushliteral(L, s) lua_pushstring(L, "" s, (sizeof(s)/sizeof(char))-1)
#endif

#if !defined(LUA_VERSION_NUM)
#define lua_pushinteger(L, n) lua_pushnumber(L, (lua_Number)n)
#define luaL_Reg luaL_reg
#endif

/*
** Adapted from Lua 5.2.0 luaL_setfuncs.
*/
static void nobj_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -nup);
    lua_pushstring(L, l->name);
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3));
  }
  lua_pop(L, nup);  /* remove upvalues */
}

/* some Lua 5.1 compatibility support. */
#if !defined(LUA_VERSION_NUM) || (LUA_VERSION_NUM == 501)

#define lua_load_no_mode(L, reader, data, source) \
	lua_load(L, reader, data, source)

#define lua_rawlen(L, idx) lua_objlen(L, idx)

#endif

#if LUA_VERSION_NUM >= 502

#define lua_load_no_mode(L, reader, data, source) \
	lua_load(L, reader, data, source, NULL)

static int luaL_typerror (lua_State *L, int narg, const char *tname) {
  const char *msg = lua_pushfstring(L, "%s expected, got %s",
                                    tname, luaL_typename(L, narg));
  return luaL_argerror(L, narg, msg);
}

#endif

#define REG_PACKAGE_IS_CONSTRUCTOR 0
#define REG_MODULES_AS_GLOBALS 1
#define REG_OBJECTS_AS_GLOBALS 0
#define OBJ_DATA_HIDDEN_METATABLE 1
#define USE_FIELD_GET_SET_METHODS 0
#define LUAJIT_FFI 1


#include <string.h>
#include "zmq.h"



#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#ifdef _MSC_VER
#define __WINDOWS__
#else
#if defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#ifdef __WINDOWS__

/* for MinGW32 compiler need to include <stdint.h> */
#ifdef __GNUC__
#include <stdint.h>
#include <stdbool.h>
#else

/* define some standard types missing on Windows. */
#ifndef __INT32_MAX__
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
#endif
#ifndef __INT64_MAX__
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif
typedef int bool;
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#endif

/* wrap strerror_s(). */
#ifdef __GNUC__
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) do { \
	strncpy((buf), strerror(errno), (buflen)-1); \
	(buf)[(buflen)-1] = '\0'; \
} while(0)
#endif
#else
#ifndef strerror_r
#define strerror_r(errno, buf, buflen) strerror_s((buf), (buflen), (errno))
#endif
#endif

#define FUNC_UNUSED

#define LUA_NOBJ_API __declspec(dllexport)

#else

#define LUA_NOBJ_API LUALIB_API

#include <stdint.h>
#include <stdbool.h>

#define FUNC_UNUSED __attribute__((unused))

#endif

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define assert_obj_type(type, obj) \
	assert(__builtin_types_compatible_p(typeof(obj), type *))
#else
#define assert_obj_type(type, obj)
#endif

void *nobj_realloc(void *ptr, size_t osize, size_t nsize);

void *nobj_realloc(void *ptr, size_t osize, size_t nsize) {
	(void)osize;
	if(0 == nsize) {
		free(ptr);
		return NULL;
	}
	return realloc(ptr, nsize);
}

#define obj_type_free(type, obj) do { \
	assert_obj_type(type, obj); \
	nobj_realloc((obj), sizeof(type), 0); \
} while(0)

#define obj_type_new(type, obj) do { \
	assert_obj_type(type, obj); \
	(obj) = nobj_realloc(NULL, 0, sizeof(type)); \
} while(0)

typedef struct obj_type obj_type;

typedef void (*base_caster_t)(void **obj);

typedef void (*dyn_caster_t)(void **obj, obj_type **type);

#define OBJ_TYPE_FLAG_WEAK_REF (1<<0)
#define OBJ_TYPE_SIMPLE (1<<1)
#define OBJ_TYPE_IMPORT (1<<2)
struct obj_type {
	dyn_caster_t    dcaster;  /**< caster to support casting to sub-objects. */
	int32_t         id;       /**< type's id. */
	uint32_t        flags;    /**< type's flags (weak refs) */
	const char      *name;    /**< type's object name. */
};

typedef struct obj_base {
	int32_t        id;
	base_caster_t  bcaster;
} obj_base;

typedef enum obj_const_type {
	CONST_UNKOWN    = 0,
	CONST_BOOLEAN   = 1,
	CONST_NUMBER    = 2,
	CONST_STRING    = 3
} obj_const_type;

typedef struct obj_const {
	const char      *name;    /**< constant's name. */
	const char      *str;
	double          num;
	obj_const_type  type;
} obj_const;

typedef enum obj_field_type {
	TYPE_UNKOWN    = 0,
	TYPE_UINT8     = 1,
	TYPE_UINT16    = 2,
	TYPE_UINT32    = 3,
	TYPE_UINT64    = 4,
	TYPE_INT8      = 5,
	TYPE_INT16     = 6,
	TYPE_INT32     = 7,
	TYPE_INT64     = 8,
	TYPE_DOUBLE    = 9,
	TYPE_FLOAT     = 10,
	TYPE_STRING    = 11
} obj_field_type;

typedef struct obj_field {
	const char      *name;  /**< field's name. */
	uint32_t        offset; /**< offset to field's data. */
	obj_field_type  type;   /**< field's data type. */
	uint32_t        flags;  /**< is_writable:1bit */
} obj_field;

typedef enum {
	REG_OBJECT,
	REG_PACKAGE,
	REG_META,
} module_reg_type;

typedef struct reg_impl {
	const char *if_name;
	const void *impl;
} reg_impl;

typedef struct reg_sub_module {
	obj_type        *type;
	module_reg_type req_type;
	const luaL_Reg  *pub_funcs;
	const luaL_Reg  *methods;
	const luaL_Reg  *metas;
	const obj_base  *bases;
	const obj_field *fields;
	const obj_const *constants;
	const reg_impl  *implements;
	int             bidirectional_consts;
} reg_sub_module;

#define OBJ_UDATA_FLAG_OWN (1<<0)
#define OBJ_UDATA_FLAG_LOOKUP (1<<1)
#define OBJ_UDATA_LAST_FLAG (OBJ_UDATA_FLAG_LOOKUP)
typedef struct obj_udata {
	void     *obj;
	uint32_t flags;  /**< lua_own:1bit */
} obj_udata;

/* use static pointer as key to weak userdata table. */
static char obj_udata_weak_ref_key[] = "obj_udata_weak_ref_key";

/* use static pointer as key to module's private table. */
static char obj_udata_private_key[] = "obj_udata_private_key";

#if LUAJIT_FFI
typedef int (*ffi_export_func_t)(void);
typedef struct ffi_export_symbol {
	const char *name;
	union {
	void               *data;
	ffi_export_func_t  func;
	} sym;
} ffi_export_symbol;
#endif

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



typedef int ZMQ_Error;

static void error_code__ZMQ_Error__push(lua_State *L, ZMQ_Error err);


static obj_type obj_types[] = {
#define obj_type_id_ZErrors 0
#define obj_type_ZErrors (obj_types[obj_type_id_ZErrors])
  { NULL, 0, OBJ_TYPE_FLAG_WEAK_REF, "ZErrors" },
#define obj_type_id_zmq_msg_t 1
#define obj_type_zmq_msg_t (obj_types[obj_type_id_zmq_msg_t])
  { NULL, 1, OBJ_TYPE_SIMPLE, "zmq_msg_t" },
#define obj_type_id_ZMQ_Socket 2
#define obj_type_ZMQ_Socket (obj_types[obj_type_id_ZMQ_Socket])
  { NULL, 2, OBJ_TYPE_FLAG_WEAK_REF, "ZMQ_Socket" },
#define obj_type_id_ZMQ_Poller 3
#define obj_type_ZMQ_Poller (obj_types[obj_type_id_ZMQ_Poller])
  { NULL, 3, OBJ_TYPE_SIMPLE, "ZMQ_Poller" },
#define obj_type_id_ZMQ_Ctx 4
#define obj_type_ZMQ_Ctx (obj_types[obj_type_id_ZMQ_Ctx])
  { NULL, 4, OBJ_TYPE_FLAG_WEAK_REF, "ZMQ_Ctx" },
#define obj_type_id_ZMQ_StopWatch 5
#define obj_type_ZMQ_StopWatch (obj_types[obj_type_id_ZMQ_StopWatch])
  { NULL, 5, OBJ_TYPE_FLAG_WEAK_REF, "ZMQ_StopWatch" },
  {NULL, -1, 0, NULL},
};


#if LUAJIT_FFI

/* nobj_ffi_support_enabled_hint should be set to 1 when FFI support is enabled in at-least one
 * instance of a LuaJIT state.  It should never be set back to 0. */
static int nobj_ffi_support_enabled_hint = 0;
static const char nobj_ffi_support_key[] = "LuaNativeObject_FFI_SUPPORT";
static const char nobj_check_ffi_support_code[] =
"local stat, ffi=pcall(require,\"ffi\")\n" /* try loading LuaJIT`s FFI module. */
"if not stat then return false end\n"
"return true\n";

static int nobj_check_ffi_support(lua_State *L) {
	int rc;
	int err;

	/* check if ffi test has already been done. */
	lua_pushstring(L, nobj_ffi_support_key);
	lua_rawget(L, LUA_REGISTRYINDEX);
	if(!lua_isnil(L, -1)) {
		rc = lua_toboolean(L, -1);
		lua_pop(L, 1);
		/* use results of previous check. */
		goto finished;
	}
	lua_pop(L, 1); /* pop nil. */

	err = luaL_loadbuffer(L, nobj_check_ffi_support_code,
		sizeof(nobj_check_ffi_support_code) - 1, nobj_ffi_support_key);
	if(0 == err) {
		err = lua_pcall(L, 0, 1, 0);
	}
	if(err) {
		const char *msg = "<err not a string>";
		if(lua_isstring(L, -1)) {
			msg = lua_tostring(L, -1);
		}
		printf("Error when checking for FFI-support: %s\n", msg);
		lua_pop(L, 1); /* pop error message. */
		return 0;
	}
	/* check results of test. */
	rc = lua_toboolean(L, -1);
	lua_pop(L, 1); /* pop results. */
		/* cache results. */
	lua_pushstring(L, nobj_ffi_support_key);
	lua_pushboolean(L, rc);
	lua_rawset(L, LUA_REGISTRYINDEX);

finished:
	/* turn-on hint that there is FFI code enabled. */
	if(rc) {
		nobj_ffi_support_enabled_hint = 1;
	}

	return rc;
}

typedef struct {
	const char **ffi_init_code;
	int offset;
} nobj_reader_state;

static const char *nobj_lua_Reader(lua_State *L, void *data, size_t *size) {
	nobj_reader_state *state = (nobj_reader_state *)data;
	const char *ptr;

	(void)L;
	ptr = state->ffi_init_code[state->offset];
	if(ptr != NULL) {
		*size = strlen(ptr);
		state->offset++;
	} else {
		*size = 0;
	}
	return ptr;
}

static int nobj_try_loading_ffi(lua_State *L, const char *ffi_mod_name,
		const char *ffi_init_code[], const ffi_export_symbol *ffi_exports, int priv_table)
{
	nobj_reader_state state = { ffi_init_code, 0 };
	int err;

	/* export symbols to priv_table. */
	while(ffi_exports->name != NULL) {
		lua_pushstring(L, ffi_exports->name);
		lua_pushlightuserdata(L, ffi_exports->sym.data);
		lua_settable(L, priv_table);
		ffi_exports++;
	}
	err = lua_load_no_mode(L, nobj_lua_Reader, &state, ffi_mod_name);
	if(0 == err) {
		lua_pushvalue(L, -2); /* dup C module's table. */
		lua_pushvalue(L, priv_table); /* move priv_table to top of stack. */
		lua_remove(L, priv_table);
		lua_pushvalue(L, LUA_REGISTRYINDEX);
		err = lua_pcall(L, 3, 0, 0);
	}
	if(err) {
		const char *msg = "<err not a string>";
		if(lua_isstring(L, -1)) {
			msg = lua_tostring(L, -1);
		}
		printf("Failed to install FFI-based bindings: %s\n", msg);
		lua_pop(L, 1); /* pop error message. */
	}
	return err;
}
#endif


typedef struct {
	void *impl;
	void *obj;
} obj_implement;

static FUNC_UNUSED void *obj_implement_luaoptional(lua_State *L, int _index, void **impl, char *if_name) {
	void *ud;
	if(lua_isnoneornil(L, _index)) {
		return NULL;
	}
	/* get the implements table for this interface. */
	lua_pushlightuserdata(L, if_name);
	lua_rawget(L, LUA_REGISTRYINDEX);

	/* get pointer to userdata value & check if it is a userdata value. */
	ud = (obj_implement *)lua_touserdata(L, _index);
	if(ud != NULL) {
		/* get the userdata's metatable */
		if(lua_getmetatable(L, _index)) {
			/* lookup metatable in interface table for this object's implementation of the interface. */
			lua_gettable(L, -2);
		} else {
			/* no metatable. */
			goto no_interface;
		}
#if LUAJIT_FFI
	} else if(nobj_ffi_support_enabled_hint) { /* handle cdata. */
		/* get cdata interface check function from interface table. */
		lua_getfield(L, -1, "cdata");
		if(lua_isfunction(L, -1)) {
			/* pass cdata to function, return value should be an implmentation. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			/* get pointer to cdata. */
			ud = (void *)lua_topointer(L, _index);
		} else {
			lua_pop(L, 1); /* pop non-function. */
			goto no_interface;
		}
#endif
	} else {
		goto no_interface;
	}

	if(!lua_isnil(L, -1)) {
		*impl = lua_touserdata(L, -1);
		lua_pop(L, 2); /* pop interface table & implementation. */
		/* object implements interface. */
		return ud;
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
no_interface:
	lua_pop(L, 1); /* pop interface table. */
	return NULL;
}

static FUNC_UNUSED void *obj_implement_luacheck(lua_State *L, int _index, void **impl, char *type) {
	void *ud = obj_implement_luaoptional(L, _index, impl, type);
	if(ud == NULL) {
#define ERROR_BUFFER_SIZE 256
		char buf[ERROR_BUFFER_SIZE];
		snprintf(buf, ERROR_BUFFER_SIZE-1,"Expected object with %s interface", type);
		/* value doesn't implement this interface. */
		luaL_argerror(L, _index, buf);
	}
	return ud;
}

/* use static pointer as key to interfaces table. (version 1.0) */
static char obj_interfaces_table_key[] = "obj_interfaces<1.0>_table_key";

static void obj_get_global_interfaces_table(lua_State *L) {
	/* get global interfaces table. */
	lua_getfield(L, LUA_REGISTRYINDEX, obj_interfaces_table_key);
	if(lua_isnil(L, -1)) {
		/* Need to create global interfaces table. */
		lua_pop(L, 1); /* pop nil */
		lua_createtable(L, 0, 4); /* 0 size array part, small hash part. */
		lua_pushvalue(L, -1); /* dup table. */
		/* store interfaces table in Lua registery. */
		lua_setfield(L, LUA_REGISTRYINDEX, obj_interfaces_table_key);
	}
}

static void obj_get_interface(lua_State *L, const char *name, int global_if_tab) {
	/* get a interface's implementation table */
	lua_getfield(L, global_if_tab, name);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil */
		/* new interface. (i.e. no object implement it yet.)
		 *
		 * create an empty table for this interface that will be used when an
		 * implementation is registered for this interface.
		 */
		lua_createtable(L, 0, 2); /* 0 size array part, small hash part. */
		lua_pushvalue(L, -1); /* dup table. */
		lua_setfield(L, global_if_tab, name); /* store interface in global interfaces table. */
	}
}

static int obj_get_userdata_interface(lua_State *L) {
	/* get the userdata's metatable */
	if(lua_getmetatable(L, 2)) {
		/* lookup metatable in interface table for the userdata's implementation of the interface. */
		lua_gettable(L, 1);
		if(!lua_isnil(L, -1)) {
			/* return the implementation. */
			return 1;
		}
	}
	/* no metatable or no implementation. */
	return 0;
}

static void obj_interface_register(lua_State *L, char *name, int global_if_tab) {
	/* get the table of implementations for this interface. */
	obj_get_interface(L, name, global_if_tab);

	/* check for 'userdata' function. */
	lua_getfield(L, -1, "userdata");
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		/* add C function for getting a userdata's implementation. */
		lua_pushcfunction(L, obj_get_userdata_interface);
		lua_setfield(L, -2, "userdata");
	} else {
		/* already have function. */
		lua_pop(L, 1); /* pop C function. */
	}
	/* we are going to use a lightuserdata pointer for fast lookup of the interface's impl. table. */
	lua_pushlightuserdata(L, name);
	lua_insert(L, -2);
	lua_settable(L, LUA_REGISTRYINDEX);
}

static void obj_register_interfaces(lua_State *L, char *interfaces[]) {
	int i;
	int if_tab;
	/* get global interfaces table. */
	obj_get_global_interfaces_table(L);
	if_tab = lua_gettop(L);

	for(i = 0; interfaces[i] != NULL ; i++) {
		obj_interface_register(L, interfaces[i], if_tab);
	}
	lua_pop(L, 1); /* pop global interfaces table. */
}

static void obj_type_register_implement(lua_State *L, const reg_impl *impl, int global_if_tab, int mt_tab) {
	/* get the table of implementations for this interface. */
	obj_get_interface(L, impl->if_name, global_if_tab);

	/* register object's implement in the interface table. */
	lua_pushvalue(L, mt_tab);
	lua_pushlightuserdata(L, (void *)impl->impl);
	lua_settable(L, -3);

	lua_pop(L, 1); /* pop inteface table. */
}

static void obj_type_register_implements(lua_State *L, const reg_impl *impls) {
	int if_tab;
	int mt_tab;
	/* get absolute position of object's metatable. */
	mt_tab = lua_gettop(L);
	/* get global interfaces table. */
	obj_get_global_interfaces_table(L);
	if_tab = lua_gettop(L);

	for(; impls->if_name != NULL ; impls++) {
		obj_type_register_implement(L, impls, if_tab, mt_tab);
	}
	lua_pop(L, 1); /* pop global interfaces table. */
}

#ifndef REG_PACKAGE_IS_CONSTRUCTOR
#define REG_PACKAGE_IS_CONSTRUCTOR 1
#endif

#ifndef REG_MODULES_AS_GLOBALS
#define REG_MODULES_AS_GLOBALS 0
#endif

/* For Lua >=5.2 don't register modules as globals. */
#if LUA_VERSION_NUM >= 502
#undef REG_MODULES_AS_GLOBALS
#define REG_MODULES_AS_GLOBALS 0
#endif

#ifndef REG_OBJECTS_AS_GLOBALS
#define REG_OBJECTS_AS_GLOBALS 0
#endif

#ifndef OBJ_DATA_HIDDEN_METATABLE
#define OBJ_DATA_HIDDEN_METATABLE 1
#endif

static FUNC_UNUSED int obj_import_external_type(lua_State *L, obj_type *type) {
	/* find the external type's metatable using it's name. */
	lua_pushstring(L, type->name);
	lua_rawget(L, LUA_REGISTRYINDEX); /* external type's metatable. */
	if(!lua_isnil(L, -1)) {
		/* found it.  Now we will map our 'type' pointer to the metatable. */
		/* REGISTERY[lightuserdata<type>] = REGISTERY[type->name] */
		lua_pushlightuserdata(L, type); /* use our 'type' pointer as lookup key. */
		lua_pushvalue(L, -2); /* dup. type's metatable. */
		lua_rawset(L, LUA_REGISTRYINDEX); /* save external type's metatable. */
		/* NOTE: top of Lua stack still has the type's metatable. */
		return 1;
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
	return 0;
}

static FUNC_UNUSED int obj_import_external_ffi_type(lua_State *L, obj_type *type) {
	/* find the external type's metatable using it's name. */
	lua_pushstring(L, type->name);
	lua_rawget(L, LUA_REGISTRYINDEX); /* external type's metatable. */
	if(!lua_isnil(L, -1)) {
		/* found it.  Now we will map our 'type' pointer to the C check function. */
		/* _priv_table[lightuserdata<type>] = REGISTERY[type->name].c_check */
		lua_getfield(L, -1, "c_check");
		lua_remove(L, -2); /* remove metatable. */
		if(lua_isfunction(L, -1)) {
			lua_pushlightuserdata(L, type); /* use our 'type' pointer as lookup key. */
			lua_pushvalue(L, -2); /* dup. check function */
			lua_rawset(L, -4); /* save check function to module's private table. */
			/* NOTE: top of Lua stack still has the type's C check function. */
			return 1;
		} else {
			lua_pop(L, 1); /* pop non function value. */
		}
	} else {
		lua_pop(L, 1); /* pop nil. */
	}
	return 0;
}

static FUNC_UNUSED obj_udata *obj_udata_toobj(lua_State *L, int _index) {
	obj_udata *ud;
	size_t len;

	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	/* verify userdata size. */
	len = lua_rawlen(L, _index);
	if(len != sizeof(obj_udata)) {
		/* This shouldn't be possible */
		luaL_error(L, "invalid userdata size: size=%d, expected=%d", len, sizeof(obj_udata));
	}
	return ud;
}

static FUNC_UNUSED int obj_udata_is_compatible(lua_State *L, obj_udata *ud, void **obj, base_caster_t *caster, obj_type *type) {
	obj_base *base;
	obj_type *ud_type;
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
recheck_metatable:
	if(lua_rawequal(L, -1, -2)) {
		*obj = ud->obj;
		/* same type no casting needed. */
		return 1;
	} else if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		if((type->flags & OBJ_TYPE_IMPORT) == 0) {
			/* can't resolve internal type. */
			luaL_error(L, "Unknown object type(id=%d, name=%s)", type->id, type->name);
		}
		/* try to import external type. */
		if(obj_import_external_type(L, type)) {
			/* imported type, re-try metatable check. */
			goto recheck_metatable;
		}
		/* External type not yet available, so the object can't be compatible. */
	} else {
		/* Different types see if we can cast to the required type. */
		lua_rawgeti(L, -2, type->id);
		base = lua_touserdata(L, -1);
		lua_pop(L, 1); /* pop obj_base or nil */
		if(base != NULL) {
			*caster = base->bcaster;
			/* get the obj_type for this userdata. */
			lua_pushliteral(L, ".type");
			lua_rawget(L, -3); /* type's metatable. */
			ud_type = lua_touserdata(L, -1);
			lua_pop(L, 1); /* pop obj_type or nil */
			if(base == NULL) {
				luaL_error(L, "bad userdata, missing type info.");
				return 0;
			}
			/* check if userdata is a simple object. */
			if(ud_type->flags & OBJ_TYPE_SIMPLE) {
				*obj = ud;
			} else {
				*obj = ud->obj;
			}
			return 1;
		}
	}
	return 0;
}

static FUNC_UNUSED obj_udata *obj_udata_luacheck_internal(lua_State *L, int _index, void **obj, obj_type *type, int not_delete) {
	obj_udata *ud;
	base_caster_t caster = NULL;
	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			if(obj_udata_is_compatible(L, ud, obj, &(caster), type)) {
				lua_pop(L, 2); /* pop both metatables. */
				/* apply caster function if needed. */
				if(caster != NULL && *obj != NULL) {
					caster(obj);
				}
				/* check object pointer. */
				if(*obj == NULL) {
					if(not_delete) {
						luaL_error(L, "null %s", type->name); /* object was garbage collected? */
					}
					return NULL;
				}
				return ud;
			}
		}
	} else if(!lua_isnoneornil(L, _index)) {
		/* handle cdata. */
		/* get private table. */
		lua_pushlightuserdata(L, obj_udata_private_key);
		lua_rawget(L, LUA_REGISTRYINDEX); /* private table. */
		/* get cdata type check function from private table. */
		lua_pushlightuserdata(L, type);
		lua_rawget(L, -2);

		/* check for function. */
		if(!lua_isnil(L, -1)) {
got_check_func:
			/* pass cdata value to type checking function. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			if(!lua_isnil(L, -1)) {
				/* valid type get pointer from cdata. */
				lua_pop(L, 2);
				*obj = *(void **)lua_topointer(L, _index);
				return ud;
			}
			lua_pop(L, 2);
		} else {
			lua_pop(L, 1); /* pop nil. */
			if(type->flags & OBJ_TYPE_IMPORT) {
				/* try to import external ffi type. */
				if(obj_import_external_ffi_type(L, type)) {
					/* imported type. */
					goto got_check_func;
				}
				/* External type not yet available, so the object can't be compatible. */
			}
		}
	}
	if(not_delete) {
		luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	}
	return NULL;
}

static FUNC_UNUSED void *obj_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *obj = NULL;
	obj_udata_luacheck_internal(L, _index, &(obj), type, 1);
	return obj;
}

static FUNC_UNUSED void *obj_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	void *obj = NULL;
	if(lua_isnoneornil(L, _index)) {
		return obj;
	}
	obj_udata_luacheck_internal(L, _index, &(obj), type, 1);
	return obj;
}

static FUNC_UNUSED void *obj_udata_luadelete(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type, 0);
	if(ud == NULL) return NULL;
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable in invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;
	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_pushinteger(L, flags);
		lua_call(L, 2, 1);
		return;
	}
#endif
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);
}

static FUNC_UNUSED void *obj_udata_luadelete_weak(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type, 0);
	if(ud == NULL) return NULL;
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	/* clear the metatable in invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* remove object from weak table. */
	lua_pushlightuserdata(L, obj);
	lua_pushnil(L);
	lua_rawset(L, -3);
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush_weak(lua_State *L, void *obj, obj_type *type, int flags) {
	obj_udata *ud;

	/* convert NULL's into Lua nil's. */
	if(obj == NULL) {
		lua_pushnil(L);
		return;
	}
	/* check for type caster. */
	if(type->dcaster) {
		(type->dcaster)(&obj, &type);
	}
	/* get objects weak table. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key);
	lua_rawget(L, LUA_REGISTRYINDEX); /* weak ref table. */
	/* lookup userdata instance from pointer. */
	lua_pushlightuserdata(L, obj);
	lua_rawget(L, -2);
	if(!lua_isnil(L, -1)) {
		lua_remove(L, -2);     /* remove objects table. */
		return;
	}
	lua_pop(L, 1);  /* pop nil. */

#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		lua_remove(L, -2);
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_pushinteger(L, flags);
		lua_call(L, 2, 1);
		return;
	}
#endif
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));

	/* init. obj_udata. */
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);

	/* add weak reference to object. */
	lua_pushlightuserdata(L, obj); /* push object pointer as the 'key' */
	lua_pushvalue(L, -2);          /* push object's udata */
	lua_rawset(L, -4);             /* add weak reference to object. */
	lua_remove(L, -2);     /* remove objects table. */
}

/* default object equal method. */
static FUNC_UNUSED int obj_udata_default_equal(lua_State *L) {
	obj_udata *ud1 = obj_udata_toobj(L, 1);
	obj_udata *ud2 = obj_udata_toobj(L, 2);

	lua_pushboolean(L, (ud1->obj == ud2->obj));
	return 1;
}

/* default object tostring method. */
static FUNC_UNUSED int obj_udata_default_tostring(lua_State *L) {
	obj_udata *ud = obj_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p, flags=%d", ud->obj, ud->flags);
	lua_concat(L, 2);

	return 1;
}

/*
 * Simple userdata objects.
 */
static FUNC_UNUSED void *obj_simple_udata_toobj(lua_State *L, int _index) {
	void *ud;

	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	return ud;
}

static FUNC_UNUSED void * obj_simple_udata_luacheck(lua_State *L, int _index, obj_type *type) {
	void *ud;
	/* make sure it's a userdata value. */
	ud = lua_touserdata(L, _index);
	if(ud != NULL) {
		/* check object type by comparing metatables. */
		if(lua_getmetatable(L, _index)) {
			lua_pushlightuserdata(L, type);
			lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
recheck_metatable:
			if(lua_rawequal(L, -1, -2)) {
				lua_pop(L, 2); /* pop both metatables. */
				return ud;
			} else if(lua_isnil(L, -1)) {
				lua_pop(L, 1); /* pop nil. */
				if((type->flags & OBJ_TYPE_IMPORT) == 0) {
					/* can't resolve internal type. */
					luaL_error(L, "Unknown object type(id=%d, name=%s)", type->id, type->name);
				}
				/* try to import external type. */
				if(obj_import_external_type(L, type)) {
					/* imported type, re-try metatable check. */
					goto recheck_metatable;
				}
				/* External type not yet available, so the object can't be compatible. */
				return 0;
			}
		}
	} else if(!lua_isnoneornil(L, _index)) {
		/* handle cdata. */
		/* get private table. */
		lua_pushlightuserdata(L, obj_udata_private_key);
		lua_rawget(L, LUA_REGISTRYINDEX); /* private table. */
		/* get cdata type check function from private table. */
		lua_pushlightuserdata(L, type);
		lua_rawget(L, -2);

		/* check for function. */
		if(!lua_isnil(L, -1)) {
got_check_func:
			/* pass cdata value to type checking function. */
			lua_pushvalue(L, _index);
			lua_call(L, 1, 1);
			if(!lua_isnil(L, -1)) {
				/* valid type get pointer from cdata. */
				lua_pop(L, 2);
				return (void *)lua_topointer(L, _index);
			}
		} else {
			if(type->flags & OBJ_TYPE_IMPORT) {
				/* try to import external ffi type. */
				if(obj_import_external_ffi_type(L, type)) {
					/* imported type. */
					goto got_check_func;
				}
				/* External type not yet available, so the object can't be compatible. */
			}
		}
	}
	luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	return NULL;
}

static FUNC_UNUSED void * obj_simple_udata_luaoptional(lua_State *L, int _index, obj_type *type) {
	if(lua_isnoneornil(L, _index)) {
		return NULL;
	}
	return obj_simple_udata_luacheck(L, _index, type);
}

static FUNC_UNUSED void * obj_simple_udata_luadelete(lua_State *L, int _index, obj_type *type) {
	void *obj;
	obj = obj_simple_udata_luacheck(L, _index, type);
	/* clear the metatable to invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void *obj_simple_udata_luapush(lua_State *L, void *obj, int size, obj_type *type)
{
	void *ud;
#if LUAJIT_FFI
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	if(nobj_ffi_support_enabled_hint && lua_isfunction(L, -1)) {
		/* call special FFI "void *" to FFI object convertion function. */
		lua_pushlightuserdata(L, obj);
		lua_call(L, 1, 1);
		return obj;
	}
#endif
	/* create new userdata. */
	ud = lua_newuserdata(L, size);
	memcpy(ud, obj, size);
	/* get obj_type metatable. */
#if LUAJIT_FFI
	lua_insert(L, -2); /* move userdata below metatable. */
#else
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
#endif
	lua_setmetatable(L, -2);

	return ud;
}

/* default simple object equal method. */
static FUNC_UNUSED int obj_simple_udata_default_equal(lua_State *L) {
	void *ud1 = obj_simple_udata_toobj(L, 1);
	size_t len1 = lua_rawlen(L, 1);
	void *ud2 = obj_simple_udata_toobj(L, 2);
	size_t len2 = lua_rawlen(L, 2);

	if(len1 == len2) {
		lua_pushboolean(L, (memcmp(ud1, ud2, len1) == 0));
	} else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

/* default simple object tostring method. */
static FUNC_UNUSED int obj_simple_udata_default_tostring(lua_State *L) {
	void *ud = obj_simple_udata_toobj(L, 1);

	/* get object's metatable. */
	lua_getmetatable(L, 1);
	lua_remove(L, 1); /* remove userdata. */
	/* get the object's name from the metatable */
	lua_getfield(L, 1, ".name");
	lua_remove(L, 1); /* remove metatable */
	/* push object's pointer */
	lua_pushfstring(L, ": %p", ud);
	lua_concat(L, 2);

	return 1;
}

static int obj_constructor_call_wrapper(lua_State *L) {
	/* replace '__call' table with constructor function. */
	lua_pushvalue(L, lua_upvalueindex(1));
	lua_replace(L, 1);

	/* call constructor function with all parameters after the '__call' table. */
	lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
	/* return all results from constructor. */
	return lua_gettop(L);
}

static void obj_type_register_constants(lua_State *L, const obj_const *constants, int tab_idx,
		int bidirectional) {
	/* register constants. */
	while(constants->name != NULL) {
		lua_pushstring(L, constants->name);
		switch(constants->type) {
		case CONST_BOOLEAN:
			lua_pushboolean(L, constants->num != 0.0);
			break;
		case CONST_NUMBER:
			lua_pushnumber(L, constants->num);
			break;
		case CONST_STRING:
			lua_pushstring(L, constants->str);
			break;
		default:
			lua_pushnil(L);
			break;
		}
		/* map values back to keys. */
		if(bidirectional) {
			/* check if value already exists. */
			lua_pushvalue(L, -1);
			lua_rawget(L, tab_idx - 3);
			if(lua_isnil(L, -1)) {
				lua_pop(L, 1);
				/* add value->key mapping. */
				lua_pushvalue(L, -1);
				lua_pushvalue(L, -3);
				lua_rawset(L, tab_idx - 4);
			} else {
				/* value already exists. */
				lua_pop(L, 1);
			}
		}
		lua_rawset(L, tab_idx - 2);
		constants++;
	}
}

static void obj_type_register_package(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_Reg *reg_list = type_reg->pub_funcs;

	/* create public functions table. */
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		nobj_setfuncs(L, reg_list, 0);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	lua_pop(L, 1);  /* drop package table */
}

static void obj_type_register_meta(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_Reg *reg_list;

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		nobj_setfuncs(L, reg_list, 0);
	}

	obj_type_register_constants(L, type_reg->constants, -1, type_reg->bidirectional_consts);

	/* register methods. */
	nobj_setfuncs(L, type_reg->methods, 0);

	/* create metatable table. */
	lua_newtable(L);
	nobj_setfuncs(L, type_reg->metas, 0); /* fill metatable */
	/* setmetatable on meta-object. */
	lua_setmetatable(L, -2);

	lua_pop(L, 1);  /* drop meta-object */
}

static void obj_type_register(lua_State *L, const reg_sub_module *type_reg, int priv_table) {
	const luaL_Reg *reg_list;
	obj_type *type = type_reg->type;
	const obj_base *base = type_reg->bases;

	if(type_reg->req_type == REG_PACKAGE) {
		obj_type_register_package(L, type_reg);
		return;
	}
	if(type_reg->req_type == REG_META) {
		obj_type_register_meta(L, type_reg);
		return;
	}

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register "constructors" as to object's public API */
		nobj_setfuncs(L, reg_list, 0); /* fill public API table. */

		/* make public API table callable as the default constructor. */
		lua_newtable(L); /* create metatable */
		lua_pushliteral(L, "__call");
		lua_pushcfunction(L, reg_list[0].func); /* push first constructor function. */
		lua_pushcclosure(L, obj_constructor_call_wrapper, 1); /* make __call wrapper. */
		lua_rawset(L, -3);         /* metatable.__call = <default constructor> */

#if OBJ_DATA_HIDDEN_METATABLE
		lua_pushliteral(L, "__metatable");
		lua_pushboolean(L, 0);
		lua_rawset(L, -3);         /* metatable.__metatable = false */
#endif

		/* setmetatable on public API table. */
		lua_setmetatable(L, -2);

		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
	} else {
		/* register all methods as public functions. */
#if OBJ_DATA_HIDDEN_METATABLE
		lua_pop(L, 1); /* pop public API table, don't need it any more. */
		/* create methods table. */
		lua_newtable(L);
#endif
	}

	nobj_setfuncs(L, type_reg->methods, 0); /* fill methods table. */

	luaL_newmetatable(L, type->name); /* create metatable */
	lua_pushliteral(L, ".name");
	lua_pushstring(L, type->name);
	lua_rawset(L, -3);    /* metatable['.name'] = "<object_name>" */
	lua_pushliteral(L, ".type");
	lua_pushlightuserdata(L, type);
	lua_rawset(L, -3);    /* metatable['.type'] = lightuserdata -> obj_type */
	lua_pushlightuserdata(L, type);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, LUA_REGISTRYINDEX);    /* REGISTRY[type] = metatable */

	/* add metatable to 'priv_table' */
	lua_pushstring(L, type->name);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, priv_table);    /* priv_table["<object_name>"] = metatable */

	nobj_setfuncs(L, type_reg->metas, 0); /* fill metatable */

	/* add obj_bases to metatable. */
	while(base->id >= 0) {
		lua_pushlightuserdata(L, (void *)base);
		lua_rawseti(L, -2, base->id);
		base++;
	}

	obj_type_register_constants(L, type_reg->constants, -2, type_reg->bidirectional_consts);

	obj_type_register_implements(L, type_reg->implements);

	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);               /* dup methods table */
	lua_rawset(L, -3);                  /* metatable.__index = methods */
#if OBJ_DATA_HIDDEN_METATABLE
	lua_pushliteral(L, "__metatable");
	lua_pushboolean(L, 0);
	lua_rawset(L, -3);                  /* hide metatable:
	                                       metatable.__metatable = false */
#endif
	lua_pop(L, 2);                      /* drop metatable & methods */
}

static FUNC_UNUSED int lua_checktype_ref(lua_State *L, int _index, int _type) {
	luaL_checktype(L,_index,_type);
	lua_pushvalue(L,_index);
	return luaL_ref(L, LUA_REGISTRYINDEX);
}

/* use static pointer as key to weak callback_state table. */
static char obj_callback_state_weak_ref_key[] = "obj_callback_state_weak_ref_key";

static FUNC_UNUSED void *nobj_get_callback_state(lua_State *L, int owner_idx, int size) {
	void *cb_state;

	lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		/* create weak table for callback_state */
		lua_newtable(L);               /* weak table. */
		lua_newtable(L);               /* metatable for weak table. */
		lua_pushliteral(L, "__mode");
		lua_pushliteral(L, "k");
		lua_rawset(L, -3);             /* metatable.__mode = 'k'  weak keys. */
		lua_setmetatable(L, -2);       /* add metatable to weak table. */
		lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
		lua_pushvalue(L, -2);          /* dup weak table. */
		lua_rawset(L, LUA_REGISTRYINDEX);  /* add weak table to registry. */
	}

	/* check weak table for callback_state. */
	lua_pushvalue(L, owner_idx); /* dup. owner as lookup key. */
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil. */
		lua_pushvalue(L, owner_idx); /* dup. owner as lookup key. */
		/* create new callback state. */
		cb_state = lua_newuserdata(L, size);
		lua_rawset(L, -3);
		lua_pop(L, 1); /* pop <weak table> */
	} else {
		/* got existing callback state. */
		cb_state = lua_touserdata(L, -1);
		lua_pop(L, 2); /* pop <weak table>, <callback_state> */
	}

	return cb_state;
}

static FUNC_UNUSED void *nobj_delete_callback_state(lua_State *L, int owner_idx) {
	void *cb_state = NULL;

	lua_pushlightuserdata(L, obj_callback_state_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop nil.  no weak table, so there is no callback state. */
		return NULL;
	}
	/* get callback state. */
	lua_pushvalue(L, owner_idx); /* dup. owner */
	lua_rawget(L, -2);
	if(lua_isnil(L, -1)) {
		lua_pop(L, 2); /* pop <weak table>, nil.  No callback state for the owner. */
	} else {
		cb_state = lua_touserdata(L, -1);
		lua_pop(L, 1); /* pop <state> */
		/* remove callback state. */
		lua_pushvalue(L, owner_idx); /* dup. owner */
		lua_pushnil(L);
		lua_rawset(L, -3);
		lua_pop(L, 1); /* pop <weak table> */
	}

	return cb_state;
}





typedef struct Buffer_if {
  const uint8_t * (* const const_data)(void *this_v);
  size_t (* const get_size)(void *this_v);
} BufferIF;

/* a per-module unique pointer for fast lookup of an interface's implementation table. */
static char obj_interface_BufferIF[] = "BufferIF";

#define BufferIF_VAR(var_name) \
	BufferIF *var_name ## _if; \
	void *var_name;

#define BufferIF_LUA_OPTIONAL(L, _index, var_name) \
	var_name = obj_implement_luaoptional(L, _index, (void **)&(var_name ## _if), \
		obj_interface_BufferIF)

#define BufferIF_LUA_CHECK(L, _index, var_name) \
	var_name = obj_implement_luacheck(L, _index, (void **)&(var_name ## _if), \
		obj_interface_BufferIF)






typedef struct MutableBuffer_if {
  uint8_t * (* const data)(void *this_v);
  size_t (* const get_size)(void *this_v);
} MutableBufferIF;

/* a per-module unique pointer for fast lookup of an interface's implementation table. */
static char obj_interface_MutableBufferIF[] = "MutableBufferIF";

#define MutableBufferIF_VAR(var_name) \
	MutableBufferIF *var_name ## _if; \
	void *var_name;

#define MutableBufferIF_LUA_OPTIONAL(L, _index, var_name) \
	var_name = obj_implement_luaoptional(L, _index, (void **)&(var_name ## _if), \
		obj_interface_MutableBufferIF)

#define MutableBufferIF_LUA_CHECK(L, _index, var_name) \
	var_name = obj_implement_luacheck(L, _index, (void **)&(var_name ## _if), \
		obj_interface_MutableBufferIF)




static char *obj_interfaces[] = {
  obj_interface_BufferIF,
  obj_interface_MutableBufferIF,
  NULL,
};

/**
 * zmq_msg_t implements Buffer interface
 */
/** 
 * Buffer interface method const_data
 */
static const uint8_t * zmq_msg_t_Buffer_const_data(void *this_v) {
  zmq_msg_t * this_p = this_v;
  return zmq_msg_data(this_p);
}
/** 
 * Buffer interface method get_size
 */
static size_t zmq_msg_t_Buffer_get_size(void *this_v) {
  zmq_msg_t * this_p = this_v;
  return zmq_msg_size(this_p);
}

static const BufferIF zmq_msg_t_Buffer = {
  zmq_msg_t_Buffer_const_data,
  zmq_msg_t_Buffer_get_size
};

/**
 * zmq_msg_t implements MutableBuffer interface
 */
/** 
 * MutableBuffer interface method data
 */
static uint8_t * zmq_msg_t_MutableBuffer_data(void *this_v) {
  zmq_msg_t * this_p = this_v;
  return zmq_msg_data(this_p);
}
/** 
 * MutableBuffer interface method get_size
 */
static size_t zmq_msg_t_MutableBuffer_get_size(void *this_v) {
  zmq_msg_t * this_p = this_v;
  return zmq_msg_size(this_p);
}

static const MutableBufferIF zmq_msg_t_MutableBuffer = {
  zmq_msg_t_MutableBuffer_data,
  zmq_msg_t_MutableBuffer_get_size
};



#define obj_type_zmq_msg_t_check(L, _index) \
	(zmq_msg_t *)obj_simple_udata_luacheck(L, _index, &(obj_type_zmq_msg_t))
#define obj_type_zmq_msg_t_optional(L, _index) \
	(zmq_msg_t *)obj_simple_udata_luaoptional(L, _index, &(obj_type_zmq_msg_t))
#define obj_type_zmq_msg_t_delete(L, _index) \
	(zmq_msg_t *)obj_simple_udata_luadelete(L, _index, &(obj_type_zmq_msg_t))
#define obj_type_zmq_msg_t_push(L, obj) \
	obj_simple_udata_luapush(L, obj, sizeof(zmq_msg_t), &(obj_type_zmq_msg_t))

#define obj_type_ZMQ_Socket_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_ZMQ_Socket))
#define obj_type_ZMQ_Socket_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_ZMQ_Socket))
#define obj_type_ZMQ_Socket_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_ZMQ_Socket), flags)
#define obj_type_ZMQ_Socket_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_ZMQ_Socket), flags)

#define obj_type_ZMQ_Poller_check(L, _index) \
	(ZMQ_Poller *)obj_simple_udata_luacheck(L, _index, &(obj_type_ZMQ_Poller))
#define obj_type_ZMQ_Poller_optional(L, _index) \
	(ZMQ_Poller *)obj_simple_udata_luaoptional(L, _index, &(obj_type_ZMQ_Poller))
#define obj_type_ZMQ_Poller_delete(L, _index) \
	(ZMQ_Poller *)obj_simple_udata_luadelete(L, _index, &(obj_type_ZMQ_Poller))
#define obj_type_ZMQ_Poller_push(L, obj) \
	obj_simple_udata_luapush(L, obj, sizeof(ZMQ_Poller), &(obj_type_ZMQ_Poller))

#define obj_type_ZMQ_Ctx_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_ZMQ_Ctx))
#define obj_type_ZMQ_Ctx_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_ZMQ_Ctx))
#define obj_type_ZMQ_Ctx_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_ZMQ_Ctx), flags)
#define obj_type_ZMQ_Ctx_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_ZMQ_Ctx), flags)

#define obj_type_ZMQ_StopWatch_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_ZMQ_StopWatch))
#define obj_type_ZMQ_StopWatch_optional(L, _index) \
	obj_udata_luaoptional(L, _index, &(obj_type_ZMQ_StopWatch))
#define obj_type_ZMQ_StopWatch_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_ZMQ_StopWatch), flags)
#define obj_type_ZMQ_StopWatch_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_ZMQ_StopWatch), flags)




static const char *zmq_ffi_lua_code[] = { "local ffi=require\"ffi\"\n"
"local function ffi_safe_load(name, global)\n"
"	local stat, C = pcall(ffi.load, name, global)\n"
"	if not stat then return nil, C end\n"
"	if global then return ffi.C end\n"
"	return C\n"
"end\n"
"local function ffi_load(name, global)\n"
"	return assert(ffi_safe_load(name, global))\n"
"end\n"
"\n"
"local ffi_string = ffi.string\n"
"\n"
"local f_cast = ffi.cast\n"
"local pcall = pcall\n"
"local error = error\n"
"local type = type\n"
"local tonumber = tonumber\n"
"local tostring = tostring\n"
"local sformat = require\"string\".format\n"
"local rawset = rawset\n"
"local setmetatable = setmetatable\n"
"local package = (require\"package\") or {}\n"
"local p_config = package.config\n"
"local p_cpath = package.cpath\n"
"\n"
"\n"
"local ffi_load_cmodule\n"
"\n"
"-- try to detect luvit.\n"
"if p_config == nil and p_cpath == nil then\n"
"	ffi_load_cmodule = function(name, global)\n"
"		for path,module in pairs(package.loaded) do\n"
"			if module == name then\n"
"				local C, err = ffi_safe_load(path, global)\n"
"				-- return opened library\n"
"				if C then return C end\n"
"			end\n"
"		end\n"
"		error(\"Failed to find: \" .. name)\n"
"	end\n"
"else\n"
"	ffi_load_cmodule = function(name, global)\n"
"		local dir_sep = p_config:sub(1,1)\n"
"		local path_sep = p_config:sub(3,3)\n"
"		local path_mark = p_config:sub(5,5)\n"
"		local path_match = \"([^\" .. path_sep .. \"]+)\"\n"
"		-- convert dotted name to directory path.\n"
"		name = name:gsub('%.', dir_sep)\n"
"		-- try each path in search path.\n"
"		for path in p_cpath:gmatch(path_match) do\n"
"			local fname = path:gsub(path_mark, name)\n"
"			local C, err = ffi_safe_load(fname, global)\n"
"			-- return opened library\n"
"			if C then return C end\n"
"		end\n"
"		error(\"Failed to find: \" .. name)\n"
"	end\n"
"end\n"
"\n"
"local _M, _priv, reg_table = ...\n"
"local REG_OBJECTS_AS_GLOBALS = false\n"
"local C = ffi.C\n"
"\n"
"local OBJ_UDATA_FLAG_OWN		= 1\n"
"\n"
"local function ffi_safe_cdef(block_name, cdefs)\n"
"	local fake_type = \"struct sentinel_\" .. block_name .. \"_ty\"\n"
"	local stat, size = pcall(ffi.sizeof, fake_type)\n"
"	if stat and size > 0 then\n"
"		-- already loaded this cdef block\n"
"		return\n"
"	end\n"
"	cdefs = fake_type .. \"{ int a; int b; int c; };\" .. cdefs\n"
"	return ffi.cdef(cdefs)\n"
"end\n"
"\n"
"ffi_safe_cdef(\"LuaNativeObjects\", [[\n"
"\n"
"typedef struct obj_type obj_type;\n"
"\n"
"typedef void (*base_caster_t)(void **obj);\n"
"\n"
"typedef void (*dyn_caster_t)(void **obj, obj_type **type);\n"
"\n"
"struct obj_type {\n"
"	dyn_caster_t    dcaster;  /**< caster to support casting to sub-objects. */\n"
"	int32_t         id;       /**< type's id. */\n"
"	uint32_t        flags;    /**< type's flags (weak refs) */\n"
"	const char      *name;    /**< type's object name. */\n"
"};\n"
"\n"
"typedef struct obj_base {\n"
"	int32_t        id;\n"
"	base_caster_t  bcaster;\n"
"} obj_base;\n"
"\n"
"typedef struct obj_udata {\n"
"	void     *obj;\n"
"	uint32_t flags;  /**< lua_own:1bit */\n"
"} obj_udata;\n"
"\n"
"int memcmp(const void *s1, const void *s2, size_t n);\n"
"\n"
"]])\n"
"\n"
"local nobj_callback_states = {}\n"
"local nobj_weak_objects = setmetatable({}, {__mode = \"v\"})\n"
"local nobj_obj_flags = {}\n"
"\n"
"local function obj_ptr_to_id(ptr)\n"
"	return tonumber(f_cast('uintptr_t', ptr))\n"
"end\n"
"\n"
"local function obj_to_id(ptr)\n"
"	return tonumber(f_cast('uintptr_t', f_cast('void *', ptr)))\n"
"end\n"
"\n"
"local function register_default_constructor(_pub, obj_name, constructor)\n"
"	local obj_pub = _pub[obj_name]\n"
"	if type(obj_pub) == 'table' then\n"
"		-- copy table since it might have a locked metatable\n"
"		local new_pub = {}\n"
"		for k,v in pairs(obj_pub) do\n"
"			new_pub[k] = v\n"
"		end\n"
"		setmetatable(new_pub, { __call = function(t,...)\n"
"			return constructor(...)\n"
"		end,\n"
"		__metatable = false,\n"
"		})\n"
"		obj_pub = new_pub\n"
"	else\n"
"		obj_pub = constructor\n"
"	end\n"
"	_pub[obj_name] = obj_pub\n"
"	_M[obj_name] = obj_pub\n"
"	if REG_OBJECTS_AS_GLOBALS then\n"
"		_G[obj_name] = obj_pub\n"
"	end\n"
"end\n"
"\n"
"\n"
"-- detect zmq version\n"
"local VERSION_2_0 = true\n"
"local VERSION_2_1 = false\n"
"local VERSION_2_2 = false\n"
"local VERSION_3_0 = false\n"
"local VERSION_4_0 = false\n"
"local zver = _M.version()\n"
"if zver[1] == 4 then\n"
"	VERSION_2_0 = false\n"
"	VERSION_4_0 = true\n"
"elseif zver[1] == 3 then\n"
"	VERSION_2_0 = false\n"
"	VERSION_3_0 = true\n"
"elseif zver[1] == 2 and zver[2] == 2 then\n"
"	VERSION_2_2 = true\n"
"	VERSION_2_1 = true\n"
"elseif zver[1] == 2 and zver[2] == 1 then\n"
"	VERSION_2_1 = true\n"
"end\n"
"\n"
"if VERSION_2_0 then\n"
"	ffi.cdef[==[\n"
"typedef int ZMQ_Error;\n"
"typedef struct ZMQ_Socket ZMQ_Socket;\n"
"typedef struct zmq_msg_t zmq_msg_t;\n"
"\n"
"ZMQ_Error zmq_sendmsg(ZMQ_Socket *sock, zmq_msg_t *msg, int flags) __asm__(\"zmq_send\");\n"
"ZMQ_Error zmq_recvmsg(ZMQ_Socket *sock, zmq_msg_t *msg, int flags) __asm__(\"zmq_recv\");\n"
"]==]\n"
"end\n"
"\n"
"ffi_safe_cdef(\"BufferIF\", [[\n"
"typedef struct Buffer_if {\n"
"  const uint8_t * (* const const_data)(void *this_v);\n"
"  size_t (* const get_size)(void *this_v);\n"
"} BufferIF;\n"
"]])\n"
"\n"
"ffi_safe_cdef(\"FDIF\", [[\n"
"typedef struct FD_if {\n"
"  int (* const get_fd)(void *this_v);\n"
"  int (* const get_type)(void *this_v);\n"
"} FDIF;\n"
"]])\n"
"\n"
"ffi_safe_cdef(\"MutableBufferIF\", [[\n"
"typedef struct MutableBuffer_if {\n"
"  uint8_t * (* const data)(void *this_v);\n"
"  size_t (* const get_size)(void *this_v);\n"
"} MutableBufferIF;\n"
"]])\n"
"\n"
"local Cmod = ffi_load_cmodule(\"zmq\", false)\n"
"local C = Cmod\n"
"\n"
"ffi.cdef[[\n"
"typedef int ZMQ_Error;\n"
"\n"
"typedef struct zmq_msg_t zmq_msg_t;\n"
"typedef struct ZMQ_Socket ZMQ_Socket;\n"
"typedef struct ZMQ_Poller ZMQ_Poller;\n"
"typedef struct ZMQ_Ctx ZMQ_Ctx;\n"
"typedef struct ZMQ_StopWatch ZMQ_StopWatch;\n"
"\n"
"]]\n"
"\n"
"ffi.cdef[[\n"
"int zmq_errno (void);\n"
"\n"
"\n"
"struct zmq_msg_t\n"
"{\n"
"  unsigned char _ [64];\n"
"};\n"
"\n"
"int zmq_msg_init (zmq_msg_t *msg);\n"
"int zmq_msg_init_size (zmq_msg_t *msg, size_t size);\n"
"\n"
"\n"
"ZMQ_Error zmq_msg_init(zmq_msg_t *);\n"
"\n"
"ZMQ_Error zmq_msg_init_size(zmq_msg_t *, size_t);\n"
"\n"
"ZMQ_Error zmq_msg_close(zmq_msg_t *);\n"
"\n"
"ZMQ_Error zmq_msg_move(zmq_msg_t *, zmq_msg_t *);\n"
"\n"
"ZMQ_Error zmq_msg_copy(zmq_msg_t *, zmq_msg_t *);\n"
"\n"
"void * zmq_msg_data(zmq_msg_t *);\n"
"\n"
"size_t zmq_msg_size(zmq_msg_t *);\n"
"\n"
"ZMQ_Error zmq_close(ZMQ_Socket *);\n"
"\n"
"ZMQ_Error zmq_bind(ZMQ_Socket *, const char *);\n"
"\n"
"ZMQ_Error zmq_unbind(ZMQ_Socket *, const char *);\n"
"\n"
"ZMQ_Error zmq_connect(ZMQ_Socket *, const char *);\n"
"\n"
"ZMQ_Error zmq_disconnect(ZMQ_Socket *, const char *);\n"
"\n"
"int zmq_setsockopt (void *s, int option, const void *optval, size_t optvallen);\n"
"int zmq_getsockopt (void *s, int option, void *optval, size_t *optvallen);\n"
"\n"
"ZMQ_Error zmq_sendmsg(ZMQ_Socket *, zmq_msg_t *, int);\n"
"\n"
"ZMQ_Error simple_zmq_send(ZMQ_Socket *, const char *, size_t, int);\n"
"\n"
"ZMQ_Error zmq_recvmsg(ZMQ_Socket *, zmq_msg_t *, int);\n"
"\n"
"ZMQ_Error zmq_socket_monitor(ZMQ_Socket *, const char *, int);\n"
"\n"
"typedef struct ZMQ_recv_event {\n"
"	int        event_id;\n"
"	int        value;\n"
"	const char *addr;\n"
"	size_t     addr_len;\n"
"	const char *err;\n"
"} ZMQ_recv_event;\n"
"\n"
"int monitor_recv_event(ZMQ_Socket *s, zmq_msg_t *msg, int flags, ZMQ_recv_event *ev);\n"
"\n"
"ZMQ_Error lzmq_socket_hwm(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_hwm(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_swap(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_swap(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_affinity(ZMQ_Socket *, uint64_t*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_affinity(ZMQ_Socket *, uint64_t);\n"
"\n"
"ZMQ_Error lzmq_socket_identity(ZMQ_Socket *, char *, size_t*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_identity(ZMQ_Socket *, const char *, size_t);\n"
"\n"
"ZMQ_Error lzmq_socket_subscribe(ZMQ_Socket *, const char *, size_t);\n"
"\n"
"ZMQ_Error lzmq_socket_unsubscribe(ZMQ_Socket *, const char *, size_t);\n"
"\n"
"ZMQ_Error lzmq_socket_rate(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_rate(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_recovery_ivl(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_recovery_ivl(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_mcast_loop(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_mcast_loop(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_sndbuf(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_sndbuf(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_rcvbuf(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_rcvbuf(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_rcvmore(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_fd(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_events(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_type(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_linger(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_linger(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_reconnect_ivl(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_reconnect_ivl(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_backlog(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_backlog(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_recovery_ivl_msec(ZMQ_Socket *, int64_t*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_recovery_ivl_msec(ZMQ_Socket *, int64_t);\n"
"\n"
"ZMQ_Error lzmq_socket_reconnect_ivl_max(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_reconnect_ivl_max(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_maxmsgsize(ZMQ_Socket *, int64_t*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_maxmsgsize(ZMQ_Socket *, int64_t);\n"
"\n"
"ZMQ_Error lzmq_socket_sndhwm(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_sndhwm(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_rcvhwm(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_rcvhwm(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_multicast_hops(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_multicast_hops(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_rcvtimeo(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_rcvtimeo(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_sndtimeo(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_sndtimeo(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_ipv4only(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_ipv4only(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_last_endpoint(ZMQ_Socket *, char *, size_t*);\n"
"\n"
"ZMQ_Error lzmq_socket_router_mandatory(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_tcp_keepalive(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_tcp_keepalive(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_tcp_keepalive_cnt(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_tcp_keepalive_cnt(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_tcp_keepalive_idle(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_tcp_keepalive_idle(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_tcp_keepalive_intvl(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_tcp_keepalive_intvl(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_tcp_accept_filter(ZMQ_Socket *, const char *, size_t);\n"
"\n"
"ZMQ_Error lzmq_socket_immediate(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_immediate(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_xpub_verbose(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_router_raw(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_ipv6(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_ipv6(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_mechanism(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_plain_server(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_plain_server(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_plain_username(ZMQ_Socket *, char *, size_t*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_plain_username(ZMQ_Socket *, const char *, size_t);\n"
"\n"
"ZMQ_Error lzmq_socket_plain_password(ZMQ_Socket *, char *, size_t*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_plain_password(ZMQ_Socket *, const char *, size_t);\n"
"\n"
"ZMQ_Error lzmq_socket_curve_server(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_curve_server(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_curve_publickey(ZMQ_Socket *, char *, size_t*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_curve_publickey(ZMQ_Socket *, const char *, size_t);\n"
"\n"
"ZMQ_Error lzmq_socket_curve_secretkey(ZMQ_Socket *, char *, size_t*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_curve_secretkey(ZMQ_Socket *, const char *, size_t);\n"
"\n"
"ZMQ_Error lzmq_socket_curve_serverkey(ZMQ_Socket *, char *, size_t*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_curve_serverkey(ZMQ_Socket *, const char *, size_t);\n"
"\n"
"ZMQ_Error lzmq_socket_probe_router(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_req_correlate(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_req_relaxed(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_conflate(ZMQ_Socket *, int*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_conflate(ZMQ_Socket *, int);\n"
"\n"
"ZMQ_Error lzmq_socket_zap_domain(ZMQ_Socket *, char *, size_t*);\n"
"\n"
"ZMQ_Error lzmq_socket_set_zap_domain(ZMQ_Socket *, const char *, size_t);\n"
"\n"
"typedef int socket_t;\n"
"typedef struct zmq_pollitem_t {\n"
"	ZMQ_Socket *socket;\n"
"	socket_t fd;\n"
"	short events;\n"
"	short revents;\n"
"} zmq_pollitem_t;\n"
"\n"
"int poller_find_sock_item(ZMQ_Poller *poller, ZMQ_Socket *sock);\n"
"int poller_find_fd_item(ZMQ_Poller *poller, socket_t fd);\n"
"int poller_get_free_item(ZMQ_Poller *poller);\n"
"int poller_poll(ZMQ_Poller *poller, long timeout);\n"
"void poller_remove_item(ZMQ_Poller *poller, int idx);\n"
"\n"
"\n"
"struct ZMQ_Poller {\n"
"	zmq_pollitem_t *items;\n"
"	int    next;\n"
"	int    count;\n"
"	int    free_list;\n"
"	int    len;\n"
"};\n"
"\n"
"void poller_init(ZMQ_Poller *, unsigned int);\n"
"\n"
"void poller_cleanup(ZMQ_Poller *);\n"
"\n"
"ZMQ_Error poller_poll(ZMQ_Poller *, long);\n"
"\n"
"int poller_next_revents(ZMQ_Poller *, int*);\n"
"\n"
"ZMQ_Error zmq_term(ZMQ_Ctx *);\n"
"\n"
"ZMQ_Socket * zmq_socket(ZMQ_Ctx *, int);\n"
"\n"
"int zmq_ctx_set(ZMQ_Ctx *, int, int);\n"
"\n"
"int zmq_ctx_get(ZMQ_Ctx *, int);\n"
"\n"
"ZMQ_StopWatch * zmq_stopwatch_start();\n"
"\n"
"unsigned long zmq_stopwatch_stop(ZMQ_StopWatch *);\n"
"\n"
"ZMQ_Ctx * zmq_init(int);\n"
"\n"
"ZMQ_Error zmq_device(int, ZMQ_Socket *, ZMQ_Socket *);\n"
"\n"
"ZMQ_Error zmq_proxy(ZMQ_Socket *, ZMQ_Socket *, ZMQ_Socket *);\n"
"\n"
"void zmq_sleep(int);\n"
"\n"
"\n"
"]]\n"
"\n"
"REG_OBJECTS_AS_GLOBALS = false\n"
"local _obj_interfaces_ffi = {}\n"
"local _pub = {}\n"
"local _meth = {}\n"
"local _push = {}\n"
"local _obj_subs = {}\n"
"for obj_name,mt in pairs(_priv) do\n"
"	if type(mt) == 'table' then\n"
"		_obj_subs[obj_name] = {}\n"
"		if mt.__index then\n"
"			_meth[obj_name] = mt.__index\n"
"		end\n"
"	end\n"
"end\n"
"for obj_name,pub in pairs(_M) do\n"
"	_pub[obj_name] = pub\n"
"end\n"
"\n"
"--\n"
"-- CData Metatable access\n"
"--\n"
"local _ctypes = {}\n"
"local _type_names = {}\n"
"local _get_mt_key = {}\n"
"local _ctype_meta_map = {}\n"
"\n"
"local f_typeof = ffi.typeof\n"
"local function get_cdata_type_id(cdata)\n"
"	return tonumber(f_typeof(cdata))\n"
"end\n"
"local function get_cdata_mt(cdata)\n"
"	return _ctype_meta_map[tonumber(f_typeof(cdata))]\n"
"end\n"
"\n"
"local function obj_register_ctype(name, ctype)\n"
"	local obj_mt = _priv[name]\n"
"	local obj_type = obj_mt['.type']\n"
"	local obj_ctype = ffi.typeof(ctype)\n"
"	local obj_type_id = tonumber(obj_ctype)\n"
"	_ctypes[name] = obj_ctype\n"
"	_type_names[name] = tostring(obj_ctype)\n"
"	_ctype_meta_map[obj_type_id] = obj_mt\n"
"	_ctype_meta_map[obj_mt] = obj_type_id\n"
"	return obj_mt, obj_type, obj_ctype\n"
"end\n"
"\n"
"--\n"
"-- Interfaces helper code.\n"
"--\n"
"local _obj_interfaces_key = \"obj_interfaces<1.0>_table_key\"\n"
"local _obj_interfaces_ud = reg_table[_obj_interfaces_key]\n"
"local _obj_interfaces_key_ffi = _obj_interfaces_key .. \"_LJ2_FFI\"\n"
"_obj_interfaces_ffi = reg_table[_obj_interfaces_key_ffi]\n"
"if not _obj_interfaces_ffi then\n", /* ----- CUT ----- */
"	-- create missing interfaces table for FFI bindings.\n"
"	_obj_interfaces_ffi = {}\n"
"	reg_table[_obj_interfaces_key_ffi] = _obj_interfaces_ffi\n"
"end\n"
"\n"
"local function obj_get_userdata_interface(if_name, expected_err)\n"
"	local impls_ud = _obj_interfaces_ud[if_name]\n"
"	if not impls_ud then\n"
"		impls_ud = {}\n"
"		_obj_interfaces_ud[if_name] = impls_ud\n"
"	end\n"
"	-- create cdata check function to be used by non-ffi bindings.\n"
"	if not impls_ud.cdata then\n"
"		function impls_ud.cdata(obj)\n"
"			return assert(impls_ud[get_cdata_mt(obj)], expected_err)\n"
"		end\n"
"	end\n"
"	return impls_ud\n"
"end\n"
"\n"
"local function obj_get_interface_check(if_name, expected_err)\n"
"	local impls_ffi = _obj_interfaces_ffi[if_name]\n"
"	if not impls_ffi then\n"
"		local if_type = ffi.typeof(if_name .. \" *\")\n"
"		local impls_ud = obj_get_userdata_interface(if_name, expected_err)\n"
"		-- create table for FFI-based interface implementations.\n"
"		impls_ffi = setmetatable({}, {\n"
"		__index = function(impls_ffi, mt)\n"
"			local impl = impls_ud[mt]\n"
"			if impl then\n"
"				-- cast to cdata\n"
"				impl = if_type(impl)\n"
"				rawset(impls_ffi, mt, impl)\n"
"			end\n"
"			return impl\n"
"		end})\n"
"		_obj_interfaces_ffi[if_name] = impls_ffi\n"
"\n"
"		-- create check function for this interface.\n"
"		function impls_ffi.check(obj)\n"
"			local impl\n"
"			if type(obj) == 'cdata' then\n"
"				impl = impls_ffi[get_cdata_type_id(obj)]\n"
"			else\n"
"				impl = impls_ud.userdata(impls_ffi, obj)\n"
"			end\n"
"			return assert(impl, expected_err)\n"
"		end\n"
"	end\n"
"	return impls_ffi.check\n"
"end\n"
"\n"
"local function obj_register_interface(if_name, obj_name)\n"
"	-- loopkup cdata id\n"
"	local obj_mt = _priv[obj_name]\n"
"	local obj_type_id = _ctype_meta_map[obj_mt]\n"
"	local impl_meths = {}\n"
"	local ffi_impls = _obj_interfaces_ffi[if_name]\n"
"	ffi_impls[obj_type_id] = impl_meths\n"
"	_meth[obj_name]['NOBJ_get_' .. if_name] = impl_meths\n"
"	return impl_meths\n"
"end\n"
"\n"
"\n"
"local obj_type_zmq_msg_t_check\n"
"local obj_type_zmq_msg_t_delete\n"
"local obj_type_zmq_msg_t_push\n"
"\n"
"do\n"
"	local obj_mt, obj_type, obj_ctype = obj_register_ctype(\"zmq_msg_t\", \"zmq_msg_t\")\n"
"	local zmq_msg_t_sizeof = ffi.sizeof\"zmq_msg_t\"\n"
"\n"
"	function obj_type_zmq_msg_t_check(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_type_zmq_msg_t_delete(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_type_zmq_msg_t_push(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_mt:__tostring()\n"
"		return sformat(\"zmq_msg_t: %p\", self)\n"
"	end\n"
"\n"
"	function obj_mt.__eq(val1, val2)\n"
"		if not ffi.istype(obj_ctype, val2) then return false end\n"
"		assert(ffi.istype(obj_ctype, val1), \"expected zmq_msg_t\")\n"
"		return (C.memcmp(val1, val2, zmq_msg_t_sizeof) == 0)\n"
"	end\n"
"\n"
"	-- type checking function for C API.\n"
"	local function c_check(obj)\n"
"		if ffi.istype(obj_ctype, obj) then return obj end\n"
"		return nil\n"
"	end\n"
"	_priv[obj_type] = c_check\n"
"	-- push function for C API.\n"
"	reg_table[obj_type] = function(ptr)\n"
"		local obj = obj_ctype()\n"
"		ffi.copy(obj, ptr, zmq_msg_t_sizeof);\n"
"		return obj\n"
"	end\n"
"\n"
"	-- export check functions for use in other modules.\n"
"	obj_mt.c_check = c_check\n"
"	obj_mt.ffi_check = obj_type_zmq_msg_t_check\n"
"end\n"
"\n"
"\n"
"local obj_type_ZMQ_Socket_check\n"
"local obj_type_ZMQ_Socket_delete\n"
"local obj_type_ZMQ_Socket_push\n"
"\n"
"do\n"
"	local obj_mt, obj_type, obj_ctype = obj_register_ctype(\"ZMQ_Socket\", \"ZMQ_Socket *\")\n"
"\n"
"	function obj_type_ZMQ_Socket_check(ptr)\n"
"		-- if ptr is nil or is the correct type, then just return it.\n"
"		if not ptr or ffi.istype(obj_ctype, ptr) then return ptr end\n"
"		-- check if it is a compatible type.\n"
"		local ctype = tostring(ffi.typeof(ptr))\n"
"		local bcaster = _obj_subs.ZMQ_Socket[ctype]\n"
"		if bcaster then\n"
"			return bcaster(ptr)\n"
"		end\n"
"		return error(\"Expected 'ZMQ_Socket *'\", 2)\n"
"	end\n"
"\n"
"	function obj_type_ZMQ_Socket_delete(ptr)\n"
"		local id = obj_ptr_to_id(ptr)\n"
"		local flags = nobj_obj_flags[id]\n"
"		if not flags then return nil, 0 end\n"
"		ffi.gc(ptr, nil)\n"
"		nobj_obj_flags[id] = nil\n"
"		return ptr, flags\n"
"	end\n"
"\n"
"	function obj_type_ZMQ_Socket_push(ptr, flags)\n"
"		local id = obj_ptr_to_id(ptr)\n"
"		-- check weak refs\n"
"		if nobj_obj_flags[id] then return nobj_weak_objects[id] end\n"
"\n"
"		if flags ~= 0 then\n"
"			nobj_obj_flags[id] = flags\n"
"			ffi.gc(ptr, obj_mt.__gc)\n"
"		end\n"
"		nobj_weak_objects[id] = ptr\n"
"		return ptr\n"
"	end\n"
"\n"
"	function obj_mt:__tostring()\n"
"		return sformat(\"ZMQ_Socket: %p, flags=%d\", self, nobj_obj_flags[obj_ptr_to_id(self)] or 0)\n"
"	end\n"
"\n"
"	-- type checking function for C API.\n"
"	_priv[obj_type] = obj_type_ZMQ_Socket_check\n"
"	-- push function for C API.\n"
"	reg_table[obj_type] = function(ptr, flags)\n"
"		return obj_type_ZMQ_Socket_push(ffi.cast(obj_ctype,ptr), flags)\n"
"	end\n"
"\n"
"	-- export check functions for use in other modules.\n"
"	obj_mt.c_check = obj_type_ZMQ_Socket_check\n"
"	obj_mt.ffi_check = obj_type_ZMQ_Socket_check\n"
"end\n"
"\n"
"\n"
"local obj_type_ZMQ_Poller_check\n"
"local obj_type_ZMQ_Poller_delete\n"
"local obj_type_ZMQ_Poller_push\n"
"\n"
"do\n"
"	local obj_mt, obj_type, obj_ctype = obj_register_ctype(\"ZMQ_Poller\", \"ZMQ_Poller\")\n"
"	local ZMQ_Poller_sizeof = ffi.sizeof\"ZMQ_Poller\"\n"
"\n"
"	function obj_type_ZMQ_Poller_check(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_type_ZMQ_Poller_delete(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_type_ZMQ_Poller_push(obj)\n"
"		return obj\n"
"	end\n"
"\n"
"	function obj_mt:__tostring()\n"
"		return sformat(\"ZMQ_Poller: %p\", self)\n"
"	end\n"
"\n"
"	function obj_mt.__eq(val1, val2)\n"
"		if not ffi.istype(obj_ctype, val2) then return false end\n"
"		assert(ffi.istype(obj_ctype, val1), \"expected ZMQ_Poller\")\n"
"		return (C.memcmp(val1, val2, ZMQ_Poller_sizeof) == 0)\n"
"	end\n"
"\n"
"	-- type checking function for C API.\n"
"	local function c_check(obj)\n"
"		if ffi.istype(obj_ctype, obj) then return obj end\n"
"		return nil\n"
"	end\n"
"	_priv[obj_type] = c_check\n"
"	-- push function for C API.\n"
"	reg_table[obj_type] = function(ptr)\n"
"		local obj = obj_ctype()\n"
"		ffi.copy(obj, ptr, ZMQ_Poller_sizeof);\n"
"		return obj\n"
"	end\n"
"\n"
"	-- export check functions for use in other modules.\n"
"	obj_mt.c_check = c_check\n"
"	obj_mt.ffi_check = obj_type_ZMQ_Poller_check\n"
"end\n"
"\n"
"\n"
"local obj_type_ZMQ_Ctx_check\n"
"local obj_type_ZMQ_Ctx_delete\n"
"local obj_type_ZMQ_Ctx_push\n"
"\n"
"do\n"
"	local obj_mt, obj_type, obj_ctype = obj_register_ctype(\"ZMQ_Ctx\", \"ZMQ_Ctx *\")\n"
"\n"
"	function obj_type_ZMQ_Ctx_check(ptr)\n"
"		-- if ptr is nil or is the correct type, then just return it.\n"
"		if not ptr or ffi.istype(obj_ctype, ptr) then return ptr end\n"
"		-- check if it is a compatible type.\n"
"		local ctype = tostring(ffi.typeof(ptr))\n"
"		local bcaster = _obj_subs.ZMQ_Ctx[ctype]\n"
"		if bcaster then\n"
"			return bcaster(ptr)\n"
"		end\n"
"		return error(\"Expected 'ZMQ_Ctx *'\", 2)\n"
"	end\n"
"\n"
"	function obj_type_ZMQ_Ctx_delete(ptr)\n"
"		local id = obj_ptr_to_id(ptr)\n"
"		local flags = nobj_obj_flags[id]\n"
"		if not flags then return nil, 0 end\n"
"		ffi.gc(ptr, nil)\n"
"		nobj_obj_flags[id] = nil\n"
"		return ptr, flags\n"
"	end\n"
"\n"
"	function obj_type_ZMQ_Ctx_push(ptr, flags)\n"
"		local id = obj_ptr_to_id(ptr)\n"
"		-- check weak refs\n"
"		if nobj_obj_flags[id] then return nobj_weak_objects[id] end\n"
"\n"
"		if flags ~= 0 then\n"
"			nobj_obj_flags[id] = flags\n"
"			ffi.gc(ptr, obj_mt.__gc)\n"
"		end\n"
"		nobj_weak_objects[id] = ptr\n"
"		return ptr\n"
"	end\n"
"\n"
"	function obj_mt:__tostring()\n"
"		return sformat(\"ZMQ_Ctx: %p, flags=%d\", self, nobj_obj_flags[obj_ptr_to_id(self)] or 0)\n"
"	end\n"
"\n"
"	-- type checking function for C API.\n"
"	_priv[obj_type] = obj_type_ZMQ_Ctx_check\n"
"	-- push function for C API.\n"
"	reg_table[obj_type] = function(ptr, flags)\n"
"		return obj_type_ZMQ_Ctx_push(ffi.cast(obj_ctype,ptr), flags)\n"
"	end\n"
"\n"
"	-- export check functions for use in other modules.\n"
"	obj_mt.c_check = obj_type_ZMQ_Ctx_check\n"
"	obj_mt.ffi_check = obj_type_ZMQ_Ctx_check\n"
"end\n"
"\n"
"\n"
"local obj_type_ZMQ_StopWatch_check\n"
"local obj_type_ZMQ_StopWatch_delete\n"
"local obj_type_ZMQ_StopWatch_push\n"
"\n"
"do\n"
"	local obj_mt, obj_type, obj_ctype = obj_register_ctype(\"ZMQ_StopWatch\", \"ZMQ_StopWatch *\")\n"
"\n"
"	function obj_type_ZMQ_StopWatch_check(ptr)\n"
"		-- if ptr is nil or is the correct type, then just return it.\n"
"		if not ptr or ffi.istype(obj_ctype, ptr) then return ptr end\n"
"		-- check if it is a compatible type.\n"
"		local ctype = tostring(ffi.typeof(ptr))\n"
"		local bcaster = _obj_subs.ZMQ_StopWatch[ctype]\n"
"		if bcaster then\n"
"			return bcaster(ptr)\n"
"		end\n"
"		return error(\"Expected 'ZMQ_StopWatch *'\", 2)\n"
"	end\n"
"\n"
"	function obj_type_ZMQ_StopWatch_delete(ptr)\n"
"		local id = obj_ptr_to_id(ptr)\n"
"		local flags = nobj_obj_flags[id]\n"
"		if not flags then return nil, 0 end\n"
"		ffi.gc(ptr, nil)\n"
"		nobj_obj_flags[id] = nil\n"
"		return ptr, flags\n"
"	end\n"
"\n"
"	function obj_type_ZMQ_StopWatch_push(ptr, flags)\n"
"		local id = obj_ptr_to_id(ptr)\n"
"		-- check weak refs\n"
"		if nobj_obj_flags[id] then return nobj_weak_objects[id] end\n"
"\n"
"		if flags ~= 0 then\n"
"			nobj_obj_flags[id] = flags\n"
"			ffi.gc(ptr, obj_mt.__gc)\n"
"		end\n"
"		nobj_weak_objects[id] = ptr\n"
"		return ptr\n"
"	end\n"
"\n"
"	function obj_mt:__tostring()\n"
"		return sformat(\"ZMQ_StopWatch: %p, flags=%d\", self, nobj_obj_flags[obj_ptr_to_id(self)] or 0)\n"
"	end\n"
"\n"
"	-- type checking function for C API.\n"
"	_priv[obj_type] = obj_type_ZMQ_StopWatch_check\n"
"	-- push function for C API.\n"
"	reg_table[obj_type] = function(ptr, flags)\n"
"		return obj_type_ZMQ_StopWatch_push(ffi.cast(obj_ctype,ptr), flags)\n"
"	end\n"
"\n"
"	-- export check functions for use in other modules.\n"
"	obj_mt.c_check = obj_type_ZMQ_StopWatch_check\n"
"	obj_mt.ffi_check = obj_type_ZMQ_StopWatch_check\n"
"end\n"
"\n"
"\n"
"local obj_type_Buffer_check =\n"
"	obj_get_interface_check(\"BufferIF\", \"Expected object with Buffer interface\")\n"
"\n"
"local obj_type_FD_check =\n"
"	obj_get_interface_check(\"FDIF\", \"Expected object with FD interface\")\n"
"\n"
"local obj_type_MutableBuffer_check =\n"
"	obj_get_interface_check(\"MutableBufferIF\", \"Expected object with MutableBuffer interface\")\n"
"\n"
"local os_lib_table = {\n"
"	[\"Windows\"] = \"libzmq\",\n"
"}\n"
"C = ffi_load(os_lib_table[ffi.os] or \"zmq\")\n"
"\n"
"\n"
"-- Start \"ZErrors\" FFI interface\n"
"-- End \"ZErrors\" FFI interface\n"
"\n"
"-- get ZErrors table to map errno to error name.\n"
"local ZError_names = _M.ZErrors\n"
"\n"
"local function get_zmq_strerror()\n"
"	return ZError_names[C.zmq_errno()]\n"
"end\n"
"\n"
"local function error_code__ZMQ_Error__push(err)\n"
"  local err_str\n"
"	if(-1 == err) then\n"
"		err_str = ZError_names[C.zmq_errno()]\n"
"	end\n"
"\n"
"	return err_str\n"
"end\n"
"\n"
"\n"
"-- Start \"zmq_msg_t\" FFI interface\n"
"-- method: init\n"
"function _pub.zmq_msg_t.init()\n"
"  local self = ffi.new(\"zmq_msg_t\")\n"
"  local rc_zmq_msg_init2 = 0\n"
"  rc_zmq_msg_init2 = C.zmq_msg_init(self)\n"
"  if (-1 == rc_zmq_msg_init2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_zmq_msg_init2)\n"
"  end\n"
"  return obj_type_zmq_msg_t_push(self)\n"
"end\n"
"register_default_constructor(_pub,\"zmq_msg_t\",_pub.zmq_msg_t.init)\n"
"\n"
"-- method: init_size\n"
"function _pub.zmq_msg_t.init_size(size1)\n"
"  \n"
"  local self = ffi.new(\"zmq_msg_t\")\n"
"  local rc_zmq_msg_init_size2 = 0\n"
"  rc_zmq_msg_init_size2 = C.zmq_msg_init_size(self, size1)\n"
"  if (-1 == rc_zmq_msg_init_size2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_zmq_msg_init_size2)\n"
"  end\n"
"  return obj_type_zmq_msg_t_push(self)\n"
"end\n"
"\n"
"-- method: init_data\n"
"function _pub.zmq_msg_t.init_data(data1)\n"
"  local data_len1 = #data1\n"
"  local self = ffi.new(\"zmq_msg_t\")\n"
"  local err2 = 0\n"
"  err2 = C.zmq_msg_init_size(self, data_len1)\n"
"	if(0 == err2) then\n"
"		-- fill message\n"
"		ffi.copy(C.zmq_msg_data(self), data1, data_len1)\n"
"	end\n"
"\n"
"  if (-1 == err2) then\n"
"    return nil,error_code__ZMQ_Error__push(err2)\n"
"  end\n"
"  return obj_type_zmq_msg_t_push(self)\n"
"end\n"
"\n"
"-- method: __gc\n"
"function _priv.zmq_msg_t.__gc(self)\n"
"  local self = obj_type_zmq_msg_t_delete(self)\n"
"  if not self then return end\n"
"  local rc_zmq_msg_close1 = 0\n"
"  rc_zmq_msg_close1 = C.zmq_msg_close(self)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_msg_close1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_msg_close1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: close\n"
"function _meth.zmq_msg_t.close(self)\n"
"  \n"
"  local rc_zmq_msg_close1 = 0\n"
"  rc_zmq_msg_close1 = C.zmq_msg_close(self)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_msg_close1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_msg_close1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: move\n"
"function _meth.zmq_msg_t.move(self, src2)\n"
"  \n"
"  \n"
"  local rc_zmq_msg_move1 = 0\n"
"  rc_zmq_msg_move1 = C.zmq_msg_move(self, src2)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_msg_move1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_msg_move1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: copy\n"
"function _meth.zmq_msg_t.copy(self, src2)\n"
"  \n"
"  \n"
"  local rc_zmq_msg_copy1 = 0\n"
"  rc_zmq_msg_copy1 = C.zmq_msg_copy(self, src2)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_msg_copy1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_msg_copy1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: set_data\n"
"function _meth.zmq_msg_t.set_data(self, data2)\n"
"  \n"
"  local data_len2 = #data2\n"
"  local err1 = 0\n"
"	-- check message data size.\n"
"	if (C.zmq_msg_size(self) ~= data_len2) then\n"
"		-- need to resize message.\n"
"		C.zmq_msg_close(self); -- close old message, to free old data.\n"
"		err1 = C.zmq_msg_init_size(self, data_len2); -- re-initialize message.\n"
"		if (0 ~= err1) then\n"
"			error(\"set_data() failed: \" .. get_zmq_strerror());\n"
"		end\n"
"	end\n"
"	-- copy data into message\n"
"	ffi.copy(C.zmq_msg_data(self), data2, data_len2);\n"
"\n"
"  -- check for error.\n"
"  if (-1 == err1) then\n"
"    return nil, error_code__ZMQ_Error__push(err1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: data\n"
"function _meth.zmq_msg_t.data(self)\n"
"  \n"
"  local rc_zmq_msg_data1\n"
"  rc_zmq_msg_data1 = C.zmq_msg_data(self)\n"
"  return rc_zmq_msg_data1\n"
"end\n"
"\n"
"-- method: set_size\n"
"function _meth.zmq_msg_t.set_size(self, size2)\n"
"  \n"
"  \n"
"  local err1 = 0\n"
"	-- check message data size.\n"
"	if (C.zmq_msg_size(self) ~= size2) then\n"
"		-- need to resize message.\n"
"		C.zmq_msg_close(self); -- close old message, to free old data.\n"
"		err1 = C.zmq_msg_init_size(self, size2); -- re-initialize message.\n"
"		if (0 ~= err1) then\n"
"			error(\"set_size() failed: \" .. get_zmq_strerror());\n"
"		end\n"
"	end\n"
"\n"
"  -- check for error.\n"
"  if (-1 == err1) then\n"
"    return nil, error_code__ZMQ_Error__push(err1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: size\n"
"function _meth.zmq_msg_t.size(self)\n"
"  \n"
"  local size1 = 0\n"
"  size1 = C.zmq_msg_size(self)\n"
"  return tonumber(size1)\n"
"end\n"
"\n"
"-- method: __tostring\n"
"function _priv.zmq_msg_t.__tostring(self)\n"
"  \n"
"  local data_len1 = 0\n"
"  local data1\n"
"	data1 = C.zmq_msg_data(self);\n"
"	data_len1 = C.zmq_msg_size(self);\n"
"\n"
"  return data1 ~= nil and ffi_string(data1,data_len1) or nil\n"
"end\n"
"\n"
"-- zmq_msg_t implements Buffer interface\n"
"do\n"
"  local impl_meths = obj_register_interface(\"BufferIF\", \"zmq_msg_t\")\n"
"-- Buffer interface method const_data\n"
"impl_meths.const_data = C.zmq_msg_data\n"
"-- Buffer interface method get_size\n"
"impl_meths.get_size = C.zmq_msg_size\n"
"end\n"
"\n"
"-- zmq_msg_t implements MutableBuffer interface\n"
"do\n"
"  local impl_meths = obj_register_interface(\"MutableBufferIF\", \"zmq_msg_t\")\n", /* ----- CUT ----- */
"-- MutableBuffer interface method data\n"
"impl_meths.data = C.zmq_msg_data\n"
"-- MutableBuffer interface method get_size\n"
"impl_meths.get_size = C.zmq_msg_size\n"
"end\n"
"\n"
"_push.zmq_msg_t = obj_type_zmq_msg_t_push\n"
"ffi.metatype(\"zmq_msg_t\", _priv.zmq_msg_t)\n"
"-- End \"zmq_msg_t\" FFI interface\n"
"\n"
"\n"
"-- Start \"ZMQ_Socket\" FFI interface\n"
"-- method: close\n"
"function _meth.ZMQ_Socket.close(self)\n"
"  local self,this_flags1 = obj_type_ZMQ_Socket_delete(self)\n"
"  if not self then return end\n"
"  local rc_zmq_close1 = 0\n"
"  rc_zmq_close1 = C.zmq_close(self)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_close1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_close1)\n"
"  end\n"
"  return true\n"
"end\n"
"_priv.ZMQ_Socket.__gc = _meth.ZMQ_Socket.close\n"
"\n"
"-- method: bind\n"
"function _meth.ZMQ_Socket.bind(self, addr2)\n"
"  \n"
"  local addr_len2 = #addr2\n"
"  local rc_zmq_bind1 = 0\n"
"  rc_zmq_bind1 = C.zmq_bind(self, addr2)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_bind1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_bind1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: unbind\n"
"if (_meth.ZMQ_Socket.unbind) then\n"
"function _meth.ZMQ_Socket.unbind(self, addr2)\n"
"  \n"
"  local addr_len2 = #addr2\n"
"  local rc_zmq_unbind1 = 0\n"
"  rc_zmq_unbind1 = C.zmq_unbind(self, addr2)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_unbind1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_unbind1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: connect\n"
"function _meth.ZMQ_Socket.connect(self, addr2)\n"
"  \n"
"  local addr_len2 = #addr2\n"
"  local rc_zmq_connect1 = 0\n"
"  rc_zmq_connect1 = C.zmq_connect(self, addr2)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_connect1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_connect1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: disconnect\n"
"if (_meth.ZMQ_Socket.disconnect) then\n"
"function _meth.ZMQ_Socket.disconnect(self, addr2)\n"
"  \n"
"  local addr_len2 = #addr2\n"
"  local rc_zmq_disconnect1 = 0\n"
"  rc_zmq_disconnect1 = C.zmq_disconnect(self, addr2)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_disconnect1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_disconnect1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"local option_gets = {}\n"
"local option_sets = {}\n"
"\n"
"do\n"
"	local opt_name\n"
"	local methods = _meth.ZMQ_Socket\n"
"	setmetatable(option_gets,{__index = function(tab,opt)\n"
"		local opt_name = opt_name[opt]\n"
"		if not opt_name then return nil end\n"
"		local method = methods[opt_name]\n"
"		rawset(tab, opt, method)\n"
"		return method\n"
"	end})\n"
"	setmetatable(option_sets,{__index = function(tab,opt)\n"
"		local opt_name = opt_name[opt]\n"
"		if not opt_name then return nil end\n"
"		local method = methods['set_' .. opt_name] or methods[opt_name]\n"
"		rawset(tab, opt, method)\n"
"		return method\n"
"	end})\n"
"	opt_name = {\n"
"		[1] = 'hwm',\n"
"		[3] = 'swap',\n"
"		[4] = 'affinity',\n"
"		[5] = 'identity',\n"
"		[6] = 'subscribe',\n"
"		[7] = 'unsubscribe',\n"
"		[8] = 'rate',\n"
"		[9] = 'recovery_ivl',\n"
"		[10] = 'mcast_loop',\n"
"		[11] = 'sndbuf',\n"
"		[12] = 'rcvbuf',\n"
"		[13] = 'rcvmore',\n"
"		[14] = 'fd',\n"
"		[15] = 'events',\n"
"		[16] = 'type',\n"
"		[17] = 'linger',\n"
"		[18] = 'reconnect_ivl',\n"
"		[19] = 'backlog',\n"
"		[20] = 'recovery_ivl_msec',\n"
"		[21] = 'reconnect_ivl_max',\n"
"		[27] = 'rcvtimeo',\n"
"		[28] = 'sndtimeo',\n"
"		[22] = 'maxmsgsize',\n"
"		[23] = 'sndhwm',\n"
"		[24] = 'rcvhwm',\n"
"		[25] = 'multicast_hops',\n"
"		[31] = 'ipv4only',\n"
"		[32] = 'last_endpoint',\n"
"		[33] = 'router_mandatory',\n"
"		[34] = 'tcp_keepalive',\n"
"		[35] = 'tcp_keepalive_cnt',\n"
"		[36] = 'tcp_keepalive_idle',\n"
"		[37] = 'tcp_keepalive_intvl',\n"
"		[38] = 'tcp_accept_filter',\n"
"		[39] = 'immediate',\n"
"		[40] = 'xpub_verbose',\n"
"		[41] = 'router_raw',\n"
"		[42] = 'ipv6',\n"
"		[43] = 'mechanism',\n"
"		[44] = 'plain_server',\n"
"		[45] = 'plain_username',\n"
"		[46] = 'plain_password',\n"
"		[47] = 'curve_server',\n"
"		[48] = 'curve_publickey',\n"
"		[49] = 'curve_secretkey',\n"
"		[50] = 'curve_serverkey',\n"
"		[51] = 'probe_router',\n"
"		[52] = 'req_correlate',\n"
"		[53] = 'req_relaxed',\n"
"		[54] = 'conflate',\n"
"		[55] = 'zap_domain',\n"
"}\n"
"end\n"
"\n"
"\n"
"-- method: setopt\n"
"function _meth.ZMQ_Socket.setopt(self, opt2, val3)\n"
"  \n"
"  \n"
"  local err1 = 0\n"
"	local set = option_sets[opt2]\n"
"	if set then\n"
"		return set(self,val3)\n"
"	else\n"
"		error(\"Invalid socket option.\")\n"
"	end\n"
"\n"
"  -- check for error.\n"
"  if (-1 == err1) then\n"
"    return nil, error_code__ZMQ_Error__push(err1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"local tmp_val_len = ffi.new('size_t[1]', 4)\n"
"\n"
"-- method: getopt\n"
"function _meth.ZMQ_Socket.getopt(self, opt2)\n"
"  \n"
"  \n"
"  local val1\n"
"  local err2 = 0\n"
"	local get = option_gets[opt2]\n"
"	if get then\n"
"		return get(self)\n"
"	else\n"
"		error(\"Invalid socket option.\")\n"
"	end\n"
"\n"
"  return val1\n"
"end\n"
"\n"
"-- method: send_msg\n"
"function _meth.ZMQ_Socket.send_msg(self, msg2, flags3)\n"
"  \n"
"  \n"
"  flags3 = flags3 or 0\n"
"  local rc_zmq_sendmsg1 = 0\n"
"  rc_zmq_sendmsg1 = C.zmq_sendmsg(self, msg2, flags3)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_sendmsg1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_sendmsg1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: send\n"
"function _meth.ZMQ_Socket.send(self, data2, flags3)\n"
"  \n"
"  local data_len2 = #data2\n"
"  flags3 = flags3 or 0\n"
"  local rc_simple_zmq_send1 = 0\n"
"  rc_simple_zmq_send1 = Cmod.simple_zmq_send(self, data2, data_len2, flags3)\n"
"  -- check for error.\n"
"  if (-1 == rc_simple_zmq_send1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_simple_zmq_send1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"-- method: recv_msg\n"
"function _meth.ZMQ_Socket.recv_msg(self, msg2, flags3)\n"
"  \n"
"  \n"
"  flags3 = flags3 or 0\n"
"  local rc_zmq_recvmsg1 = 0\n"
"  rc_zmq_recvmsg1 = C.zmq_recvmsg(self, msg2, flags3)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_recvmsg1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_recvmsg1)\n"
"  end\n"
"  return true\n"
"end\n"
"\n"
"local tmp_msg = ffi.new('zmq_msg_t')\n"
"\n"
"-- method: recv\n"
"function _meth.ZMQ_Socket.recv(self, flags2)\n"
"  \n"
"  flags2 = flags2 or 0\n"
"  local data_len1 = 0\n"
"  local data1\n"
"  local err2 = 0\n"
"	local msg = tmp_msg\n"
"	-- initialize blank message.\n"
"	if C.zmq_msg_init(msg) < 0 then\n"
"		return nil, get_zmq_strerror()\n"
"	end\n"
"\n"
"	-- receive message\n"
"	err2 = C.zmq_recvmsg(self, msg, flags2)\n"
"	if err2 >= 0 then\n"
"		local data = ffi.string(C.zmq_msg_data(msg), C.zmq_msg_size(msg))\n"
"		-- close message\n"
"		C.zmq_msg_close(msg)\n"
"		return data\n"
"	end\n"
"	-- close message\n"
"	C.zmq_msg_close(msg)\n"
"\n"
"  if (-1 == err2) then\n"
"    return nil,error_code__ZMQ_Error__push(err2)\n"
"  end\n"
"  return data1 ~= nil and ffi_string(data1,data_len1) or nil\n"
"end\n"
"\n"
"-- method: monitor\n"
"if (_meth.ZMQ_Socket.monitor) then\n"
"function _meth.ZMQ_Socket.monitor(self, addr2, events3)\n"
"  \n"
"  local addr_len2 = #addr2\n"
"  \n"
"  local rc_zmq_socket_monitor1 = 0\n"
"  rc_zmq_socket_monitor1 = C.zmq_socket_monitor(self, addr2, events3)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_socket_monitor1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_socket_monitor1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"local tmp_recv_event = ffi.new('ZMQ_recv_event')\n"
"\n"
"-- method: recv_event\n"
"if (_meth.ZMQ_Socket.recv_event) then\n"
"function _meth.ZMQ_Socket.recv_event(self, flags2)\n"
"  \n"
"  flags2 = flags2 or 0\n"
"  local event_id1 = 0\n"
"  local value2 = 0\n"
"  local addr_len3 = 0\n"
"  local addr3\n"
"  local err4 = 0\n"
"	local msg = tmp_msg\n"
"	local event = tmp_recv_event\n"
"	local addr\n"
"\n"
"	-- receive monitor event\n"
"	err4 = Cmod.monitor_recv_event(self, msg, flags2, event)\n"
"	if err4 >= 0 then\n"
"		addr = ffi.string(event.addr, event.addr_len)\n"
"		-- close message\n"
"		C.zmq_msg_close(msg)\n"
"		return event.event_id, event.value, addr\n"
"	end\n"
"	-- close message\n"
"	C.zmq_msg_close(msg)\n"
"	if event.err ~= nil then\n"
"		-- error parsing monitor event.\n"
"		return nil, ffi.string(event.err)\n"
"	end\n"
"\n"
"  if (-1 == err4) then\n"
"    return nil,error_code__ZMQ_Error__push(err4)\n"
"  end\n"
"  if (-1 == err4) then\n"
"    return nil,error_code__ZMQ_Error__push(err4)\n"
"  end\n"
"  if (-1 == err4) then\n"
"    return nil,error_code__ZMQ_Error__push(err4)\n"
"  end\n"
"  return event_id1, value2, addr3 ~= nil and ffi_string(addr3,addr_len3) or nil\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local hwm_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: hwm\n"
"if (_meth.ZMQ_Socket.hwm) then\n"
"function _meth.ZMQ_Socket.hwm(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_hwm2 = 0\n"
"  rc_lzmq_socket_hwm2 = Cmod.lzmq_socket_hwm(self, hwm_value_tmp)\n"
"  value1 = hwm_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_hwm2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_hwm2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_hwm\n"
"if (_meth.ZMQ_Socket.set_hwm) then\n"
"function _meth.ZMQ_Socket.set_hwm(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_hwm1 = 0\n"
"  rc_lzmq_socket_set_hwm1 = Cmod.lzmq_socket_set_hwm(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_hwm1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_hwm1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local swap_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: swap\n"
"if (_meth.ZMQ_Socket.swap) then\n"
"function _meth.ZMQ_Socket.swap(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_swap2 = 0\n"
"  rc_lzmq_socket_swap2 = Cmod.lzmq_socket_swap(self, swap_value_tmp)\n"
"  value1 = swap_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_swap2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_swap2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_swap\n"
"if (_meth.ZMQ_Socket.set_swap) then\n"
"function _meth.ZMQ_Socket.set_swap(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_swap1 = 0\n"
"  rc_lzmq_socket_set_swap1 = Cmod.lzmq_socket_set_swap(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_swap1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_swap1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local affinity_value_tmp = ffi.new(\"uint64_t[1]\")\n"
"\n"
"-- method: affinity\n"
"if (_meth.ZMQ_Socket.affinity) then\n"
"function _meth.ZMQ_Socket.affinity(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_affinity2 = 0\n"
"  rc_lzmq_socket_affinity2 = Cmod.lzmq_socket_affinity(self, affinity_value_tmp)\n"
"  value1 = affinity_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_affinity2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_affinity2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_affinity\n"
"if (_meth.ZMQ_Socket.set_affinity) then\n"
"function _meth.ZMQ_Socket.set_affinity(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_affinity1 = 0\n"
"  rc_lzmq_socket_set_affinity1 = Cmod.lzmq_socket_set_affinity(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_affinity1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_affinity1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local identity_value_len_tmp = ffi.new(\"size_t[1]\")\n"
"\n"
"-- method: identity\n"
"if (_meth.ZMQ_Socket.identity) then\n"
"function _meth.ZMQ_Socket.identity(self)\n"
"  \n"
"  local value_len1 = 0\n"
"  local value1\n"
"  local rc_lzmq_socket_identity2 = 0\n"
"  rc_lzmq_socket_identity2 = Cmod.lzmq_socket_identity(self, value1, identity_value_len_tmp)\n"
"  value_len1 = identity_value_len_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_identity2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_identity2)\n"
"  end\n"
"  return value1 ~= nil and ffi_string(value1,value_len1) or nil\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_identity\n"
"if (_meth.ZMQ_Socket.set_identity) then\n"
"function _meth.ZMQ_Socket.set_identity(self, value2)\n"
"  \n"
"  local value_len2 = #value2\n"
"  local rc_lzmq_socket_set_identity1 = 0\n"
"  rc_lzmq_socket_set_identity1 = Cmod.lzmq_socket_set_identity(self, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_identity1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_identity1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: subscribe\n"
"if (_meth.ZMQ_Socket.subscribe) then\n"
"function _meth.ZMQ_Socket.subscribe(self, value2)\n"
"  \n"
"  local value_len2 = #value2\n"
"  local rc_lzmq_socket_subscribe1 = 0\n"
"  rc_lzmq_socket_subscribe1 = Cmod.lzmq_socket_subscribe(self, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_subscribe1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_subscribe1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: unsubscribe\n"
"if (_meth.ZMQ_Socket.unsubscribe) then\n"
"function _meth.ZMQ_Socket.unsubscribe(self, value2)\n"
"  \n"
"  local value_len2 = #value2\n"
"  local rc_lzmq_socket_unsubscribe1 = 0\n"
"  rc_lzmq_socket_unsubscribe1 = Cmod.lzmq_socket_unsubscribe(self, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_unsubscribe1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_unsubscribe1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local rate_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: rate\n"
"if (_meth.ZMQ_Socket.rate) then\n"
"function _meth.ZMQ_Socket.rate(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_rate2 = 0\n"
"  rc_lzmq_socket_rate2 = Cmod.lzmq_socket_rate(self, rate_value_tmp)\n"
"  value1 = rate_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_rate2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_rate2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_rate\n"
"if (_meth.ZMQ_Socket.set_rate) then\n"
"function _meth.ZMQ_Socket.set_rate(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_rate1 = 0\n"
"  rc_lzmq_socket_set_rate1 = Cmod.lzmq_socket_set_rate(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_rate1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_rate1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local recovery_ivl_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: recovery_ivl\n"
"if (_meth.ZMQ_Socket.recovery_ivl) then\n"
"function _meth.ZMQ_Socket.recovery_ivl(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_recovery_ivl2 = 0\n"
"  rc_lzmq_socket_recovery_ivl2 = Cmod.lzmq_socket_recovery_ivl(self, recovery_ivl_value_tmp)\n"
"  value1 = recovery_ivl_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_recovery_ivl2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_recovery_ivl2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_recovery_ivl\n"
"if (_meth.ZMQ_Socket.set_recovery_ivl) then\n"
"function _meth.ZMQ_Socket.set_recovery_ivl(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_recovery_ivl1 = 0\n"
"  rc_lzmq_socket_set_recovery_ivl1 = Cmod.lzmq_socket_set_recovery_ivl(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_recovery_ivl1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_recovery_ivl1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local mcast_loop_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: mcast_loop\n"
"if (_meth.ZMQ_Socket.mcast_loop) then\n"
"function _meth.ZMQ_Socket.mcast_loop(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_mcast_loop2 = 0\n"
"  rc_lzmq_socket_mcast_loop2 = Cmod.lzmq_socket_mcast_loop(self, mcast_loop_value_tmp)\n"
"  value1 = mcast_loop_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_mcast_loop2) then\n", /* ----- CUT ----- */
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_mcast_loop2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_mcast_loop\n"
"if (_meth.ZMQ_Socket.set_mcast_loop) then\n"
"function _meth.ZMQ_Socket.set_mcast_loop(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_mcast_loop1 = 0\n"
"  rc_lzmq_socket_set_mcast_loop1 = Cmod.lzmq_socket_set_mcast_loop(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_mcast_loop1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_mcast_loop1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local sndbuf_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: sndbuf\n"
"if (_meth.ZMQ_Socket.sndbuf) then\n"
"function _meth.ZMQ_Socket.sndbuf(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_sndbuf2 = 0\n"
"  rc_lzmq_socket_sndbuf2 = Cmod.lzmq_socket_sndbuf(self, sndbuf_value_tmp)\n"
"  value1 = sndbuf_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_sndbuf2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_sndbuf2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_sndbuf\n"
"if (_meth.ZMQ_Socket.set_sndbuf) then\n"
"function _meth.ZMQ_Socket.set_sndbuf(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_sndbuf1 = 0\n"
"  rc_lzmq_socket_set_sndbuf1 = Cmod.lzmq_socket_set_sndbuf(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_sndbuf1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_sndbuf1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local rcvbuf_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: rcvbuf\n"
"if (_meth.ZMQ_Socket.rcvbuf) then\n"
"function _meth.ZMQ_Socket.rcvbuf(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_rcvbuf2 = 0\n"
"  rc_lzmq_socket_rcvbuf2 = Cmod.lzmq_socket_rcvbuf(self, rcvbuf_value_tmp)\n"
"  value1 = rcvbuf_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_rcvbuf2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_rcvbuf2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_rcvbuf\n"
"if (_meth.ZMQ_Socket.set_rcvbuf) then\n"
"function _meth.ZMQ_Socket.set_rcvbuf(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_rcvbuf1 = 0\n"
"  rc_lzmq_socket_set_rcvbuf1 = Cmod.lzmq_socket_set_rcvbuf(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_rcvbuf1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_rcvbuf1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local rcvmore_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: rcvmore\n"
"if (_meth.ZMQ_Socket.rcvmore) then\n"
"function _meth.ZMQ_Socket.rcvmore(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_rcvmore2 = 0\n"
"  rc_lzmq_socket_rcvmore2 = Cmod.lzmq_socket_rcvmore(self, rcvmore_value_tmp)\n"
"  value1 = rcvmore_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_rcvmore2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_rcvmore2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local fd_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: fd\n"
"if (_meth.ZMQ_Socket.fd) then\n"
"function _meth.ZMQ_Socket.fd(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_fd2 = 0\n"
"  rc_lzmq_socket_fd2 = Cmod.lzmq_socket_fd(self, fd_value_tmp)\n"
"  value1 = fd_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_fd2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_fd2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local events_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: events\n"
"if (_meth.ZMQ_Socket.events) then\n"
"function _meth.ZMQ_Socket.events(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_events2 = 0\n"
"  rc_lzmq_socket_events2 = Cmod.lzmq_socket_events(self, events_value_tmp)\n"
"  value1 = events_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_events2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_events2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local type_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: type\n"
"if (_meth.ZMQ_Socket.type) then\n"
"function _meth.ZMQ_Socket.type(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_type2 = 0\n"
"  rc_lzmq_socket_type2 = Cmod.lzmq_socket_type(self, type_value_tmp)\n"
"  value1 = type_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_type2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_type2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local linger_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: linger\n"
"if (_meth.ZMQ_Socket.linger) then\n"
"function _meth.ZMQ_Socket.linger(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_linger2 = 0\n"
"  rc_lzmq_socket_linger2 = Cmod.lzmq_socket_linger(self, linger_value_tmp)\n"
"  value1 = linger_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_linger2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_linger2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_linger\n"
"if (_meth.ZMQ_Socket.set_linger) then\n"
"function _meth.ZMQ_Socket.set_linger(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_linger1 = 0\n"
"  rc_lzmq_socket_set_linger1 = Cmod.lzmq_socket_set_linger(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_linger1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_linger1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local reconnect_ivl_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: reconnect_ivl\n"
"if (_meth.ZMQ_Socket.reconnect_ivl) then\n"
"function _meth.ZMQ_Socket.reconnect_ivl(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_reconnect_ivl2 = 0\n"
"  rc_lzmq_socket_reconnect_ivl2 = Cmod.lzmq_socket_reconnect_ivl(self, reconnect_ivl_value_tmp)\n"
"  value1 = reconnect_ivl_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_reconnect_ivl2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_reconnect_ivl2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_reconnect_ivl\n"
"if (_meth.ZMQ_Socket.set_reconnect_ivl) then\n"
"function _meth.ZMQ_Socket.set_reconnect_ivl(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_reconnect_ivl1 = 0\n"
"  rc_lzmq_socket_set_reconnect_ivl1 = Cmod.lzmq_socket_set_reconnect_ivl(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_reconnect_ivl1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_reconnect_ivl1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local backlog_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: backlog\n"
"if (_meth.ZMQ_Socket.backlog) then\n"
"function _meth.ZMQ_Socket.backlog(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_backlog2 = 0\n"
"  rc_lzmq_socket_backlog2 = Cmod.lzmq_socket_backlog(self, backlog_value_tmp)\n"
"  value1 = backlog_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_backlog2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_backlog2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_backlog\n"
"if (_meth.ZMQ_Socket.set_backlog) then\n"
"function _meth.ZMQ_Socket.set_backlog(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_backlog1 = 0\n"
"  rc_lzmq_socket_set_backlog1 = Cmod.lzmq_socket_set_backlog(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_backlog1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_backlog1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local recovery_ivl_msec_value_tmp = ffi.new(\"int64_t[1]\")\n"
"\n"
"-- method: recovery_ivl_msec\n"
"if (_meth.ZMQ_Socket.recovery_ivl_msec) then\n"
"function _meth.ZMQ_Socket.recovery_ivl_msec(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_recovery_ivl_msec2 = 0\n"
"  rc_lzmq_socket_recovery_ivl_msec2 = Cmod.lzmq_socket_recovery_ivl_msec(self, recovery_ivl_msec_value_tmp)\n"
"  value1 = recovery_ivl_msec_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_recovery_ivl_msec2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_recovery_ivl_msec2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_recovery_ivl_msec\n"
"if (_meth.ZMQ_Socket.set_recovery_ivl_msec) then\n"
"function _meth.ZMQ_Socket.set_recovery_ivl_msec(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_recovery_ivl_msec1 = 0\n"
"  rc_lzmq_socket_set_recovery_ivl_msec1 = Cmod.lzmq_socket_set_recovery_ivl_msec(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_recovery_ivl_msec1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_recovery_ivl_msec1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local reconnect_ivl_max_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: reconnect_ivl_max\n"
"if (_meth.ZMQ_Socket.reconnect_ivl_max) then\n"
"function _meth.ZMQ_Socket.reconnect_ivl_max(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_reconnect_ivl_max2 = 0\n"
"  rc_lzmq_socket_reconnect_ivl_max2 = Cmod.lzmq_socket_reconnect_ivl_max(self, reconnect_ivl_max_value_tmp)\n"
"  value1 = reconnect_ivl_max_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_reconnect_ivl_max2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_reconnect_ivl_max2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_reconnect_ivl_max\n"
"if (_meth.ZMQ_Socket.set_reconnect_ivl_max) then\n"
"function _meth.ZMQ_Socket.set_reconnect_ivl_max(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_reconnect_ivl_max1 = 0\n"
"  rc_lzmq_socket_set_reconnect_ivl_max1 = Cmod.lzmq_socket_set_reconnect_ivl_max(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_reconnect_ivl_max1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_reconnect_ivl_max1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local maxmsgsize_value_tmp = ffi.new(\"int64_t[1]\")\n"
"\n"
"-- method: maxmsgsize\n"
"if (_meth.ZMQ_Socket.maxmsgsize) then\n"
"function _meth.ZMQ_Socket.maxmsgsize(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_maxmsgsize2 = 0\n"
"  rc_lzmq_socket_maxmsgsize2 = Cmod.lzmq_socket_maxmsgsize(self, maxmsgsize_value_tmp)\n"
"  value1 = maxmsgsize_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_maxmsgsize2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_maxmsgsize2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_maxmsgsize\n"
"if (_meth.ZMQ_Socket.set_maxmsgsize) then\n"
"function _meth.ZMQ_Socket.set_maxmsgsize(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_maxmsgsize1 = 0\n"
"  rc_lzmq_socket_set_maxmsgsize1 = Cmod.lzmq_socket_set_maxmsgsize(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_maxmsgsize1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_maxmsgsize1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local sndhwm_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: sndhwm\n"
"if (_meth.ZMQ_Socket.sndhwm) then\n"
"function _meth.ZMQ_Socket.sndhwm(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_sndhwm2 = 0\n"
"  rc_lzmq_socket_sndhwm2 = Cmod.lzmq_socket_sndhwm(self, sndhwm_value_tmp)\n"
"  value1 = sndhwm_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_sndhwm2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_sndhwm2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_sndhwm\n"
"if (_meth.ZMQ_Socket.set_sndhwm) then\n"
"function _meth.ZMQ_Socket.set_sndhwm(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_sndhwm1 = 0\n"
"  rc_lzmq_socket_set_sndhwm1 = Cmod.lzmq_socket_set_sndhwm(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_sndhwm1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_sndhwm1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local rcvhwm_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: rcvhwm\n"
"if (_meth.ZMQ_Socket.rcvhwm) then\n"
"function _meth.ZMQ_Socket.rcvhwm(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_rcvhwm2 = 0\n"
"  rc_lzmq_socket_rcvhwm2 = Cmod.lzmq_socket_rcvhwm(self, rcvhwm_value_tmp)\n"
"  value1 = rcvhwm_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_rcvhwm2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_rcvhwm2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_rcvhwm\n"
"if (_meth.ZMQ_Socket.set_rcvhwm) then\n"
"function _meth.ZMQ_Socket.set_rcvhwm(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_rcvhwm1 = 0\n"
"  rc_lzmq_socket_set_rcvhwm1 = Cmod.lzmq_socket_set_rcvhwm(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_rcvhwm1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_rcvhwm1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local multicast_hops_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: multicast_hops\n"
"if (_meth.ZMQ_Socket.multicast_hops) then\n"
"function _meth.ZMQ_Socket.multicast_hops(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_multicast_hops2 = 0\n"
"  rc_lzmq_socket_multicast_hops2 = Cmod.lzmq_socket_multicast_hops(self, multicast_hops_value_tmp)\n"
"  value1 = multicast_hops_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_multicast_hops2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_multicast_hops2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_multicast_hops\n"
"if (_meth.ZMQ_Socket.set_multicast_hops) then\n"
"function _meth.ZMQ_Socket.set_multicast_hops(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_multicast_hops1 = 0\n"
"  rc_lzmq_socket_set_multicast_hops1 = Cmod.lzmq_socket_set_multicast_hops(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_multicast_hops1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_multicast_hops1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local rcvtimeo_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: rcvtimeo\n"
"if (_meth.ZMQ_Socket.rcvtimeo) then\n"
"function _meth.ZMQ_Socket.rcvtimeo(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_rcvtimeo2 = 0\n"
"  rc_lzmq_socket_rcvtimeo2 = Cmod.lzmq_socket_rcvtimeo(self, rcvtimeo_value_tmp)\n"
"  value1 = rcvtimeo_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_rcvtimeo2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_rcvtimeo2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_rcvtimeo\n"
"if (_meth.ZMQ_Socket.set_rcvtimeo) then\n"
"function _meth.ZMQ_Socket.set_rcvtimeo(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_rcvtimeo1 = 0\n"
"  rc_lzmq_socket_set_rcvtimeo1 = Cmod.lzmq_socket_set_rcvtimeo(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_rcvtimeo1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_rcvtimeo1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local sndtimeo_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: sndtimeo\n"
"if (_meth.ZMQ_Socket.sndtimeo) then\n"
"function _meth.ZMQ_Socket.sndtimeo(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_sndtimeo2 = 0\n"
"  rc_lzmq_socket_sndtimeo2 = Cmod.lzmq_socket_sndtimeo(self, sndtimeo_value_tmp)\n"
"  value1 = sndtimeo_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_sndtimeo2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_sndtimeo2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_sndtimeo\n"
"if (_meth.ZMQ_Socket.set_sndtimeo) then\n"
"function _meth.ZMQ_Socket.set_sndtimeo(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_sndtimeo1 = 0\n"
"  rc_lzmq_socket_set_sndtimeo1 = Cmod.lzmq_socket_set_sndtimeo(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_sndtimeo1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_sndtimeo1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local ipv4only_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: ipv4only\n"
"if (_meth.ZMQ_Socket.ipv4only) then\n"
"function _meth.ZMQ_Socket.ipv4only(self)\n"
"  \n"
"  local value1\n", /* ----- CUT ----- */
"  local rc_lzmq_socket_ipv4only2 = 0\n"
"  rc_lzmq_socket_ipv4only2 = Cmod.lzmq_socket_ipv4only(self, ipv4only_value_tmp)\n"
"  value1 = ipv4only_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_ipv4only2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_ipv4only2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_ipv4only\n"
"if (_meth.ZMQ_Socket.set_ipv4only) then\n"
"function _meth.ZMQ_Socket.set_ipv4only(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_ipv4only1 = 0\n"
"  rc_lzmq_socket_set_ipv4only1 = Cmod.lzmq_socket_set_ipv4only(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_ipv4only1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_ipv4only1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local last_endpoint_value_len_tmp = ffi.new(\"size_t[1]\")\n"
"\n"
"-- method: last_endpoint\n"
"if (_meth.ZMQ_Socket.last_endpoint) then\n"
"function _meth.ZMQ_Socket.last_endpoint(self)\n"
"  \n"
"  local value_len1 = 0\n"
"  local value1\n"
"  local rc_lzmq_socket_last_endpoint2 = 0\n"
"  rc_lzmq_socket_last_endpoint2 = Cmod.lzmq_socket_last_endpoint(self, value1, last_endpoint_value_len_tmp)\n"
"  value_len1 = last_endpoint_value_len_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_last_endpoint2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_last_endpoint2)\n"
"  end\n"
"  return value1 ~= nil and ffi_string(value1,value_len1) or nil\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: router_mandatory\n"
"if (_meth.ZMQ_Socket.router_mandatory) then\n"
"function _meth.ZMQ_Socket.router_mandatory(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_router_mandatory1 = 0\n"
"  rc_lzmq_socket_router_mandatory1 = Cmod.lzmq_socket_router_mandatory(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_router_mandatory1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_router_mandatory1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local tcp_keepalive_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: tcp_keepalive\n"
"if (_meth.ZMQ_Socket.tcp_keepalive) then\n"
"function _meth.ZMQ_Socket.tcp_keepalive(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_tcp_keepalive2 = 0\n"
"  rc_lzmq_socket_tcp_keepalive2 = Cmod.lzmq_socket_tcp_keepalive(self, tcp_keepalive_value_tmp)\n"
"  value1 = tcp_keepalive_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_tcp_keepalive2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_tcp_keepalive2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_tcp_keepalive\n"
"if (_meth.ZMQ_Socket.set_tcp_keepalive) then\n"
"function _meth.ZMQ_Socket.set_tcp_keepalive(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_tcp_keepalive1 = 0\n"
"  rc_lzmq_socket_set_tcp_keepalive1 = Cmod.lzmq_socket_set_tcp_keepalive(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_tcp_keepalive1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_tcp_keepalive1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local tcp_keepalive_cnt_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: tcp_keepalive_cnt\n"
"if (_meth.ZMQ_Socket.tcp_keepalive_cnt) then\n"
"function _meth.ZMQ_Socket.tcp_keepalive_cnt(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_tcp_keepalive_cnt2 = 0\n"
"  rc_lzmq_socket_tcp_keepalive_cnt2 = Cmod.lzmq_socket_tcp_keepalive_cnt(self, tcp_keepalive_cnt_value_tmp)\n"
"  value1 = tcp_keepalive_cnt_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_tcp_keepalive_cnt2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_tcp_keepalive_cnt2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_tcp_keepalive_cnt\n"
"if (_meth.ZMQ_Socket.set_tcp_keepalive_cnt) then\n"
"function _meth.ZMQ_Socket.set_tcp_keepalive_cnt(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_tcp_keepalive_cnt1 = 0\n"
"  rc_lzmq_socket_set_tcp_keepalive_cnt1 = Cmod.lzmq_socket_set_tcp_keepalive_cnt(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_tcp_keepalive_cnt1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_tcp_keepalive_cnt1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local tcp_keepalive_idle_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: tcp_keepalive_idle\n"
"if (_meth.ZMQ_Socket.tcp_keepalive_idle) then\n"
"function _meth.ZMQ_Socket.tcp_keepalive_idle(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_tcp_keepalive_idle2 = 0\n"
"  rc_lzmq_socket_tcp_keepalive_idle2 = Cmod.lzmq_socket_tcp_keepalive_idle(self, tcp_keepalive_idle_value_tmp)\n"
"  value1 = tcp_keepalive_idle_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_tcp_keepalive_idle2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_tcp_keepalive_idle2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_tcp_keepalive_idle\n"
"if (_meth.ZMQ_Socket.set_tcp_keepalive_idle) then\n"
"function _meth.ZMQ_Socket.set_tcp_keepalive_idle(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_tcp_keepalive_idle1 = 0\n"
"  rc_lzmq_socket_set_tcp_keepalive_idle1 = Cmod.lzmq_socket_set_tcp_keepalive_idle(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_tcp_keepalive_idle1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_tcp_keepalive_idle1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local tcp_keepalive_intvl_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: tcp_keepalive_intvl\n"
"if (_meth.ZMQ_Socket.tcp_keepalive_intvl) then\n"
"function _meth.ZMQ_Socket.tcp_keepalive_intvl(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_tcp_keepalive_intvl2 = 0\n"
"  rc_lzmq_socket_tcp_keepalive_intvl2 = Cmod.lzmq_socket_tcp_keepalive_intvl(self, tcp_keepalive_intvl_value_tmp)\n"
"  value1 = tcp_keepalive_intvl_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_tcp_keepalive_intvl2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_tcp_keepalive_intvl2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_tcp_keepalive_intvl\n"
"if (_meth.ZMQ_Socket.set_tcp_keepalive_intvl) then\n"
"function _meth.ZMQ_Socket.set_tcp_keepalive_intvl(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_tcp_keepalive_intvl1 = 0\n"
"  rc_lzmq_socket_set_tcp_keepalive_intvl1 = Cmod.lzmq_socket_set_tcp_keepalive_intvl(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_tcp_keepalive_intvl1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_tcp_keepalive_intvl1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: tcp_accept_filter\n"
"if (_meth.ZMQ_Socket.tcp_accept_filter) then\n"
"function _meth.ZMQ_Socket.tcp_accept_filter(self, value2)\n"
"  \n"
"  local value_len2 = #value2\n"
"  local rc_lzmq_socket_tcp_accept_filter1 = 0\n"
"  rc_lzmq_socket_tcp_accept_filter1 = Cmod.lzmq_socket_tcp_accept_filter(self, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_tcp_accept_filter1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_tcp_accept_filter1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local immediate_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: immediate\n"
"if (_meth.ZMQ_Socket.immediate) then\n"
"function _meth.ZMQ_Socket.immediate(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_immediate2 = 0\n"
"  rc_lzmq_socket_immediate2 = Cmod.lzmq_socket_immediate(self, immediate_value_tmp)\n"
"  value1 = immediate_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_immediate2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_immediate2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_immediate\n"
"if (_meth.ZMQ_Socket.set_immediate) then\n"
"function _meth.ZMQ_Socket.set_immediate(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_immediate1 = 0\n"
"  rc_lzmq_socket_set_immediate1 = Cmod.lzmq_socket_set_immediate(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_immediate1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_immediate1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: xpub_verbose\n"
"if (_meth.ZMQ_Socket.xpub_verbose) then\n"
"function _meth.ZMQ_Socket.xpub_verbose(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_xpub_verbose1 = 0\n"
"  rc_lzmq_socket_xpub_verbose1 = Cmod.lzmq_socket_xpub_verbose(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_xpub_verbose1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_xpub_verbose1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: router_raw\n"
"if (_meth.ZMQ_Socket.router_raw) then\n"
"function _meth.ZMQ_Socket.router_raw(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_router_raw1 = 0\n"
"  rc_lzmq_socket_router_raw1 = Cmod.lzmq_socket_router_raw(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_router_raw1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_router_raw1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local ipv6_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: ipv6\n"
"if (_meth.ZMQ_Socket.ipv6) then\n"
"function _meth.ZMQ_Socket.ipv6(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_ipv62 = 0\n"
"  rc_lzmq_socket_ipv62 = Cmod.lzmq_socket_ipv6(self, ipv6_value_tmp)\n"
"  value1 = ipv6_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_ipv62) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_ipv62)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_ipv6\n"
"if (_meth.ZMQ_Socket.set_ipv6) then\n"
"function _meth.ZMQ_Socket.set_ipv6(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_ipv61 = 0\n"
"  rc_lzmq_socket_set_ipv61 = Cmod.lzmq_socket_set_ipv6(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_ipv61) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_ipv61)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local mechanism_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: mechanism\n"
"if (_meth.ZMQ_Socket.mechanism) then\n"
"function _meth.ZMQ_Socket.mechanism(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_mechanism2 = 0\n"
"  rc_lzmq_socket_mechanism2 = Cmod.lzmq_socket_mechanism(self, mechanism_value_tmp)\n"
"  value1 = mechanism_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_mechanism2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_mechanism2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local plain_server_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: plain_server\n"
"if (_meth.ZMQ_Socket.plain_server) then\n"
"function _meth.ZMQ_Socket.plain_server(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_plain_server2 = 0\n"
"  rc_lzmq_socket_plain_server2 = Cmod.lzmq_socket_plain_server(self, plain_server_value_tmp)\n"
"  value1 = plain_server_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_plain_server2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_plain_server2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_plain_server\n"
"if (_meth.ZMQ_Socket.set_plain_server) then\n"
"function _meth.ZMQ_Socket.set_plain_server(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_plain_server1 = 0\n"
"  rc_lzmq_socket_set_plain_server1 = Cmod.lzmq_socket_set_plain_server(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_plain_server1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_plain_server1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local plain_username_value_len_tmp = ffi.new(\"size_t[1]\")\n"
"\n"
"-- method: plain_username\n"
"if (_meth.ZMQ_Socket.plain_username) then\n"
"function _meth.ZMQ_Socket.plain_username(self)\n"
"  \n"
"  local value_len1 = 0\n"
"  local value1\n"
"  local rc_lzmq_socket_plain_username2 = 0\n"
"  rc_lzmq_socket_plain_username2 = Cmod.lzmq_socket_plain_username(self, value1, plain_username_value_len_tmp)\n"
"  value_len1 = plain_username_value_len_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_plain_username2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_plain_username2)\n"
"  end\n"
"  return value1 ~= nil and ffi_string(value1,value_len1) or nil\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_plain_username\n"
"if (_meth.ZMQ_Socket.set_plain_username) then\n"
"function _meth.ZMQ_Socket.set_plain_username(self, value2)\n"
"  \n"
"  local value_len2 = #value2\n"
"  local rc_lzmq_socket_set_plain_username1 = 0\n"
"  rc_lzmq_socket_set_plain_username1 = Cmod.lzmq_socket_set_plain_username(self, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_plain_username1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_plain_username1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local plain_password_value_len_tmp = ffi.new(\"size_t[1]\")\n"
"\n"
"-- method: plain_password\n"
"if (_meth.ZMQ_Socket.plain_password) then\n"
"function _meth.ZMQ_Socket.plain_password(self)\n"
"  \n"
"  local value_len1 = 0\n"
"  local value1\n"
"  local rc_lzmq_socket_plain_password2 = 0\n"
"  rc_lzmq_socket_plain_password2 = Cmod.lzmq_socket_plain_password(self, value1, plain_password_value_len_tmp)\n"
"  value_len1 = plain_password_value_len_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_plain_password2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_plain_password2)\n"
"  end\n"
"  return value1 ~= nil and ffi_string(value1,value_len1) or nil\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_plain_password\n"
"if (_meth.ZMQ_Socket.set_plain_password) then\n"
"function _meth.ZMQ_Socket.set_plain_password(self, value2)\n"
"  \n"
"  local value_len2 = #value2\n"
"  local rc_lzmq_socket_set_plain_password1 = 0\n"
"  rc_lzmq_socket_set_plain_password1 = Cmod.lzmq_socket_set_plain_password(self, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_plain_password1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_plain_password1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local curve_server_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: curve_server\n"
"if (_meth.ZMQ_Socket.curve_server) then\n"
"function _meth.ZMQ_Socket.curve_server(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_curve_server2 = 0\n"
"  rc_lzmq_socket_curve_server2 = Cmod.lzmq_socket_curve_server(self, curve_server_value_tmp)\n"
"  value1 = curve_server_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_curve_server2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_curve_server2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_curve_server\n"
"if (_meth.ZMQ_Socket.set_curve_server) then\n"
"function _meth.ZMQ_Socket.set_curve_server(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_curve_server1 = 0\n"
"  rc_lzmq_socket_set_curve_server1 = Cmod.lzmq_socket_set_curve_server(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_curve_server1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_curve_server1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local curve_publickey_value_len_tmp = ffi.new(\"size_t[1]\")\n"
"\n"
"-- method: curve_publickey\n"
"if (_meth.ZMQ_Socket.curve_publickey) then\n"
"function _meth.ZMQ_Socket.curve_publickey(self)\n"
"  \n"
"  local value_len1 = 0\n"
"  local value1\n"
"  local rc_lzmq_socket_curve_publickey2 = 0\n"
"  rc_lzmq_socket_curve_publickey2 = Cmod.lzmq_socket_curve_publickey(self, value1, curve_publickey_value_len_tmp)\n"
"  value_len1 = curve_publickey_value_len_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_curve_publickey2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_curve_publickey2)\n"
"  end\n"
"  return value1 ~= nil and ffi_string(value1,value_len1) or nil\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_curve_publickey\n"
"if (_meth.ZMQ_Socket.set_curve_publickey) then\n"
"function _meth.ZMQ_Socket.set_curve_publickey(self, value2)\n"
"  \n"
"  local value_len2 = #value2\n", /* ----- CUT ----- */
"  local rc_lzmq_socket_set_curve_publickey1 = 0\n"
"  rc_lzmq_socket_set_curve_publickey1 = Cmod.lzmq_socket_set_curve_publickey(self, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_curve_publickey1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_curve_publickey1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local curve_secretkey_value_len_tmp = ffi.new(\"size_t[1]\")\n"
"\n"
"-- method: curve_secretkey\n"
"if (_meth.ZMQ_Socket.curve_secretkey) then\n"
"function _meth.ZMQ_Socket.curve_secretkey(self)\n"
"  \n"
"  local value_len1 = 0\n"
"  local value1\n"
"  local rc_lzmq_socket_curve_secretkey2 = 0\n"
"  rc_lzmq_socket_curve_secretkey2 = Cmod.lzmq_socket_curve_secretkey(self, value1, curve_secretkey_value_len_tmp)\n"
"  value_len1 = curve_secretkey_value_len_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_curve_secretkey2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_curve_secretkey2)\n"
"  end\n"
"  return value1 ~= nil and ffi_string(value1,value_len1) or nil\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_curve_secretkey\n"
"if (_meth.ZMQ_Socket.set_curve_secretkey) then\n"
"function _meth.ZMQ_Socket.set_curve_secretkey(self, value2)\n"
"  \n"
"  local value_len2 = #value2\n"
"  local rc_lzmq_socket_set_curve_secretkey1 = 0\n"
"  rc_lzmq_socket_set_curve_secretkey1 = Cmod.lzmq_socket_set_curve_secretkey(self, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_curve_secretkey1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_curve_secretkey1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local curve_serverkey_value_len_tmp = ffi.new(\"size_t[1]\")\n"
"\n"
"-- method: curve_serverkey\n"
"if (_meth.ZMQ_Socket.curve_serverkey) then\n"
"function _meth.ZMQ_Socket.curve_serverkey(self)\n"
"  \n"
"  local value_len1 = 0\n"
"  local value1\n"
"  local rc_lzmq_socket_curve_serverkey2 = 0\n"
"  rc_lzmq_socket_curve_serverkey2 = Cmod.lzmq_socket_curve_serverkey(self, value1, curve_serverkey_value_len_tmp)\n"
"  value_len1 = curve_serverkey_value_len_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_curve_serverkey2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_curve_serverkey2)\n"
"  end\n"
"  return value1 ~= nil and ffi_string(value1,value_len1) or nil\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_curve_serverkey\n"
"if (_meth.ZMQ_Socket.set_curve_serverkey) then\n"
"function _meth.ZMQ_Socket.set_curve_serverkey(self, value2)\n"
"  \n"
"  local value_len2 = #value2\n"
"  local rc_lzmq_socket_set_curve_serverkey1 = 0\n"
"  rc_lzmq_socket_set_curve_serverkey1 = Cmod.lzmq_socket_set_curve_serverkey(self, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_curve_serverkey1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_curve_serverkey1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: probe_router\n"
"if (_meth.ZMQ_Socket.probe_router) then\n"
"function _meth.ZMQ_Socket.probe_router(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_probe_router1 = 0\n"
"  rc_lzmq_socket_probe_router1 = Cmod.lzmq_socket_probe_router(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_probe_router1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_probe_router1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: req_correlate\n"
"if (_meth.ZMQ_Socket.req_correlate) then\n"
"function _meth.ZMQ_Socket.req_correlate(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_req_correlate1 = 0\n"
"  rc_lzmq_socket_req_correlate1 = Cmod.lzmq_socket_req_correlate(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_req_correlate1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_req_correlate1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: req_relaxed\n"
"if (_meth.ZMQ_Socket.req_relaxed) then\n"
"function _meth.ZMQ_Socket.req_relaxed(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_req_relaxed1 = 0\n"
"  rc_lzmq_socket_req_relaxed1 = Cmod.lzmq_socket_req_relaxed(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_req_relaxed1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_req_relaxed1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local conflate_value_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: conflate\n"
"if (_meth.ZMQ_Socket.conflate) then\n"
"function _meth.ZMQ_Socket.conflate(self)\n"
"  \n"
"  local value1\n"
"  local rc_lzmq_socket_conflate2 = 0\n"
"  rc_lzmq_socket_conflate2 = Cmod.lzmq_socket_conflate(self, conflate_value_tmp)\n"
"  value1 = conflate_value_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_conflate2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_conflate2)\n"
"  end\n"
"  return value1\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_conflate\n"
"if (_meth.ZMQ_Socket.set_conflate) then\n"
"function _meth.ZMQ_Socket.set_conflate(self, value2)\n"
"  \n"
"  \n"
"  local rc_lzmq_socket_set_conflate1 = 0\n"
"  rc_lzmq_socket_set_conflate1 = Cmod.lzmq_socket_set_conflate(self, value2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_conflate1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_conflate1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"do\n"
"  local zap_domain_value_len_tmp = ffi.new(\"size_t[1]\")\n"
"\n"
"-- method: zap_domain\n"
"if (_meth.ZMQ_Socket.zap_domain) then\n"
"function _meth.ZMQ_Socket.zap_domain(self)\n"
"  \n"
"  local value_len1 = 0\n"
"  local value1\n"
"  local rc_lzmq_socket_zap_domain2 = 0\n"
"  rc_lzmq_socket_zap_domain2 = Cmod.lzmq_socket_zap_domain(self, value1, zap_domain_value_len_tmp)\n"
"  value_len1 = zap_domain_value_len_tmp[0]\n"
"  if (-1 == rc_lzmq_socket_zap_domain2) then\n"
"    return nil,error_code__ZMQ_Error__push(rc_lzmq_socket_zap_domain2)\n"
"  end\n"
"  return value1 ~= nil and ffi_string(value1,value_len1) or nil\n"
"end\n"
"end\n"
"end\n"
"\n"
"-- method: set_zap_domain\n"
"if (_meth.ZMQ_Socket.set_zap_domain) then\n"
"function _meth.ZMQ_Socket.set_zap_domain(self, value2)\n"
"  \n"
"  local value_len2 = #value2\n"
"  local rc_lzmq_socket_set_zap_domain1 = 0\n"
"  rc_lzmq_socket_set_zap_domain1 = Cmod.lzmq_socket_set_zap_domain(self, value2, value_len2)\n"
"  -- check for error.\n"
"  if (-1 == rc_lzmq_socket_set_zap_domain1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_lzmq_socket_set_zap_domain1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"_push.ZMQ_Socket = obj_type_ZMQ_Socket_push\n"
"ffi.metatype(\"ZMQ_Socket\", _priv.ZMQ_Socket)\n"
"-- End \"ZMQ_Socket\" FFI interface\n"
"\n"
"\n"
"-- Start \"ZMQ_Poller\" FFI interface\n"
"-- method: new\n"
"function _pub.ZMQ_Poller.new(length1)\n"
"  length1 = length1 or 10\n"
"  local self = ffi.new(\"ZMQ_Poller\")\n"
"  Cmod.poller_init(self, length1)\n"
"  return obj_type_ZMQ_Poller_push(self)\n"
"end\n"
"register_default_constructor(_pub,\"ZMQ_Poller\",_pub.ZMQ_Poller.new)\n"
"\n"
"-- method: close\n"
"function _meth.ZMQ_Poller.close(self)\n"
"  local self = obj_type_ZMQ_Poller_delete(self)\n"
"  if not self then return end\n"
"  Cmod.poller_cleanup(self)\n"
"  return \n"
"end\n"
"_priv.ZMQ_Poller.__gc = _meth.ZMQ_Poller.close\n"
"\n"
"-- method: add\n"
"function _meth.ZMQ_Poller.add(self, sock2, events3)\n"
"  \n"
"  \n"
"  local idx1 = 0\n"
"	local fd = 0\n"
"	local sock_type = type(sock2)\n"
"	local sock\n"
"	if sock_type == 'cdata' then\n"
"		sock = obj_type_ZMQ_Socket_check(sock2)\n"
"	elseif sock_type == 'number' then\n"
"		fd = sock2\n"
"	else\n"
"		error(\"expected number or ZMQ_Socket\")\n"
"	end\n"
"	idx1 = Cmod.poller_get_free_item(self)\n"
"	local item = self.items[idx1]\n"
"	item.socket = sock\n"
"	item.fd = fd\n"
"	item.events = events3\n"
"\n"
"  return idx1\n"
"end\n"
"\n"
"-- method: modify\n"
"function _meth.ZMQ_Poller.modify(self, sock2, events3)\n"
"  \n"
"  \n"
"  local idx1 = 0\n"
"	local fd = 0\n"
"	local sock_type = type(sock2)\n"
"	local sock\n"
"	if sock_type == 'cdata' then\n"
"		sock = obj_type_ZMQ_Socket_check(sock2)\n"
"		-- find sock in items list.\n"
"		idx1 = Cmod.poller_find_sock_item(self, sock)\n"
"	elseif sock_type == 'number' then\n"
"		fd = sock2\n"
"		-- find fd in items list.\n"
"		idx1 = Cmod.poller_find_fd_item(self, fd);\n"
"	else\n"
"		error(\"expected number or ZMQ_Socket\")\n"
"	end\n"
"	if events3 ~= 0 then\n"
"		local item = self.items[idx1]\n"
"		item.socket = sock\n"
"		item.fd = fd\n"
"		item.events = events3\n"
"	else\n"
"		Cmod.poller_remove_item(self, idx1)\n"
"	end\n"
"\n"
"  return idx1\n"
"end\n"
"\n"
"-- method: remove\n"
"function _meth.ZMQ_Poller.remove(self, sock2)\n"
"  \n"
"  local idx1 = 0\n"
"	local fd = 0\n"
"	local sock_type = type(sock2)\n"
"	local sock\n"
"	if sock_type == 'cdata' then\n"
"		sock = obj_type_ZMQ_Socket_check(sock2)\n"
"		-- find sock in items list.\n"
"		idx1 = Cmod.poller_find_sock_item(self, sock)\n"
"	elseif sock_type == 'number' then\n"
"		fd = sock2\n"
"		-- find fd in items list.\n"
"		idx1 = Cmod.poller_find_fd_item(self, fd);\n"
"	else\n"
"		error(\"expected number or ZMQ_Socket\")\n"
"	end\n"
"	if idx1 >= 0 then\n"
"		Cmod.poller_remove_item(self, idx1)\n"
"	end\n"
"\n"
"  return idx1\n"
"end\n"
"\n"
"-- method: poll\n"
"function _meth.ZMQ_Poller.poll(self, timeout2)\n"
"  \n"
"  \n"
"  local count1 = 0\n"
"  local err2 = 0\n"
"  err2 = Cmod.poller_poll(self, timeout2)\n"
"	if(err2 > 0) then\n"
"		self.next = 0\n"
"		count1 = err2\n"
"	else\n"
"		self.next = -1\n"
"		count1 = 0\n"
"	end\n"
"\n"
"  if (-1 == err2) then\n"
"    return nil,error_code__ZMQ_Error__push(err2)\n"
"  end\n"
"  return count1\n"
"end\n"
"\n"
"do\n"
"  local next_revents_idx_revents_tmp = ffi.new(\"int[1]\")\n"
"\n"
"-- method: next_revents_idx\n"
"function _meth.ZMQ_Poller.next_revents_idx(self)\n"
"  \n"
"  local idx1 = 0\n"
"  local revents2\n"
"  idx1 = Cmod.poller_next_revents(self, next_revents_idx_revents_tmp)\n"
"  revents2 = next_revents_idx_revents_tmp[0]\n"
"  return idx1, revents2\n"
"end\n"
"end\n"
"\n"
"-- method: count\n"
"function _meth.ZMQ_Poller.count(self)\n"
"  \n"
"  local count1 = 0\n"
"	count1 = self.count;\n"
"\n"
"  return count1\n"
"end\n"
"\n"
"_push.ZMQ_Poller = obj_type_ZMQ_Poller_push\n"
"ffi.metatype(\"ZMQ_Poller\", _priv.ZMQ_Poller)\n"
"-- End \"ZMQ_Poller\" FFI interface\n"
"\n"
"\n"
"-- Start \"ZMQ_Ctx\" FFI interface\n"
"-- method: term\n"
"function _meth.ZMQ_Ctx.term(self)\n"
"  local self,this_flags1 = obj_type_ZMQ_Ctx_delete(self)\n"
"  if not self then return end\n"
"  local rc_zmq_term1 = 0\n"
"  rc_zmq_term1 = C.zmq_term(self)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_term1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_term1)\n"
"  end\n"
"  return true\n"
"end\n"
"_priv.ZMQ_Ctx.__gc = _meth.ZMQ_Ctx.term\n"
"\n"
"-- method: socket\n"
"function _meth.ZMQ_Ctx.socket(self, type2)\n"
"  \n"
"  \n"
"  local rc_zmq_socket_flags1 = OBJ_UDATA_FLAG_OWN\n"
"  local rc_zmq_socket1\n"
"  rc_zmq_socket1 = C.zmq_socket(self, type2)\n"
"  if (nil == rc_zmq_socket1) then\n"
"    return nil, get_zmq_strerror()\n"
"  end\n"
"  return obj_type_ZMQ_Socket_push(rc_zmq_socket1, rc_zmq_socket_flags1)\n"
"end\n"
"\n"
"-- method: set\n"
"if (_meth.ZMQ_Ctx.set) then\n"
"function _meth.ZMQ_Ctx.set(self, flag2, value3)\n"
"  \n"
"  \n"
"  \n"
"  local rc_zmq_ctx_set1 = 0\n"
"  rc_zmq_ctx_set1 = C.zmq_ctx_set(self, flag2, value3)\n"
"  return rc_zmq_ctx_set1\n"
"end\n"
"end\n"
"\n"
"-- method: get\n"
"if (_meth.ZMQ_Ctx.get) then\n"
"function _meth.ZMQ_Ctx.get(self, flag2)\n"
"  \n"
"  \n"
"  local rc_zmq_ctx_get1 = 0\n"
"  rc_zmq_ctx_get1 = C.zmq_ctx_get(self, flag2)\n"
"  return rc_zmq_ctx_get1\n"
"end\n"
"end\n"
"\n"
"_push.ZMQ_Ctx = obj_type_ZMQ_Ctx_push\n"
"ffi.metatype(\"ZMQ_Ctx\", _priv.ZMQ_Ctx)\n"
"-- End \"ZMQ_Ctx\" FFI interface\n"
"\n"
"\n"
"-- Start \"ZMQ_StopWatch\" FFI interface\n"
"-- method: start\n"
"function _pub.ZMQ_StopWatch.start()\n"
"  local this_flags1 = OBJ_UDATA_FLAG_OWN\n"
"  local self\n"
"  self = C.zmq_stopwatch_start()\n"
"  return obj_type_ZMQ_StopWatch_push(self, this_flags1)\n"
"end\n"
"register_default_constructor(_pub,\"ZMQ_StopWatch\",_pub.ZMQ_StopWatch.start)\n"
"\n"
"-- method: stop\n"
"function _meth.ZMQ_StopWatch.stop(self)\n"
"  local self,this_flags1 = obj_type_ZMQ_StopWatch_delete(self)\n"
"  if not self then return end\n"
"  local usecs1 = 0\n"
"  usecs1 = C.zmq_stopwatch_stop(self)\n"
"  return tonumber(usecs1)\n"
"end\n"
"_priv.ZMQ_StopWatch.__gc = _meth.ZMQ_StopWatch.stop\n"
"\n"
"_push.ZMQ_StopWatch = obj_type_ZMQ_StopWatch_push\n"
"ffi.metatype(\"ZMQ_StopWatch\", _priv.ZMQ_StopWatch)\n"
"-- End \"ZMQ_StopWatch\" FFI interface\n"
"\n"
"-- method: init\n"
"function _M.init(io_threads1)\n"
"  io_threads1 = io_threads1 or 1\n"
"  local rc_zmq_init_flags1 = OBJ_UDATA_FLAG_OWN\n"
"  local rc_zmq_init1\n"
"  rc_zmq_init1 = C.zmq_init(io_threads1)\n"
"  if (nil == rc_zmq_init1) then\n"
"    return nil, get_zmq_strerror()\n"
"  end\n"
"  return obj_type_ZMQ_Ctx_push(rc_zmq_init1, rc_zmq_init_flags1)\n"
"end\n"
"\n"
"-- method: init_ctx\n"
"function _M.init_ctx(ptr1)\n"
"  local ctx1\n"
"	local p_type = type(ptr1)\n"
"	if p_type == 'userdata' then\n"
"		ctx1 = ffi.cast('ZMQ_Ctx *', ptr1);\n"
"	elseif p_type == 'cdata' and ffi.istype('void *', ptr1) then\n"
"		ctx1 = ffi.cast('ZMQ_Ctx *', ptr1);\n"
"	else\n"
"		return error(\"expected lightuserdata/cdata<void *>\");\n"
"	end\n"
"\n"
"  if (nil == ctx1) then\n"
"    return nil, get_zmq_strerror()\n"
"  end\n"
"  return obj_type_ZMQ_Ctx_push(ctx1, 0)\n"
"end\n"
"\n"
"-- method: device\n"
"if (_M.device) then\n"
"function _M.device(device1, insock2, outsock3)\n"
"  \n"
"  \n"
"  \n"
"  local rc_zmq_device1 = 0\n"
"  rc_zmq_device1 = C.zmq_device(device1, insock2, outsock3)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_device1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_device1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: proxy\n"
"if (_M.proxy) then\n"
"function _M.proxy(frontend1, backend2, capture3)\n"
"  \n"
"  \n"
"  capture3 = capture3 and obj_type_ZMQ_Socket_check(capture3) or nil\n"
"  local rc_zmq_proxy1 = 0\n"
"  rc_zmq_proxy1 = C.zmq_proxy(frontend1, backend2, capture3)\n"
"  -- check for error.\n"
"  if (-1 == rc_zmq_proxy1) then\n"
"    return nil, error_code__ZMQ_Error__push(rc_zmq_proxy1)\n"
"  end\n"
"  return true\n"
"end\n"
"end\n"
"\n"
"-- method: stopwatch_start\n"
"function _M.stopwatch_start()\n"
"  local rc_zmq_stopwatch_start_flags1 = OBJ_UDATA_FLAG_OWN\n"
"  local rc_zmq_stopwatch_start1\n"
"  rc_zmq_stopwatch_start1 = C.zmq_stopwatch_start()\n"
"  return obj_type_ZMQ_StopWatch_push(rc_zmq_stopwatch_start1, rc_zmq_stopwatch_start_flags1)\n"
"end\n"
"\n"
"-- method: sleep\n"
"function _M.sleep(seconds_1)\n"
"  \n"
"  C.zmq_sleep(seconds_1)\n"
"  return \n"
"end\n"
"\n", NULL };
static char *zmq_ZErrors_key = "zmq_ZErrors_key";
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



/* detect really old ZeroMQ 2.0.x series. */
#if !defined(ZMQ_RCVMORE)
#  error "Your version of ZeroMQ is too old.  Please upgrade to version 2.1 or to the latest 2.0.x"
#endif

typedef struct ZMQ_Socket ZMQ_Socket;

#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

#if VERSION_2_0
#  define zmq_sendmsg      zmq_send
#  define zmq_recvmsg      zmq_recv
#endif

/* socket option types. */
#define OPT_TYPE_NONE		0
#define OPT_TYPE_INT		1
#define OPT_TYPE_UINT32	2
#define OPT_TYPE_UINT64	3
#define OPT_TYPE_INT64	4
#define OPT_TYPE_BLOB		5
#define OPT_TYPE_FD			6

static const int opt_types[] = {
  OPT_TYPE_NONE,    /*  0 unused */
#if VERSION_2_0
#define VERSION_2_0_MAX_OPT 13
  OPT_TYPE_UINT64,  /* 1 ZMQ_HWM */
  OPT_TYPE_NONE,  /* 2 unused */
  OPT_TYPE_INT64,  /* 3 ZMQ_SWAP */
  OPT_TYPE_UINT64,  /* 4 ZMQ_AFFINITY */
  OPT_TYPE_BLOB,  /* 5 ZMQ_IDENTITY */
  OPT_TYPE_BLOB,  /* 6 ZMQ_SUBSCRIBE */
  OPT_TYPE_BLOB,  /* 7 ZMQ_UNSUBSCRIBE */
  OPT_TYPE_INT64,  /* 8 ZMQ_RATE */
  OPT_TYPE_INT64,  /* 9 ZMQ_RECOVERY_IVL */
  OPT_TYPE_INT64,  /* 10 ZMQ_MCAST_LOOP */
  OPT_TYPE_UINT64,  /* 11 ZMQ_SNDBUF */
  OPT_TYPE_UINT64,  /* 12 ZMQ_RCVBUF */
  OPT_TYPE_INT64,  /* 13 ZMQ_RCVMORE */
#endif /* #if VERSION_2_0 */
#if VERSION_2_1
#define VERSION_2_1_MAX_OPT 21
  OPT_TYPE_FD,  /* 14 ZMQ_FD */
  OPT_TYPE_UINT32,  /* 15 ZMQ_EVENTS */
  OPT_TYPE_INT,  /* 16 ZMQ_TYPE */
  OPT_TYPE_INT,  /* 17 ZMQ_LINGER */
  OPT_TYPE_INT,  /* 18 ZMQ_RECONNECT_IVL */
  OPT_TYPE_INT,  /* 19 ZMQ_BACKLOG */
  OPT_TYPE_INT64,  /* 20 ZMQ_RECOVERY_IVL_MSEC */
  OPT_TYPE_INT,  /* 21 ZMQ_RECONNECT_IVL_MAX */
#endif /* #if VERSION_2_1 */
#if VERSION_2_2
#define VERSION_2_2_MAX_OPT 28
  OPT_TYPE_NONE,  /* 22 unused */
  OPT_TYPE_NONE,  /* 23 unused */
  OPT_TYPE_NONE,  /* 24 unused */
  OPT_TYPE_NONE,  /* 25 unused */
  OPT_TYPE_NONE,  /* 26 unused */
  OPT_TYPE_INT,  /* 27 ZMQ_RCVTIMEO */
  OPT_TYPE_INT,  /* 28 ZMQ_SNDTIMEO */
#endif /* #if VERSION_2_2 */
#if VERSION_3_0
#define VERSION_3_0_MAX_OPT 31
  OPT_TYPE_INT,  /* 1 ZMQ_HWM */
  OPT_TYPE_NONE,  /* 2 unused */
  OPT_TYPE_NONE,  /* 3 unused */
  OPT_TYPE_UINT64,  /* 4 ZMQ_AFFINITY */
  OPT_TYPE_BLOB,  /* 5 ZMQ_IDENTITY */
  OPT_TYPE_BLOB,  /* 6 ZMQ_SUBSCRIBE */
  OPT_TYPE_BLOB,  /* 7 ZMQ_UNSUBSCRIBE */
  OPT_TYPE_INT,  /* 8 ZMQ_RATE */
  OPT_TYPE_INT,  /* 9 ZMQ_RECOVERY_IVL */
  OPT_TYPE_NONE,  /* 10 unused */
  OPT_TYPE_INT,  /* 11 ZMQ_SNDBUF */
  OPT_TYPE_INT,  /* 12 ZMQ_RCVBUF */
  OPT_TYPE_INT,  /* 13 ZMQ_RCVMORE */
  OPT_TYPE_FD,  /* 14 ZMQ_FD */
  OPT_TYPE_INT,  /* 15 ZMQ_EVENTS */
  OPT_TYPE_INT,  /* 16 ZMQ_TYPE */
  OPT_TYPE_INT,  /* 17 ZMQ_LINGER */
  OPT_TYPE_INT,  /* 18 ZMQ_RECONNECT_IVL */
  OPT_TYPE_INT,  /* 19 ZMQ_BACKLOG */
  OPT_TYPE_NONE,  /* 20 unused */
  OPT_TYPE_INT,  /* 21 ZMQ_RECONNECT_IVL_MAX */
  OPT_TYPE_INT64,  /* 22 ZMQ_MAXMSGSIZE */
  OPT_TYPE_INT,  /* 23 ZMQ_SNDHWM */
  OPT_TYPE_INT,  /* 24 ZMQ_RCVHWM */
  OPT_TYPE_INT,  /* 25 ZMQ_MULTICAST_HOPS */
  OPT_TYPE_NONE,  /* 26 unused */
  OPT_TYPE_INT,  /* 27 ZMQ_RCVTIMEO */
  OPT_TYPE_INT,  /* 28 ZMQ_SNDTIMEO */
  OPT_TYPE_NONE,  /* 29 unused */
  OPT_TYPE_NONE,  /* 30 unused */
  OPT_TYPE_INT,  /* 31 ZMQ_IPV4ONLY */
#endif /* #if VERSION_3_0 */
#if VERSION_4_0
#define VERSION_4_0_MAX_OPT 55
  OPT_TYPE_INT,  /* 1 ZMQ_HWM */
  OPT_TYPE_NONE,  /* 2 unused */
  OPT_TYPE_NONE,  /* 3 unused */
  OPT_TYPE_UINT64,  /* 4 ZMQ_AFFINITY */
  OPT_TYPE_BLOB,  /* 5 ZMQ_IDENTITY */
  OPT_TYPE_BLOB,  /* 6 ZMQ_SUBSCRIBE */
  OPT_TYPE_BLOB,  /* 7 ZMQ_UNSUBSCRIBE */
  OPT_TYPE_INT,  /* 8 ZMQ_RATE */
  OPT_TYPE_INT,  /* 9 ZMQ_RECOVERY_IVL */
  OPT_TYPE_NONE,  /* 10 unused */
  OPT_TYPE_INT,  /* 11 ZMQ_SNDBUF */
  OPT_TYPE_INT,  /* 12 ZMQ_RCVBUF */
  OPT_TYPE_INT,  /* 13 ZMQ_RCVMORE */
  OPT_TYPE_FD,  /* 14 ZMQ_FD */
  OPT_TYPE_INT,  /* 15 ZMQ_EVENTS */
  OPT_TYPE_INT,  /* 16 ZMQ_TYPE */
  OPT_TYPE_INT,  /* 17 ZMQ_LINGER */
  OPT_TYPE_INT,  /* 18 ZMQ_RECONNECT_IVL */
  OPT_TYPE_INT,  /* 19 ZMQ_BACKLOG */
  OPT_TYPE_NONE,  /* 20 unused */
  OPT_TYPE_INT,  /* 21 ZMQ_RECONNECT_IVL_MAX */
  OPT_TYPE_INT64,  /* 22 ZMQ_MAXMSGSIZE */
  OPT_TYPE_INT,  /* 23 ZMQ_SNDHWM */
  OPT_TYPE_INT,  /* 24 ZMQ_RCVHWM */
  OPT_TYPE_INT,  /* 25 ZMQ_MULTICAST_HOPS */
  OPT_TYPE_NONE,  /* 26 unused */
  OPT_TYPE_INT,  /* 27 ZMQ_RCVTIMEO */
  OPT_TYPE_INT,  /* 28 ZMQ_SNDTIMEO */
  OPT_TYPE_NONE,  /* 29 unused */
  OPT_TYPE_NONE,  /* 30 unused */
  OPT_TYPE_INT,  /* 31 ZMQ_IPV4ONLY */
  OPT_TYPE_BLOB,  /* 32 ZMQ_LAST_ENDPOINT */
  OPT_TYPE_INT,  /* 33 ZMQ_ROUTER_MANDATORY */
  OPT_TYPE_INT,  /* 34 ZMQ_TCP_KEEPALIVE */
  OPT_TYPE_INT,  /* 35 ZMQ_TCP_KEEPALIVE_CNT */
  OPT_TYPE_INT,  /* 36 ZMQ_TCP_KEEPALIVE_IDLE */
  OPT_TYPE_INT,  /* 37 ZMQ_TCP_KEEPALIVE_INTVL */
  OPT_TYPE_BLOB,  /* 38 ZMQ_TCP_ACCEPT_FILTER */
  OPT_TYPE_INT,  /* 39 ZMQ_IMMEDIATE */
  OPT_TYPE_INT,  /* 40 ZMQ_XPUB_VERBOSE */
  OPT_TYPE_INT,  /* 41 ZMQ_ROUTER_RAW */
  OPT_TYPE_INT,  /* 42 ZMQ_IPV6 */
  OPT_TYPE_INT,  /* 43 ZMQ_MECHANISM */
  OPT_TYPE_INT,  /* 44 ZMQ_PLAIN_SERVER */
  OPT_TYPE_BLOB,  /* 45 ZMQ_PLAIN_USERNAME */
  OPT_TYPE_BLOB,  /* 46 ZMQ_PLAIN_PASSWORD */
  OPT_TYPE_INT,  /* 47 ZMQ_CURVE_SERVER */
  OPT_TYPE_BLOB,  /* 48 ZMQ_CURVE_PUBLICKEY */
  OPT_TYPE_BLOB,  /* 49 ZMQ_CURVE_SECRETKEY */
  OPT_TYPE_BLOB,  /* 50 ZMQ_CURVE_SERVERKEY */
  OPT_TYPE_INT,  /* 51 ZMQ_PROBE_ROUTER */
  OPT_TYPE_INT,  /* 52 ZMQ_REQ_CORRELATE */
  OPT_TYPE_INT,  /* 53 ZMQ_REQ_RELAXED */
  OPT_TYPE_INT,  /* 54 ZMQ_CONFLATE */
  OPT_TYPE_BLOB,  /* 55 ZMQ_ZAP_DOMAIN */
#endif /* #if VERSION_4_0 */
#if VERSION_4_0
#  define MAX_OPTS VERSION_4_0_MAX_OPT
#elif VERSION_3_0
#  define MAX_OPTS VERSION_3_0_MAX_OPT
#else
#  if VERSION_2_2
#    define MAX_OPTS VERSION_2_2_MAX_OPT
#  elif VERSION_2_1
#    define MAX_OPTS VERSION_2_1_MAX_OPT
#  else
#    define MAX_OPTS VERSION_2_0_MAX_OPT
#  endif
#endif
};

#if VERSION_2_0
LUA_NOBJ_API ZMQ_Error lzmq_socket_set_hwm(ZMQ_Socket *sock, int value) {
	uint64_t val = (uint64_t)value;
	return zmq_setsockopt(sock, ZMQ_HWM, &val, sizeof(val));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_hwm(ZMQ_Socket *sock, int *value) {
	uint64_t val;
	size_t val_len = sizeof(val);
	int rc = zmq_getsockopt(sock, ZMQ_HWM, &val, &val_len);
	*value = (int)val;
	return rc;
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_swap(ZMQ_Socket *sock, int value) {
	int64_t val = (int64_t)value;
	return zmq_setsockopt(sock, ZMQ_SWAP, &val, sizeof(val));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_swap(ZMQ_Socket *sock, int *value) {
	int64_t val;
	size_t val_len = sizeof(val);
	int rc = zmq_getsockopt(sock, ZMQ_SWAP, &val, &val_len);
	*value = (int)val;
	return rc;
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_affinity(ZMQ_Socket *sock, uint64_t value) {
	return zmq_setsockopt(sock, ZMQ_AFFINITY, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_affinity(ZMQ_Socket *sock, uint64_t *value) {
	size_t val_len = sizeof(uint64_t);
	return zmq_getsockopt(sock, ZMQ_AFFINITY, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_identity(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_IDENTITY, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_identity(ZMQ_Socket *sock, char *value, size_t *len) {
	return zmq_getsockopt(sock, ZMQ_IDENTITY, value, len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_subscribe(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_SUBSCRIBE, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_unsubscribe(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_UNSUBSCRIBE, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_rate(ZMQ_Socket *sock, int value) {
	int64_t val = (int64_t)value;
	return zmq_setsockopt(sock, ZMQ_RATE, &val, sizeof(val));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rate(ZMQ_Socket *sock, int *value) {
	int64_t val;
	size_t val_len = sizeof(val);
	int rc = zmq_getsockopt(sock, ZMQ_RATE, &val, &val_len);
	*value = (int)val;
	return rc;
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_recovery_ivl(ZMQ_Socket *sock, int value) {
	int64_t val = (int64_t)value;
	return zmq_setsockopt(sock, ZMQ_RECOVERY_IVL, &val, sizeof(val));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_recovery_ivl(ZMQ_Socket *sock, int *value) {
	int64_t val;
	size_t val_len = sizeof(val);
	int rc = zmq_getsockopt(sock, ZMQ_RECOVERY_IVL, &val, &val_len);
	*value = (int)val;
	return rc;
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_mcast_loop(ZMQ_Socket *sock, int value) {
	int64_t val = (int64_t)value;
	return zmq_setsockopt(sock, ZMQ_MCAST_LOOP, &val, sizeof(val));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_mcast_loop(ZMQ_Socket *sock, int *value) {
	int64_t val;
	size_t val_len = sizeof(val);
	int rc = zmq_getsockopt(sock, ZMQ_MCAST_LOOP, &val, &val_len);
	*value = (int)val;
	return rc;
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_sndbuf(ZMQ_Socket *sock, int value) {
	uint64_t val = (uint64_t)value;
	return zmq_setsockopt(sock, ZMQ_SNDBUF, &val, sizeof(val));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_sndbuf(ZMQ_Socket *sock, int *value) {
	uint64_t val;
	size_t val_len = sizeof(val);
	int rc = zmq_getsockopt(sock, ZMQ_SNDBUF, &val, &val_len);
	*value = (int)val;
	return rc;
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_rcvbuf(ZMQ_Socket *sock, int value) {
	uint64_t val = (uint64_t)value;
	return zmq_setsockopt(sock, ZMQ_RCVBUF, &val, sizeof(val));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rcvbuf(ZMQ_Socket *sock, int *value) {
	uint64_t val;
	size_t val_len = sizeof(val);
	int rc = zmq_getsockopt(sock, ZMQ_RCVBUF, &val, &val_len);
	*value = (int)val;
	return rc;
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rcvmore(ZMQ_Socket *sock, int *value) {
	int64_t val;
	size_t val_len = sizeof(val);
	int rc = zmq_getsockopt(sock, ZMQ_RCVMORE, &val, &val_len);
	*value = (int)val;
	return rc;
}

#endif /* #if VERSION_2_0 */
#if VERSION_2_1
LUA_NOBJ_API ZMQ_Error lzmq_socket_fd(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_FD, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_events(ZMQ_Socket *sock, int *value) {
	uint32_t val;
	size_t val_len = sizeof(val);
	int rc = zmq_getsockopt(sock, ZMQ_EVENTS, &val, &val_len);
	*value = (int)val;
	return rc;
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_type(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_TYPE, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_linger(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_LINGER, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_linger(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_LINGER, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_reconnect_ivl(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RECONNECT_IVL, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_reconnect_ivl(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RECONNECT_IVL, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_backlog(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_BACKLOG, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_backlog(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_BACKLOG, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_recovery_ivl_msec(ZMQ_Socket *sock, int64_t value) {
	return zmq_setsockopt(sock, ZMQ_RECOVERY_IVL_MSEC, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_recovery_ivl_msec(ZMQ_Socket *sock, int64_t *value) {
	size_t val_len = sizeof(int64_t);
	return zmq_getsockopt(sock, ZMQ_RECOVERY_IVL_MSEC, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_reconnect_ivl_max(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RECONNECT_IVL_MAX, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_reconnect_ivl_max(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RECONNECT_IVL_MAX, value, &val_len);
}

#endif /* #if VERSION_2_1 */
#if VERSION_2_2
LUA_NOBJ_API ZMQ_Error lzmq_socket_set_rcvtimeo(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RCVTIMEO, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rcvtimeo(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RCVTIMEO, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_sndtimeo(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_SNDTIMEO, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_sndtimeo(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_SNDTIMEO, value, &val_len);
}

#endif /* #if VERSION_2_2 */
#if VERSION_3_0
ZMQ_Error lzmq_socket_set_hwm(ZMQ_Socket *sock, int value) {
	int val;
	int rc;
	val = (int)value;
	rc = zmq_setsockopt(sock, ZMQ_SNDHWM, &value, sizeof(value));
	if(-1 == rc) return rc;
	val = (int)value;
	return zmq_setsockopt(sock, ZMQ_RCVHWM, &value, sizeof(value));
}
ZMQ_Error lzmq_socket_hwm(ZMQ_Socket *sock, int *value) {
	size_t val_len;
	int rc;
	val_len = sizeof(value);
	rc = zmq_getsockopt(sock, ZMQ_SNDHWM, value, &val_len);
	if(-1 == rc) return rc;
	val_len = sizeof(value);
	return zmq_getsockopt(sock, ZMQ_RCVHWM, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_affinity(ZMQ_Socket *sock, uint64_t value) {
	return zmq_setsockopt(sock, ZMQ_AFFINITY, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_affinity(ZMQ_Socket *sock, uint64_t *value) {
	size_t val_len = sizeof(uint64_t);
	return zmq_getsockopt(sock, ZMQ_AFFINITY, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_identity(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_IDENTITY, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_identity(ZMQ_Socket *sock, char *value, size_t *len) {
	return zmq_getsockopt(sock, ZMQ_IDENTITY, value, len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_subscribe(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_SUBSCRIBE, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_unsubscribe(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_UNSUBSCRIBE, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_rate(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RATE, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rate(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RATE, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_recovery_ivl(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RECOVERY_IVL, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_recovery_ivl(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RECOVERY_IVL, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_sndbuf(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_SNDBUF, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_sndbuf(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_SNDBUF, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_rcvbuf(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RCVBUF, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rcvbuf(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RCVBUF, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rcvmore(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RCVMORE, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_fd(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_FD, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_events(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_EVENTS, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_type(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_TYPE, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_linger(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_LINGER, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_linger(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_LINGER, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_reconnect_ivl(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RECONNECT_IVL, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_reconnect_ivl(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RECONNECT_IVL, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_backlog(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_BACKLOG, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_backlog(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_BACKLOG, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_reconnect_ivl_max(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RECONNECT_IVL_MAX, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_reconnect_ivl_max(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RECONNECT_IVL_MAX, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_maxmsgsize(ZMQ_Socket *sock, int64_t value) {
	return zmq_setsockopt(sock, ZMQ_MAXMSGSIZE, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_maxmsgsize(ZMQ_Socket *sock, int64_t *value) {
	size_t val_len = sizeof(int64_t);
	return zmq_getsockopt(sock, ZMQ_MAXMSGSIZE, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_sndhwm(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_SNDHWM, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_sndhwm(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_SNDHWM, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_rcvhwm(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RCVHWM, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rcvhwm(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RCVHWM, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_multicast_hops(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_MULTICAST_HOPS, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_multicast_hops(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_MULTICAST_HOPS, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_rcvtimeo(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RCVTIMEO, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rcvtimeo(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RCVTIMEO, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_sndtimeo(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_SNDTIMEO, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_sndtimeo(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_SNDTIMEO, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_ipv4only(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_IPV4ONLY, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_ipv4only(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_IPV4ONLY, value, &val_len);
}

#endif /* #if VERSION_3_0 */
#if VERSION_4_0
ZMQ_Error lzmq_socket_set_hwm(ZMQ_Socket *sock, int value) {
	int val;
	int rc;
	val = (int)value;
	rc = zmq_setsockopt(sock, ZMQ_SNDHWM, &value, sizeof(value));
	if(-1 == rc) return rc;
	val = (int)value;
	return zmq_setsockopt(sock, ZMQ_RCVHWM, &value, sizeof(value));
}
ZMQ_Error lzmq_socket_hwm(ZMQ_Socket *sock, int *value) {
	size_t val_len;
	int rc;
	val_len = sizeof(value);
	rc = zmq_getsockopt(sock, ZMQ_SNDHWM, value, &val_len);
	if(-1 == rc) return rc;
	val_len = sizeof(value);
	return zmq_getsockopt(sock, ZMQ_RCVHWM, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_affinity(ZMQ_Socket *sock, uint64_t value) {
	return zmq_setsockopt(sock, ZMQ_AFFINITY, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_affinity(ZMQ_Socket *sock, uint64_t *value) {
	size_t val_len = sizeof(uint64_t);
	return zmq_getsockopt(sock, ZMQ_AFFINITY, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_identity(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_IDENTITY, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_identity(ZMQ_Socket *sock, char *value, size_t *len) {
	return zmq_getsockopt(sock, ZMQ_IDENTITY, value, len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_subscribe(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_SUBSCRIBE, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_unsubscribe(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_UNSUBSCRIBE, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_rate(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RATE, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rate(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RATE, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_recovery_ivl(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RECOVERY_IVL, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_recovery_ivl(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RECOVERY_IVL, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_sndbuf(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_SNDBUF, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_sndbuf(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_SNDBUF, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_rcvbuf(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RCVBUF, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rcvbuf(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RCVBUF, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rcvmore(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RCVMORE, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_fd(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_FD, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_events(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_EVENTS, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_type(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_TYPE, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_linger(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_LINGER, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_linger(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_LINGER, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_reconnect_ivl(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RECONNECT_IVL, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_reconnect_ivl(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RECONNECT_IVL, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_backlog(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_BACKLOG, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_backlog(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_BACKLOG, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_reconnect_ivl_max(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RECONNECT_IVL_MAX, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_reconnect_ivl_max(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RECONNECT_IVL_MAX, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_maxmsgsize(ZMQ_Socket *sock, int64_t value) {
	return zmq_setsockopt(sock, ZMQ_MAXMSGSIZE, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_maxmsgsize(ZMQ_Socket *sock, int64_t *value) {
	size_t val_len = sizeof(int64_t);
	return zmq_getsockopt(sock, ZMQ_MAXMSGSIZE, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_sndhwm(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_SNDHWM, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_sndhwm(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_SNDHWM, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_rcvhwm(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RCVHWM, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rcvhwm(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RCVHWM, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_multicast_hops(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_MULTICAST_HOPS, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_multicast_hops(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_MULTICAST_HOPS, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_rcvtimeo(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_RCVTIMEO, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_rcvtimeo(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_RCVTIMEO, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_sndtimeo(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_SNDTIMEO, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_sndtimeo(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_SNDTIMEO, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_ipv4only(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_IPV4ONLY, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_ipv4only(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_IPV4ONLY, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_last_endpoint(ZMQ_Socket *sock, char *value, size_t *len) {
	return zmq_getsockopt(sock, ZMQ_LAST_ENDPOINT, value, len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_router_mandatory(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_ROUTER_MANDATORY, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_tcp_keepalive(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_TCP_KEEPALIVE, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_tcp_keepalive(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_TCP_KEEPALIVE, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_tcp_keepalive_cnt(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_TCP_KEEPALIVE_CNT, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_tcp_keepalive_cnt(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_TCP_KEEPALIVE_CNT, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_tcp_keepalive_idle(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_TCP_KEEPALIVE_IDLE, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_tcp_keepalive_idle(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_TCP_KEEPALIVE_IDLE, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_tcp_keepalive_intvl(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_TCP_KEEPALIVE_INTVL, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_tcp_keepalive_intvl(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_TCP_KEEPALIVE_INTVL, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_tcp_accept_filter(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_TCP_ACCEPT_FILTER, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_immediate(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_IMMEDIATE, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_immediate(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_IMMEDIATE, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_xpub_verbose(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_XPUB_VERBOSE, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_router_raw(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_ROUTER_RAW, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_ipv6(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_IPV6, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_ipv6(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_IPV6, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_mechanism(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_MECHANISM, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_plain_server(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_PLAIN_SERVER, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_plain_server(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_PLAIN_SERVER, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_plain_username(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_PLAIN_USERNAME, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_plain_username(ZMQ_Socket *sock, char *value, size_t *len) {
	return zmq_getsockopt(sock, ZMQ_PLAIN_USERNAME, value, len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_plain_password(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_PLAIN_PASSWORD, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_plain_password(ZMQ_Socket *sock, char *value, size_t *len) {
	return zmq_getsockopt(sock, ZMQ_PLAIN_PASSWORD, value, len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_curve_server(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_CURVE_SERVER, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_curve_server(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_CURVE_SERVER, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_curve_publickey(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_CURVE_PUBLICKEY, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_curve_publickey(ZMQ_Socket *sock, char *value, size_t *len) {
	return zmq_getsockopt(sock, ZMQ_CURVE_PUBLICKEY, value, len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_curve_secretkey(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_CURVE_SECRETKEY, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_curve_secretkey(ZMQ_Socket *sock, char *value, size_t *len) {
	return zmq_getsockopt(sock, ZMQ_CURVE_SECRETKEY, value, len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_curve_serverkey(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_CURVE_SERVERKEY, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_curve_serverkey(ZMQ_Socket *sock, char *value, size_t *len) {
	return zmq_getsockopt(sock, ZMQ_CURVE_SERVERKEY, value, len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_probe_router(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_PROBE_ROUTER, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_req_correlate(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_REQ_CORRELATE, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_req_relaxed(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_REQ_RELAXED, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_conflate(ZMQ_Socket *sock, int value) {
	return zmq_setsockopt(sock, ZMQ_CONFLATE, &value, sizeof(value));
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_conflate(ZMQ_Socket *sock, int *value) {
	size_t val_len = sizeof(int);
	return zmq_getsockopt(sock, ZMQ_CONFLATE, value, &val_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_set_zap_domain(ZMQ_Socket *sock, const char *value, size_t str_len) {
	return zmq_setsockopt(sock, ZMQ_ZAP_DOMAIN, value, str_len);
}

LUA_NOBJ_API ZMQ_Error lzmq_socket_zap_domain(ZMQ_Socket *sock, char *value, size_t *len) {
	return zmq_getsockopt(sock, ZMQ_ZAP_DOMAIN, value, len);
}

#endif /* #if VERSION_4_0 */

LUA_NOBJ_API ZMQ_Error simple_zmq_send(ZMQ_Socket *sock, const char *data, size_t data_len, int flags) {
#if VERSION_3_2
	return zmq_send(sock, data, data_len, flags);
#else
	ZMQ_Error err;
	zmq_msg_t msg;
	/* initialize message */
	err = zmq_msg_init_size(&msg, data_len);
	if(0 == err) {
		/* fill message */
		memcpy(zmq_msg_data(&msg), data, data_len);
		/* send message */
		err = zmq_sendmsg(sock, &msg, flags);
		/* close message */
		zmq_msg_close(&msg);
	}
	return err;
#endif
}

typedef struct ZMQ_recv_event {
	int        event_id;
	int        value;
	const char *addr;
	size_t     addr_len;
	const char *err;
} ZMQ_recv_event;

#if (ZMQ_VERSION_MAJOR == 4) && (ZMQ_VERSION_MINOR >= 1)
typedef struct zmq_event_t {
	int16_t   event;
	int32_t   value;
} zmq_event_t;

#endif

int monitor_recv_event(ZMQ_Socket *s, zmq_msg_t *msg, int flags, ZMQ_recv_event *ev)
{
	int rc ;
	zmq_event_t event;

	ev->event_id = 0;
	ev->value = 0;
	ev->addr = NULL;
	ev->err = NULL;
	ev->addr_len = 0;
	zmq_msg_init(msg);

	/* recv binary event. */
	rc = zmq_recvmsg(s, msg, flags);
	if(rc < 0) {
		return rc;
	}
#if ZMQ_VERSION_MAJOR == 3
	if(zmq_msg_size(msg) != sizeof(event)) {
		ev->err = "Invalid monitor event.  Wrong event size.";
		return -1;
	}
	memcpy(&event, zmq_msg_data(msg), sizeof(event));
	ev->event_id = event.event;

	switch(event.event) {
	case ZMQ_EVENT_CONNECTED:
		ev->value = event.data.connected.fd;
		ev->addr = event.data.connected.addr;
		break;
	case ZMQ_EVENT_CONNECT_DELAYED:
		ev->value = event.data.connect_delayed.err;
		ev->addr = event.data.connect_delayed.addr;
		break;
	case ZMQ_EVENT_CONNECT_RETRIED:
		ev->value = event.data.connect_retried.interval;
		ev->addr = event.data.connect_retried.addr;
		break;
	case ZMQ_EVENT_LISTENING:
		ev->value = event.data.listening.fd;
		ev->addr = event.data.listening.addr;
		break;
	case ZMQ_EVENT_BIND_FAILED:
		ev->value = event.data.bind_failed.err;
		ev->addr = event.data.bind_failed.addr;
		break;
	case ZMQ_EVENT_ACCEPTED:
		ev->value = event.data.accepted.fd;
		ev->addr = event.data.accepted.addr;
		break;
	case ZMQ_EVENT_ACCEPT_FAILED:
		ev->value = event.data.accept_failed.err;
		ev->addr = event.data.accept_failed.addr;
		break;
	case ZMQ_EVENT_CLOSED:
		ev->value = event.data.closed.fd;
		ev->addr = event.data.closed.addr;
		break;
	case ZMQ_EVENT_CLOSE_FAILED:
		ev->value = event.data.close_failed.err;
		ev->addr = event.data.close_failed.addr;
		break;
	case ZMQ_EVENT_DISCONNECTED:
		ev->value = event.data.disconnected.fd;
		ev->addr = event.data.disconnected.addr;
		break;
	}
	if(ev->addr) {
		ev->addr_len = strlen(ev->addr);
	}

	if(zmq_msg_more(msg) != 0) {
		ev->err = "Invalid monitor event.  Has too many parts.";
		return -1;
	}
#else
	if(zmq_msg_size(msg) != (sizeof(event.event) + sizeof(event.value))) {
		ev->err = "Invalid monitor event.  Wrong event size.";
		return -1;
	}
	/* copy binary data to event struct */
	const char* data = (char*)zmq_msg_data(msg);
	memcpy(&(event.event), data, sizeof(event.event));
	memcpy(&(event.value), data+sizeof(event.event), sizeof(event.value));
	ev->event_id = event.event;
	ev->value = event.value;

	if(zmq_msg_more(msg) == 0) {
		ev->err = "Invalid monitor event.  Missing address part.";
		return -1;
	}
	ev->value = event.value;

	/* recv address part */
	rc = zmq_recvmsg(s, msg, flags);
	if(rc < 0) {
		return rc;
	}
	if(zmq_msg_more(msg) != 0) {
		ev->err = "Invalid monitor event.  Has too many parts.";
		return -1;
	}
	/* copy address part */
	ev->addr_len = zmq_msg_size(msg) ;
	ev->addr = zmq_msg_data(msg);
#endif

	return 1;
}


struct ZMQ_Poller {
	zmq_pollitem_t *items;
	int    next;
	int    count;
	int    free_list;
	int    len;
};


typedef struct ZMQ_Poller ZMQ_Poller;

#define FREE_ITEM_EVENTS_TAG ((short)0xFFFF)

#define ITEM_TO_INDEX(items, item) (item - (items))

static int poller_resize_items(ZMQ_Poller *poller, int len) {
	int old_len = poller->len;

	/* make sure new length is atleast as large as items count. */
	len = (poller->count <= len) ? len : poller->count;

	/* if the new length is the same as the old length, then don't try to resize. */
	if(old_len == len) return len;

	poller->items = (zmq_pollitem_t *)realloc(poller->items, len * sizeof(zmq_pollitem_t));
	poller->len = len;
	if(len > old_len) {
		/* clear new space. */
		memset(&(poller->items[old_len]), 0, (len - old_len) * sizeof(zmq_pollitem_t));
	}
	return len;
}

void poller_init(ZMQ_Poller *poller, int length) {
	poller->items = (zmq_pollitem_t *)calloc(length, sizeof(zmq_pollitem_t));
	poller->next = -1;
	poller->count = 0;
	poller->len = length;
	poller->free_list = -1;
}

void poller_cleanup(ZMQ_Poller *poller) {
	free(poller->items);
	poller->items = NULL;
	poller->next = -1;
	poller->count = 0;
	poller->len = 0;
	poller->free_list = -1;
}

int poller_find_sock_item(ZMQ_Poller *poller, ZMQ_Socket *sock) {
	zmq_pollitem_t *items;
	int count;
	int n;

	/* find ZMQ_Socket */
	items = poller->items;
	count = poller->count;
	for(n=0; n < count; n++) {
		if(items[n].socket == sock) return n;
	}
	/* not found. */
	return -1;
}

int poller_find_fd_item(ZMQ_Poller *poller, socket_t fd) {
	zmq_pollitem_t *items;
	int count;
	int n;

	/* find fd */
	items = poller->items;
	count = poller->count;
	for(n=0; n < count; n++) {
		if(items[n].fd == fd) return n;
	}
	/* not found. */
	return -1;
}

void poller_remove_item(ZMQ_Poller *poller, int idx) {
	zmq_pollitem_t *items;
	int free_list;
	int count;

	count = poller->count;
	/* no item to remove. */
	if(idx >= count || count == 0) return;

	items = poller->items;
	free_list = poller->free_list;

	/* link new free slot to head of free list. */
	if(free_list >= 0 && free_list < count) {
		/* use socket pointer for free list's 'next' field. */
		items[idx].socket = &(items[free_list]);
	} else {
		/* free list is empty mark poller slot as the end. */
		items[idx].socket = NULL;
	}
	poller->free_list = idx;
	/* mark poller slot as a free slot. */
	items[idx].events = FREE_ITEM_EVENTS_TAG;
	/* clear old revents. */
	items[idx].revents = 0;
}

int poller_get_free_item(ZMQ_Poller *poller) {
	zmq_pollitem_t *curr;
	zmq_pollitem_t *next;
	int count;
	int idx;

	count = poller->count;
	idx = poller->free_list;
	/* check for a free slot in the free list. */
	if(idx >= 0 && idx < count) {
		/* remove free slot from free list. */
		curr = &(poller->items[idx]);
		/* valid free slot. */
		assert(curr->events == FREE_ITEM_EVENTS_TAG);
		/* is poller the last free slot? */
		next = ((zmq_pollitem_t *)curr->socket);
		if(next != NULL) {
			/* set next free slot as head of free list. */
			poller->free_list = ITEM_TO_INDEX(poller->items, next);
		} else {
			/* free list is empty now. */
			poller->free_list = -1;
		}
		/* clear slot */
		memset(curr, 0, sizeof(zmq_pollitem_t));
		return idx;
	}

	idx = count;
	poller->count = ++count;
	/* make room for new item. */
	if(count >= poller->len) {
		poller_resize_items(poller, poller->len + 10);
	}
	return idx;
}

static int poller_compact_items(ZMQ_Poller *poller) {
	zmq_pollitem_t *items;
	int count;
	int old_count;
	int next;

	count = poller->count;
	/* if no free slot, then return. */
	if(poller->free_list < 0) return count;
	old_count = count;

	items = poller->items;
	next = 0;
	/* find first free slot. */
	while(next < count && items[next].events != FREE_ITEM_EVENTS_TAG) {
		++next;
	}

	/* move non-free slots into free slot. */
	count = next;
	++next;
	while(next < old_count) {
		if(items[next].events != FREE_ITEM_EVENTS_TAG) {
			/* found non-free slot, move it to the current free slot. */
			items[count] = items[next];
			++count;
		}
		++next;
	}

	/* clear old used-space */
	memset(&(items[count]), 0, ((old_count - count) * sizeof(zmq_pollitem_t)));
	poller->count = count;
	poller->free_list = -1; /* free list is now empty. */

	assert(count <= poller->len);
	return count;
}

int poller_poll(ZMQ_Poller *poller, long timeout) {
	int count;
	/* remove free slots from items list. */
	if(poller->free_list >= 0) {
		count = poller_compact_items(poller);
	} else {
		count = poller->count;
	}
	/* poll for events. */
	return zmq_poll(poller->items, count, timeout);
}

int poller_next_revents(ZMQ_Poller *poller, int *revents) {
	zmq_pollitem_t *items;
	int count;
	int idx;
	int next;

	idx = poller->next;
	/* do we need to poll for more events? */
	if(idx < 0) {
		return idx;
	}
	items = poller->items;
	count = poller->count;
	/* find next item with pending events. */
	for(;idx < count; ++idx) {
		/* did we find a pending event? */
		if(items[idx].revents != 0) {
			*revents = items[idx].revents;
			poller->next = idx+1;
			return idx;
		}
	}
	/* processed all pending events. */
	poller->next = -1;
	*revents = 0;
	return -1;
}


typedef struct ZMQ_Ctx ZMQ_Ctx;

#if (ZMQ_VERSION_MAJOR <= 4) && (ZMQ_VERSION_MINOR <= 1)
#include "zmq_utils.h"
#endif
typedef struct ZMQ_StopWatch ZMQ_StopWatch;



/* method: description */
static int ZErrors__description__meth(lua_State *L) {
  const char * msg1 = NULL;
	int err_type;
	int err_num = -1;

	err_type = lua_type(L, 2);
	if(err_type == LUA_TSTRING) {
		lua_pushvalue(L, 2);
		lua_rawget(L, 1);
		if(lua_isnumber(L, -1)) {
			err_num = lua_tointeger(L, -1);
		}
		lua_pop(L, 1);
	} else if(err_type == LUA_TNUMBER) {
		err_num = lua_tointeger(L, 2);
	} else {
		return luaL_argerror(L, 2, "expected string/number");
	}
	if(err_num < 0) {
		lua_pushnil(L);
		lua_pushliteral(L, "UNKNOWN ERROR");
		return 2;
	}
	msg1 = strerror(err_num);

  lua_pushstring(L, msg1);
  return 1;
}

/* method: __index */
static int ZErrors____index__meth(lua_State *L) {
  int err2;
  const char * msg1 = NULL;
  err2 = luaL_checkinteger(L,2);
	switch(err2) {
	case EAGAIN:
		msg1 = "timeout";
		break;
	case EINTR:
		msg1 = "interrupted";
		break;
#if defined(ETERM)
	case ETERM:
		msg1 = "closed";
		break;
#endif
	default:
		msg1 = zmq_strerror(err2);
		break;
	}
	lua_pushvalue(L, 2);
	lua_pushstring(L, msg1);
	lua_rawset(L, 1);

  lua_pushstring(L, msg1);
  return 1;
}

static void error_code__ZMQ_Error__push(lua_State *L, ZMQ_Error err) {
  const char *err_str = NULL;
	int num;
	if(-1 == err) {
		/* get ZErrors table. */
		lua_pushlightuserdata(L, zmq_ZErrors_key);
		lua_rawget(L, LUA_REGISTRYINDEX);
		/* convert zmq_errno to string. */
		num = zmq_errno();
		lua_pushinteger(L, num);
		lua_gettable(L, -2);
		/* remove ZErrors table. */
		lua_remove(L, -2);
		if(!lua_isnil(L, -1)) {
			/* found error. */
			return;
		}
		/* Unknown error. */
		lua_pop(L, 1);
		lua_pushfstring(L, "UNKNOWN ERROR(%d)", num);
		return;
	}

	if(err_str) {
		lua_pushstring(L, err_str);
	} else {
		lua_pushnil(L);
	}
}

/* method: init */
static int zmq_msg_t__init__meth(lua_State *L) {
  zmq_msg_t this1_store;
  zmq_msg_t * this1 = &(this1_store);
  ZMQ_Error rc_zmq_msg_init2 = 0;
  rc_zmq_msg_init2 = zmq_msg_init(this1);
  if(!(-1 == rc_zmq_msg_init2)) {
    obj_type_zmq_msg_t_push(L, this1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_zmq_msg_init2);
  return 2;
}

/* method: init_size */
static int zmq_msg_t__init_size__meth(lua_State *L) {
  size_t size1;
  zmq_msg_t this1_store;
  zmq_msg_t * this1 = &(this1_store);
  ZMQ_Error rc_zmq_msg_init_size2 = 0;
  size1 = luaL_checkinteger(L,1);
  rc_zmq_msg_init_size2 = zmq_msg_init_size(this1, size1);
  if(!(-1 == rc_zmq_msg_init_size2)) {
    obj_type_zmq_msg_t_push(L, this1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_zmq_msg_init_size2);
  return 2;
}

/* method: init_data */
static int zmq_msg_t__init_data__meth(lua_State *L) {
  size_t data_len1;
  const char * data1;
  zmq_msg_t this1_store;
  zmq_msg_t * this1 = &(this1_store);
  ZMQ_Error err2 = 0;
  data1 = luaL_checklstring(L,1,&(data_len1));
  err2 = zmq_msg_init_size(this1, data_len1);
	if(0 == err2) {
		/* fill message */
		memcpy(zmq_msg_data(this1), data1, data_len1);
	}

  if(!(-1 == err2)) {
    obj_type_zmq_msg_t_push(L, this1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, err2);
  return 2;
}

/* method: _priv */
static int zmq_msg_t__delete__meth(lua_State *L) {
  zmq_msg_t * this1;
  ZMQ_Error rc_zmq_msg_close1 = 0;
  this1 = obj_type_zmq_msg_t_delete(L,1);
  rc_zmq_msg_close1 = zmq_msg_close(this1);
  /* check for error. */
  if((-1 == rc_zmq_msg_close1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_msg_close1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: close */
static int zmq_msg_t__close__meth(lua_State *L) {
  zmq_msg_t * this1;
  ZMQ_Error rc_zmq_msg_close1 = 0;
  this1 = obj_type_zmq_msg_t_check(L,1);
  rc_zmq_msg_close1 = zmq_msg_close(this1);
  /* check for error. */
  if((-1 == rc_zmq_msg_close1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_msg_close1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: move */
static int zmq_msg_t__move__meth(lua_State *L) {
  zmq_msg_t * this1;
  zmq_msg_t * src2;
  ZMQ_Error rc_zmq_msg_move1 = 0;
  this1 = obj_type_zmq_msg_t_check(L,1);
  src2 = obj_type_zmq_msg_t_check(L,2);
  rc_zmq_msg_move1 = zmq_msg_move(this1, src2);
  /* check for error. */
  if((-1 == rc_zmq_msg_move1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_msg_move1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: copy */
static int zmq_msg_t__copy__meth(lua_State *L) {
  zmq_msg_t * this1;
  zmq_msg_t * src2;
  ZMQ_Error rc_zmq_msg_copy1 = 0;
  this1 = obj_type_zmq_msg_t_check(L,1);
  src2 = obj_type_zmq_msg_t_check(L,2);
  rc_zmq_msg_copy1 = zmq_msg_copy(this1, src2);
  /* check for error. */
  if((-1 == rc_zmq_msg_copy1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_msg_copy1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: set_data */
static int zmq_msg_t__set_data__meth(lua_State *L) {
  zmq_msg_t * this1;
  size_t data_len2;
  const char * data2;
  ZMQ_Error err1 = 0;
  this1 = obj_type_zmq_msg_t_check(L,1);
  data2 = luaL_checklstring(L,2,&(data_len2));
	/* check message data size. */
	if(zmq_msg_size(this1) != data_len2) {
		/* need to resize message. */
		zmq_msg_close(this1); /* close old message, to free old data. */
		err1 = zmq_msg_init_size(this1, data_len2); /* re-initialize message. */
		if(0 != err1) {
			luaL_error(L, "set_data() failed: %s", get_zmq_strerror());
		}
	}
	/* copy data into message */
	memcpy(zmq_msg_data(this1), data2, data_len2);

  /* check for error. */
  if((-1 == err1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, err1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: data */
static int zmq_msg_t__data__meth(lua_State *L) {
  zmq_msg_t * this1;
  void * rc_zmq_msg_data1 = NULL;
  this1 = obj_type_zmq_msg_t_check(L,1);
  rc_zmq_msg_data1 = zmq_msg_data(this1);
  lua_pushlightuserdata(L, rc_zmq_msg_data1);
  return 1;
}

/* method: set_size */
static int zmq_msg_t__set_size__meth(lua_State *L) {
  zmq_msg_t * this1;
  size_t size2;
  ZMQ_Error err1 = 0;
  this1 = obj_type_zmq_msg_t_check(L,1);
  size2 = luaL_checkinteger(L,2);
	/* check message data size. */
	if(zmq_msg_size(this1) != size2) {
		/* need to resize message. */
		zmq_msg_close(this1); /* close old message, to free old data. */
		err1 = zmq_msg_init_size(this1, size2); /* re-initialize message. */
		if(0 != err1) {
			luaL_error(L, "set_size() failed: %s", get_zmq_strerror());
		}
	}

  /* check for error. */
  if((-1 == err1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, err1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: size */
static int zmq_msg_t__size__meth(lua_State *L) {
  zmq_msg_t * this1;
  size_t size1 = 0;
  this1 = obj_type_zmq_msg_t_check(L,1);
  size1 = zmq_msg_size(this1);
  lua_pushinteger(L, size1);
  return 1;
}

/* method: __tostring */
static int zmq_msg_t____tostring__meth(lua_State *L) {
  zmq_msg_t * this1;
  size_t data_len1 = 0;
  const char * data1 = NULL;
  this1 = obj_type_zmq_msg_t_check(L,1);
	data1 = zmq_msg_data(this1);
	data_len1 = zmq_msg_size(this1);

  if(data1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, data1,data_len1);
  return 1;
}

/* method: close */
static int ZMQ_Socket__close__meth(lua_State *L) {
  int this_flags1 = 0;
  ZMQ_Socket * this1;
  ZMQ_Error rc_zmq_close1 = 0;
  this1 = obj_type_ZMQ_Socket_delete(L,1,&(this_flags1));
  if(!(this_flags1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  rc_zmq_close1 = zmq_close(this1);
  /* check for error. */
  if((-1 == rc_zmq_close1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_close1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: bind */
static int ZMQ_Socket__bind__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t addr_len2;
  const char * addr2;
  ZMQ_Error rc_zmq_bind1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  addr2 = luaL_checklstring(L,2,&(addr_len2));
  rc_zmq_bind1 = zmq_bind(this1, addr2);
  /* check for error. */
  if((-1 == rc_zmq_bind1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_bind1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: unbind */
#if (VERSION_3_2|VERSION_4_0)
static int ZMQ_Socket__unbind__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t addr_len2;
  const char * addr2;
  ZMQ_Error rc_zmq_unbind1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  addr2 = luaL_checklstring(L,2,&(addr_len2));
  rc_zmq_unbind1 = zmq_unbind(this1, addr2);
  /* check for error. */
  if((-1 == rc_zmq_unbind1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_unbind1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: connect */
static int ZMQ_Socket__connect__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t addr_len2;
  const char * addr2;
  ZMQ_Error rc_zmq_connect1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  addr2 = luaL_checklstring(L,2,&(addr_len2));
  rc_zmq_connect1 = zmq_connect(this1, addr2);
  /* check for error. */
  if((-1 == rc_zmq_connect1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_connect1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: disconnect */
#if (VERSION_3_2|VERSION_4_0)
static int ZMQ_Socket__disconnect__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t addr_len2;
  const char * addr2;
  ZMQ_Error rc_zmq_disconnect1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  addr2 = luaL_checklstring(L,2,&(addr_len2));
  rc_zmq_disconnect1 = zmq_disconnect(this1, addr2);
  /* check for error. */
  if((-1 == rc_zmq_disconnect1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_disconnect1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: setopt */
static int ZMQ_Socket__setopt__meth(lua_State *L) {
  ZMQ_Socket * this1;
  uint32_t opt2;
  ZMQ_Error err1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  opt2 = luaL_checkinteger(L,2);
	size_t val_len;
	const void *val;

#if VERSION_2_1 || VERSION_3_0 || VERSION_4_0
	socket_t fd_val;
#endif
	int int_val;
	uint32_t uint32_val;
	uint64_t uint64_val;
	int64_t int64_val;

#if VERSION_3_0 || VERSION_4_0
	/* 3.0 backwards compatibility support for HWM. */
	if(opt2 == ZMQ_HWM) {
		int_val = luaL_checkinteger(L, 3);
		val = &int_val;
		val_len = sizeof(int_val);
		err1 = zmq_setsockopt(this1, ZMQ_SNDHWM, val, val_len);
		if(-1 != err1) {
			err1 = zmq_setsockopt(this1, ZMQ_RCVHWM, val, val_len);
		}
		goto finished;
	}
#endif

	if(opt2 > MAX_OPTS) {
		return luaL_argerror(L, 2, "Invalid socket option.");
	}

	switch(opt_types[opt2]) {
#if VERSION_2_1 || VERSION_3_0 || VERSION_4_0
	case OPT_TYPE_FD:
		fd_val = luaL_checkinteger(L, 3);
		val = &fd_val;
		val_len = sizeof(fd_val);
		break;
#endif
	case OPT_TYPE_INT:
		int_val = luaL_checkinteger(L, 3);
		val = &int_val;
		val_len = sizeof(int_val);
		break;
	case OPT_TYPE_UINT32:
		uint32_val = luaL_checkinteger(L, 3);
		val = &uint32_val;
		val_len = sizeof(uint32_val);
		break;
	case OPT_TYPE_UINT64:
		uint64_val = luaL_checkinteger(L, 3);
		val = &uint64_val;
		val_len = sizeof(uint64_val);
		break;
	case OPT_TYPE_INT64:
		int64_val = luaL_checkinteger(L, 3);
		val = &int64_val;
		val_len = sizeof(int64_val);
		break;
	case OPT_TYPE_BLOB:
		val = luaL_checklstring(L, 3, &(val_len));
		break;
	default:
		printf("Invalid socket option type, this shouldn't happen.\n");
		abort();
		break;
	}
	err1 = zmq_setsockopt(this1, opt2, val, val_len);
finished:

  /* check for error. */
  if((-1 == err1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, err1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: getopt */
static int ZMQ_Socket__getopt__meth(lua_State *L) {
  ZMQ_Socket * this1;
  uint32_t opt2;
  ZMQ_Error err2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  opt2 = luaL_checkinteger(L,2);
	size_t val_len;

#if VERSION_2_1 || VERSION_3_0 || VERSION_4_0
	socket_t fd_val;
#endif
	int int_val;
	uint32_t uint32_val;
	uint64_t uint64_val;
	int64_t int64_val;
#define STR_MAX 255
	char str_val[STR_MAX];

	if(opt2 > MAX_OPTS) {
		lua_pushnil(L);
		lua_pushliteral(L, "Invalid socket option.");
		return 2;
	}

	switch(opt_types[opt2]) {
#if VERSION_2_1 || VERSION_3_0 || VERSION_4_0
	case OPT_TYPE_FD:
		val_len = sizeof(fd_val);
		err2 = zmq_getsockopt(this1, opt2, &fd_val, &val_len);
		if(0 == err2) {
			lua_pushinteger(L, (lua_Integer)fd_val);
			return 1;
		}
		break;
#endif
	case OPT_TYPE_INT:
		val_len = sizeof(int_val);
		err2 = zmq_getsockopt(this1, opt2, &int_val, &val_len);
		if(0 == err2) {
			lua_pushinteger(L, (lua_Integer)int_val);
			return 1;
		}
		break;
	case OPT_TYPE_UINT32:
		val_len = sizeof(uint32_val);
		err2 = zmq_getsockopt(this1, opt2, &uint32_val, &val_len);
		if(0 == err2) {
			lua_pushinteger(L, (lua_Integer)uint32_val);
			return 1;
		}
		break;
	case OPT_TYPE_UINT64:
		val_len = sizeof(uint64_val);
		err2 = zmq_getsockopt(this1, opt2, &uint64_val, &val_len);
		if(0 == err2) {
			lua_pushinteger(L, (lua_Integer)uint64_val);
			return 1;
		}
		break;
	case OPT_TYPE_INT64:
		val_len = sizeof(int64_val);
		err2 = zmq_getsockopt(this1, opt2, &int64_val, &val_len);
		if(0 == err2) {
			lua_pushinteger(L, (lua_Integer)int64_val);
			return 1;
		}
		break;
	case OPT_TYPE_BLOB:
		val_len = STR_MAX;
		err2 = zmq_getsockopt(this1, opt2, str_val, &val_len);
		if(0 == err2) {
			lua_pushlstring(L, str_val, val_len);
			return 1;
		}
#undef STR_MAX
		break;
	default:
		printf("Invalid socket option type, this shouldn't happen.\n");
		abort();
		break;
	}
	lua_pushnil(L);

  error_code__ZMQ_Error__push(L, err2);
  return 2;
}

/* method: send_msg */
static int ZMQ_Socket__send_msg__meth(lua_State *L) {
  ZMQ_Socket * this1;
  zmq_msg_t * msg2;
  int flags3;
  ZMQ_Error rc_zmq_sendmsg1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  msg2 = obj_type_zmq_msg_t_check(L,2);
  flags3 = luaL_optinteger(L,3,0);
  rc_zmq_sendmsg1 = zmq_sendmsg(this1, msg2, flags3);
  /* check for error. */
  if((-1 == rc_zmq_sendmsg1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_sendmsg1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: send */
static int ZMQ_Socket__send__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t data_len2;
  const char * data2;
  int flags3;
  ZMQ_Error rc_simple_zmq_send1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  data2 = luaL_checklstring(L,2,&(data_len2));
  flags3 = luaL_optinteger(L,3,0);
  rc_simple_zmq_send1 = simple_zmq_send(this1, data2, data_len2, flags3);
  /* check for error. */
  if((-1 == rc_simple_zmq_send1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_simple_zmq_send1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: recv_msg */
static int ZMQ_Socket__recv_msg__meth(lua_State *L) {
  ZMQ_Socket * this1;
  zmq_msg_t * msg2;
  int flags3;
  ZMQ_Error rc_zmq_recvmsg1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  msg2 = obj_type_zmq_msg_t_check(L,2);
  flags3 = luaL_optinteger(L,3,0);
  rc_zmq_recvmsg1 = zmq_recvmsg(this1, msg2, flags3);
  /* check for error. */
  if((-1 == rc_zmq_recvmsg1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_recvmsg1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: recv */
static int ZMQ_Socket__recv__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int flags2;
  size_t data_len1 = 0;
  const char * data1 = NULL;
  ZMQ_Error err2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  flags2 = luaL_optinteger(L,2,0);
	zmq_msg_t msg;
	/* initialize message */
	err2 = zmq_msg_init(&msg);
	if(0 == err2) {
		/* receive message */
		err2 = zmq_recvmsg(this1, &msg, flags2);
		if(err2 >= 0) {
			data1 = zmq_msg_data(&msg);
			data_len1 = zmq_msg_size(&msg);
		}
	}

  if(!(-1 == err2)) {
    if(data1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, data1,data_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, err2);
	/* close message */
	zmq_msg_close(&msg);

  return 2;
}

/* method: monitor */
#if (VERSION_3_2|VERSION_4_0)
static int ZMQ_Socket__monitor__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t addr_len2;
  const char * addr2;
  int events3;
  ZMQ_Error rc_zmq_socket_monitor1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  addr2 = luaL_checklstring(L,2,&(addr_len2));
  events3 = luaL_checkinteger(L,3);
  rc_zmq_socket_monitor1 = zmq_socket_monitor(this1, addr2, events3);
  /* check for error. */
  if((-1 == rc_zmq_socket_monitor1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_socket_monitor1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: recv_event */
#if (VERSION_3_2|VERSION_4_0)
static int ZMQ_Socket__recv_event__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int flags2;
  int event_id1 = 0;
  int value2 = 0;
  size_t addr_len3 = 0;
  const char * addr3 = NULL;
  ZMQ_Error err4 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  flags2 = luaL_optinteger(L,2,0);
	zmq_msg_t msg;
	ZMQ_recv_event event;

	/* receive monitor event */
	err4 = monitor_recv_event(this1, &msg, flags2, &event);
	if(err4 >= 0) {
		event_id1 = event.event_id;
		value2 = event.value;
		addr3 = event.addr;
		addr_len3 = event.addr_len; //err4;
	} else if(event.err != NULL) {
		/* error parsing monitor event. */
		lua_pushnil(L);
		lua_pushstring(L, event.err);
		return 2;
	}

  if(!(-1 == err4)) {
    lua_pushinteger(L, event_id1);
  } else {
    lua_pushnil(L);
  }
  if(!(-1 == err4)) {
    lua_pushinteger(L, value2);
  } else {
    lua_pushnil(L);
  }
  if(!(-1 == err4)) {
    if(addr3 == NULL) lua_pushnil(L);  else lua_pushlstring(L, addr3,addr_len3);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, err4);
	/* close message */
	zmq_msg_close(&msg);

  return 4;
}
#endif

/* method: hwm */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__hwm__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_hwm2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_hwm2 = lzmq_socket_hwm(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_hwm2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_hwm2);
  return 2;
}
#endif

/* method: set_hwm */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_hwm__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_hwm1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_hwm1 = lzmq_socket_set_hwm(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_hwm1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_hwm1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: swap */
#if (VERSION_2_0)
static int ZMQ_Socket__swap__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_swap2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_swap2 = lzmq_socket_swap(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_swap2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_swap2);
  return 2;
}
#endif

/* method: set_swap */
#if (VERSION_2_0)
static int ZMQ_Socket__set_swap__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_swap1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_swap1 = lzmq_socket_set_swap(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_swap1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_swap1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: affinity */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__affinity__meth(lua_State *L) {
  ZMQ_Socket * this1;
  uint64_t value1 = 0;
  ZMQ_Error rc_lzmq_socket_affinity2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_affinity2 = lzmq_socket_affinity(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_affinity2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_affinity2);
  return 2;
}
#endif

/* method: set_affinity */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_affinity__meth(lua_State *L) {
  ZMQ_Socket * this1;
  uint64_t value2;
  ZMQ_Error rc_lzmq_socket_set_affinity1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_affinity1 = lzmq_socket_set_affinity(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_affinity1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_affinity1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: identity */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__identity__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len1 = 0;
  char * value1 = NULL;
  ZMQ_Error rc_lzmq_socket_identity2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_identity2 = lzmq_socket_identity(this1, value1, &(value_len1));
  if(!(-1 == rc_lzmq_socket_identity2)) {
    if(value1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, value1,value_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_identity2);
  return 2;
}
#endif

/* method: set_identity */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_identity__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len2;
  const char * value2;
  ZMQ_Error rc_lzmq_socket_set_identity1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lzmq_socket_set_identity1 = lzmq_socket_set_identity(this1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_identity1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_identity1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: subscribe */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__subscribe__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len2;
  const char * value2;
  ZMQ_Error rc_lzmq_socket_subscribe1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lzmq_socket_subscribe1 = lzmq_socket_subscribe(this1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_subscribe1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_subscribe1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: unsubscribe */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__unsubscribe__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len2;
  const char * value2;
  ZMQ_Error rc_lzmq_socket_unsubscribe1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lzmq_socket_unsubscribe1 = lzmq_socket_unsubscribe(this1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_unsubscribe1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_unsubscribe1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: rate */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__rate__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_rate2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_rate2 = lzmq_socket_rate(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_rate2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_rate2);
  return 2;
}
#endif

/* method: set_rate */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_rate__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_rate1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_rate1 = lzmq_socket_set_rate(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_rate1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_rate1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: recovery_ivl */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__recovery_ivl__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_recovery_ivl2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_recovery_ivl2 = lzmq_socket_recovery_ivl(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_recovery_ivl2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_recovery_ivl2);
  return 2;
}
#endif

/* method: set_recovery_ivl */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_recovery_ivl__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_recovery_ivl1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_recovery_ivl1 = lzmq_socket_set_recovery_ivl(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_recovery_ivl1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_recovery_ivl1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: mcast_loop */
#if (VERSION_2_0)
static int ZMQ_Socket__mcast_loop__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_mcast_loop2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_mcast_loop2 = lzmq_socket_mcast_loop(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_mcast_loop2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_mcast_loop2);
  return 2;
}
#endif

/* method: set_mcast_loop */
#if (VERSION_2_0)
static int ZMQ_Socket__set_mcast_loop__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_mcast_loop1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_mcast_loop1 = lzmq_socket_set_mcast_loop(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_mcast_loop1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_mcast_loop1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: sndbuf */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__sndbuf__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_sndbuf2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_sndbuf2 = lzmq_socket_sndbuf(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_sndbuf2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_sndbuf2);
  return 2;
}
#endif

/* method: set_sndbuf */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_sndbuf__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_sndbuf1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_sndbuf1 = lzmq_socket_set_sndbuf(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_sndbuf1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_sndbuf1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: rcvbuf */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__rcvbuf__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_rcvbuf2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_rcvbuf2 = lzmq_socket_rcvbuf(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_rcvbuf2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_rcvbuf2);
  return 2;
}
#endif

/* method: set_rcvbuf */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_rcvbuf__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_rcvbuf1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_rcvbuf1 = lzmq_socket_set_rcvbuf(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_rcvbuf1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_rcvbuf1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: rcvmore */
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__rcvmore__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_rcvmore2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_rcvmore2 = lzmq_socket_rcvmore(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_rcvmore2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_rcvmore2);
  return 2;
}
#endif

/* method: fd */
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__fd__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_fd2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_fd2 = lzmq_socket_fd(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_fd2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_fd2);
  return 2;
}
#endif

/* method: events */
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__events__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_events2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_events2 = lzmq_socket_events(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_events2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_events2);
  return 2;
}
#endif

/* method: type */
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__type__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_type2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_type2 = lzmq_socket_type(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_type2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_type2);
  return 2;
}
#endif

/* method: linger */
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__linger__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_linger2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_linger2 = lzmq_socket_linger(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_linger2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_linger2);
  return 2;
}
#endif

/* method: set_linger */
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_linger__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_linger1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_linger1 = lzmq_socket_set_linger(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_linger1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_linger1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: reconnect_ivl */
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__reconnect_ivl__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_reconnect_ivl2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_reconnect_ivl2 = lzmq_socket_reconnect_ivl(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_reconnect_ivl2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_reconnect_ivl2);
  return 2;
}
#endif

/* method: set_reconnect_ivl */
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_reconnect_ivl__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_reconnect_ivl1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_reconnect_ivl1 = lzmq_socket_set_reconnect_ivl(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_reconnect_ivl1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_reconnect_ivl1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: backlog */
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__backlog__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_backlog2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_backlog2 = lzmq_socket_backlog(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_backlog2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_backlog2);
  return 2;
}
#endif

/* method: set_backlog */
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_backlog__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_backlog1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_backlog1 = lzmq_socket_set_backlog(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_backlog1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_backlog1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: recovery_ivl_msec */
#if (VERSION_2_1)
static int ZMQ_Socket__recovery_ivl_msec__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int64_t value1 = 0;
  ZMQ_Error rc_lzmq_socket_recovery_ivl_msec2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_recovery_ivl_msec2 = lzmq_socket_recovery_ivl_msec(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_recovery_ivl_msec2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_recovery_ivl_msec2);
  return 2;
}
#endif

/* method: set_recovery_ivl_msec */
#if (VERSION_2_1)
static int ZMQ_Socket__set_recovery_ivl_msec__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int64_t value2;
  ZMQ_Error rc_lzmq_socket_set_recovery_ivl_msec1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_recovery_ivl_msec1 = lzmq_socket_set_recovery_ivl_msec(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_recovery_ivl_msec1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_recovery_ivl_msec1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: reconnect_ivl_max */
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__reconnect_ivl_max__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_reconnect_ivl_max2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_reconnect_ivl_max2 = lzmq_socket_reconnect_ivl_max(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_reconnect_ivl_max2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_reconnect_ivl_max2);
  return 2;
}
#endif

/* method: set_reconnect_ivl_max */
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_reconnect_ivl_max__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_reconnect_ivl_max1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_reconnect_ivl_max1 = lzmq_socket_set_reconnect_ivl_max(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_reconnect_ivl_max1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_reconnect_ivl_max1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: maxmsgsize */
#if (VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__maxmsgsize__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int64_t value1 = 0;
  ZMQ_Error rc_lzmq_socket_maxmsgsize2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_maxmsgsize2 = lzmq_socket_maxmsgsize(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_maxmsgsize2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_maxmsgsize2);
  return 2;
}
#endif

/* method: set_maxmsgsize */
#if (VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_maxmsgsize__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int64_t value2;
  ZMQ_Error rc_lzmq_socket_set_maxmsgsize1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_maxmsgsize1 = lzmq_socket_set_maxmsgsize(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_maxmsgsize1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_maxmsgsize1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: sndhwm */
#if (VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__sndhwm__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_sndhwm2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_sndhwm2 = lzmq_socket_sndhwm(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_sndhwm2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_sndhwm2);
  return 2;
}
#endif

/* method: set_sndhwm */
#if (VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_sndhwm__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_sndhwm1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_sndhwm1 = lzmq_socket_set_sndhwm(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_sndhwm1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_sndhwm1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: rcvhwm */
#if (VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__rcvhwm__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_rcvhwm2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_rcvhwm2 = lzmq_socket_rcvhwm(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_rcvhwm2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_rcvhwm2);
  return 2;
}
#endif

/* method: set_rcvhwm */
#if (VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_rcvhwm__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_rcvhwm1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_rcvhwm1 = lzmq_socket_set_rcvhwm(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_rcvhwm1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_rcvhwm1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: multicast_hops */
#if (VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__multicast_hops__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_multicast_hops2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_multicast_hops2 = lzmq_socket_multicast_hops(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_multicast_hops2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_multicast_hops2);
  return 2;
}
#endif

/* method: set_multicast_hops */
#if (VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_multicast_hops__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_multicast_hops1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_multicast_hops1 = lzmq_socket_set_multicast_hops(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_multicast_hops1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_multicast_hops1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: rcvtimeo */
#if (VERSION_2_2|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__rcvtimeo__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_rcvtimeo2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_rcvtimeo2 = lzmq_socket_rcvtimeo(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_rcvtimeo2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_rcvtimeo2);
  return 2;
}
#endif

/* method: set_rcvtimeo */
#if (VERSION_2_2|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_rcvtimeo__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_rcvtimeo1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_rcvtimeo1 = lzmq_socket_set_rcvtimeo(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_rcvtimeo1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_rcvtimeo1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: sndtimeo */
#if (VERSION_2_2|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__sndtimeo__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_sndtimeo2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_sndtimeo2 = lzmq_socket_sndtimeo(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_sndtimeo2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_sndtimeo2);
  return 2;
}
#endif

/* method: set_sndtimeo */
#if (VERSION_2_2|VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_sndtimeo__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_sndtimeo1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_sndtimeo1 = lzmq_socket_set_sndtimeo(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_sndtimeo1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_sndtimeo1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: ipv4only */
#if (VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__ipv4only__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_ipv4only2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_ipv4only2 = lzmq_socket_ipv4only(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_ipv4only2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_ipv4only2);
  return 2;
}
#endif

/* method: set_ipv4only */
#if (VERSION_3_0|VERSION_4_0)
static int ZMQ_Socket__set_ipv4only__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_ipv4only1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_ipv4only1 = lzmq_socket_set_ipv4only(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_ipv4only1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_ipv4only1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: last_endpoint */
#if (VERSION_4_0)
static int ZMQ_Socket__last_endpoint__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len1 = 0;
  char * value1 = NULL;
  ZMQ_Error rc_lzmq_socket_last_endpoint2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_last_endpoint2 = lzmq_socket_last_endpoint(this1, value1, &(value_len1));
  if(!(-1 == rc_lzmq_socket_last_endpoint2)) {
    if(value1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, value1,value_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_last_endpoint2);
  return 2;
}
#endif

/* method: router_mandatory */
#if (VERSION_4_0)
static int ZMQ_Socket__router_mandatory__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_router_mandatory1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_router_mandatory1 = lzmq_socket_router_mandatory(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_router_mandatory1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_router_mandatory1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: tcp_keepalive */
#if (VERSION_4_0)
static int ZMQ_Socket__tcp_keepalive__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_tcp_keepalive2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_tcp_keepalive2 = lzmq_socket_tcp_keepalive(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_tcp_keepalive2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_tcp_keepalive2);
  return 2;
}
#endif

/* method: set_tcp_keepalive */
#if (VERSION_4_0)
static int ZMQ_Socket__set_tcp_keepalive__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_tcp_keepalive1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_tcp_keepalive1 = lzmq_socket_set_tcp_keepalive(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_tcp_keepalive1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_tcp_keepalive1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: tcp_keepalive_cnt */
#if (VERSION_4_0)
static int ZMQ_Socket__tcp_keepalive_cnt__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_tcp_keepalive_cnt2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_tcp_keepalive_cnt2 = lzmq_socket_tcp_keepalive_cnt(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_tcp_keepalive_cnt2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_tcp_keepalive_cnt2);
  return 2;
}
#endif

/* method: set_tcp_keepalive_cnt */
#if (VERSION_4_0)
static int ZMQ_Socket__set_tcp_keepalive_cnt__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_tcp_keepalive_cnt1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_tcp_keepalive_cnt1 = lzmq_socket_set_tcp_keepalive_cnt(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_tcp_keepalive_cnt1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_tcp_keepalive_cnt1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: tcp_keepalive_idle */
#if (VERSION_4_0)
static int ZMQ_Socket__tcp_keepalive_idle__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_tcp_keepalive_idle2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_tcp_keepalive_idle2 = lzmq_socket_tcp_keepalive_idle(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_tcp_keepalive_idle2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_tcp_keepalive_idle2);
  return 2;
}
#endif

/* method: set_tcp_keepalive_idle */
#if (VERSION_4_0)
static int ZMQ_Socket__set_tcp_keepalive_idle__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_tcp_keepalive_idle1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_tcp_keepalive_idle1 = lzmq_socket_set_tcp_keepalive_idle(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_tcp_keepalive_idle1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_tcp_keepalive_idle1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: tcp_keepalive_intvl */
#if (VERSION_4_0)
static int ZMQ_Socket__tcp_keepalive_intvl__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_tcp_keepalive_intvl2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_tcp_keepalive_intvl2 = lzmq_socket_tcp_keepalive_intvl(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_tcp_keepalive_intvl2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_tcp_keepalive_intvl2);
  return 2;
}
#endif

/* method: set_tcp_keepalive_intvl */
#if (VERSION_4_0)
static int ZMQ_Socket__set_tcp_keepalive_intvl__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_tcp_keepalive_intvl1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_tcp_keepalive_intvl1 = lzmq_socket_set_tcp_keepalive_intvl(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_tcp_keepalive_intvl1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_tcp_keepalive_intvl1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: tcp_accept_filter */
#if (VERSION_4_0)
static int ZMQ_Socket__tcp_accept_filter__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len2;
  const char * value2;
  ZMQ_Error rc_lzmq_socket_tcp_accept_filter1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lzmq_socket_tcp_accept_filter1 = lzmq_socket_tcp_accept_filter(this1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_tcp_accept_filter1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_tcp_accept_filter1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: immediate */
#if (VERSION_4_0)
static int ZMQ_Socket__immediate__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_immediate2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_immediate2 = lzmq_socket_immediate(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_immediate2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_immediate2);
  return 2;
}
#endif

/* method: set_immediate */
#if (VERSION_4_0)
static int ZMQ_Socket__set_immediate__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_immediate1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_immediate1 = lzmq_socket_set_immediate(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_immediate1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_immediate1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: xpub_verbose */
#if (VERSION_4_0)
static int ZMQ_Socket__xpub_verbose__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_xpub_verbose1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_xpub_verbose1 = lzmq_socket_xpub_verbose(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_xpub_verbose1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_xpub_verbose1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: router_raw */
#if (VERSION_4_0)
static int ZMQ_Socket__router_raw__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_router_raw1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_router_raw1 = lzmq_socket_router_raw(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_router_raw1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_router_raw1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: ipv6 */
#if (VERSION_4_0)
static int ZMQ_Socket__ipv6__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_ipv62 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_ipv62 = lzmq_socket_ipv6(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_ipv62)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_ipv62);
  return 2;
}
#endif

/* method: set_ipv6 */
#if (VERSION_4_0)
static int ZMQ_Socket__set_ipv6__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_ipv61 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_ipv61 = lzmq_socket_set_ipv6(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_ipv61)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_ipv61);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: mechanism */
#if (VERSION_4_0)
static int ZMQ_Socket__mechanism__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_mechanism2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_mechanism2 = lzmq_socket_mechanism(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_mechanism2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_mechanism2);
  return 2;
}
#endif

/* method: plain_server */
#if (VERSION_4_0)
static int ZMQ_Socket__plain_server__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_plain_server2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_plain_server2 = lzmq_socket_plain_server(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_plain_server2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_plain_server2);
  return 2;
}
#endif

/* method: set_plain_server */
#if (VERSION_4_0)
static int ZMQ_Socket__set_plain_server__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_plain_server1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_plain_server1 = lzmq_socket_set_plain_server(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_plain_server1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_plain_server1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: plain_username */
#if (VERSION_4_0)
static int ZMQ_Socket__plain_username__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len1 = 0;
  char * value1 = NULL;
  ZMQ_Error rc_lzmq_socket_plain_username2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_plain_username2 = lzmq_socket_plain_username(this1, value1, &(value_len1));
  if(!(-1 == rc_lzmq_socket_plain_username2)) {
    if(value1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, value1,value_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_plain_username2);
  return 2;
}
#endif

/* method: set_plain_username */
#if (VERSION_4_0)
static int ZMQ_Socket__set_plain_username__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len2;
  const char * value2;
  ZMQ_Error rc_lzmq_socket_set_plain_username1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lzmq_socket_set_plain_username1 = lzmq_socket_set_plain_username(this1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_plain_username1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_plain_username1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: plain_password */
#if (VERSION_4_0)
static int ZMQ_Socket__plain_password__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len1 = 0;
  char * value1 = NULL;
  ZMQ_Error rc_lzmq_socket_plain_password2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_plain_password2 = lzmq_socket_plain_password(this1, value1, &(value_len1));
  if(!(-1 == rc_lzmq_socket_plain_password2)) {
    if(value1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, value1,value_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_plain_password2);
  return 2;
}
#endif

/* method: set_plain_password */
#if (VERSION_4_0)
static int ZMQ_Socket__set_plain_password__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len2;
  const char * value2;
  ZMQ_Error rc_lzmq_socket_set_plain_password1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lzmq_socket_set_plain_password1 = lzmq_socket_set_plain_password(this1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_plain_password1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_plain_password1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: curve_server */
#if (VERSION_4_0)
static int ZMQ_Socket__curve_server__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_curve_server2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_curve_server2 = lzmq_socket_curve_server(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_curve_server2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_curve_server2);
  return 2;
}
#endif

/* method: set_curve_server */
#if (VERSION_4_0)
static int ZMQ_Socket__set_curve_server__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_curve_server1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_curve_server1 = lzmq_socket_set_curve_server(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_curve_server1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_curve_server1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: curve_publickey */
#if (VERSION_4_0)
static int ZMQ_Socket__curve_publickey__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len1 = 0;
  char * value1 = NULL;
  ZMQ_Error rc_lzmq_socket_curve_publickey2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_curve_publickey2 = lzmq_socket_curve_publickey(this1, value1, &(value_len1));
  if(!(-1 == rc_lzmq_socket_curve_publickey2)) {
    if(value1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, value1,value_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_curve_publickey2);
  return 2;
}
#endif

/* method: set_curve_publickey */
#if (VERSION_4_0)
static int ZMQ_Socket__set_curve_publickey__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len2;
  const char * value2;
  ZMQ_Error rc_lzmq_socket_set_curve_publickey1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lzmq_socket_set_curve_publickey1 = lzmq_socket_set_curve_publickey(this1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_curve_publickey1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_curve_publickey1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: curve_secretkey */
#if (VERSION_4_0)
static int ZMQ_Socket__curve_secretkey__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len1 = 0;
  char * value1 = NULL;
  ZMQ_Error rc_lzmq_socket_curve_secretkey2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_curve_secretkey2 = lzmq_socket_curve_secretkey(this1, value1, &(value_len1));
  if(!(-1 == rc_lzmq_socket_curve_secretkey2)) {
    if(value1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, value1,value_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_curve_secretkey2);
  return 2;
}
#endif

/* method: set_curve_secretkey */
#if (VERSION_4_0)
static int ZMQ_Socket__set_curve_secretkey__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len2;
  const char * value2;
  ZMQ_Error rc_lzmq_socket_set_curve_secretkey1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lzmq_socket_set_curve_secretkey1 = lzmq_socket_set_curve_secretkey(this1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_curve_secretkey1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_curve_secretkey1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: curve_serverkey */
#if (VERSION_4_0)
static int ZMQ_Socket__curve_serverkey__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len1 = 0;
  char * value1 = NULL;
  ZMQ_Error rc_lzmq_socket_curve_serverkey2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_curve_serverkey2 = lzmq_socket_curve_serverkey(this1, value1, &(value_len1));
  if(!(-1 == rc_lzmq_socket_curve_serverkey2)) {
    if(value1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, value1,value_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_curve_serverkey2);
  return 2;
}
#endif

/* method: set_curve_serverkey */
#if (VERSION_4_0)
static int ZMQ_Socket__set_curve_serverkey__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len2;
  const char * value2;
  ZMQ_Error rc_lzmq_socket_set_curve_serverkey1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lzmq_socket_set_curve_serverkey1 = lzmq_socket_set_curve_serverkey(this1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_curve_serverkey1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_curve_serverkey1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: probe_router */
#if (VERSION_4_0)
static int ZMQ_Socket__probe_router__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_probe_router1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_probe_router1 = lzmq_socket_probe_router(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_probe_router1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_probe_router1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: req_correlate */
#if (VERSION_4_0)
static int ZMQ_Socket__req_correlate__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_req_correlate1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_req_correlate1 = lzmq_socket_req_correlate(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_req_correlate1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_req_correlate1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: req_relaxed */
#if (VERSION_4_0)
static int ZMQ_Socket__req_relaxed__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_req_relaxed1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_req_relaxed1 = lzmq_socket_req_relaxed(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_req_relaxed1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_req_relaxed1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: conflate */
#if (VERSION_4_0)
static int ZMQ_Socket__conflate__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value1 = 0;
  ZMQ_Error rc_lzmq_socket_conflate2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_conflate2 = lzmq_socket_conflate(this1, &(value1));
  if(!(-1 == rc_lzmq_socket_conflate2)) {
    lua_pushinteger(L, value1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_conflate2);
  return 2;
}
#endif

/* method: set_conflate */
#if (VERSION_4_0)
static int ZMQ_Socket__set_conflate__meth(lua_State *L) {
  ZMQ_Socket * this1;
  int value2;
  ZMQ_Error rc_lzmq_socket_set_conflate1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checkinteger(L,2);
  rc_lzmq_socket_set_conflate1 = lzmq_socket_set_conflate(this1, value2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_conflate1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_conflate1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: zap_domain */
#if (VERSION_4_0)
static int ZMQ_Socket__zap_domain__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len1 = 0;
  char * value1 = NULL;
  ZMQ_Error rc_lzmq_socket_zap_domain2 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  rc_lzmq_socket_zap_domain2 = lzmq_socket_zap_domain(this1, value1, &(value_len1));
  if(!(-1 == rc_lzmq_socket_zap_domain2)) {
    if(value1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, value1,value_len1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, rc_lzmq_socket_zap_domain2);
  return 2;
}
#endif

/* method: set_zap_domain */
#if (VERSION_4_0)
static int ZMQ_Socket__set_zap_domain__meth(lua_State *L) {
  ZMQ_Socket * this1;
  size_t value_len2;
  const char * value2;
  ZMQ_Error rc_lzmq_socket_set_zap_domain1 = 0;
  this1 = obj_type_ZMQ_Socket_check(L,1);
  value2 = luaL_checklstring(L,2,&(value_len2));
  rc_lzmq_socket_set_zap_domain1 = lzmq_socket_set_zap_domain(this1, value2, value_len2);
  /* check for error. */
  if((-1 == rc_lzmq_socket_set_zap_domain1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_lzmq_socket_set_zap_domain1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: new */
static int ZMQ_Poller__new__meth(lua_State *L) {
  unsigned int length1;
  ZMQ_Poller this1_store;
  ZMQ_Poller * this1 = &(this1_store);
  length1 = luaL_optinteger(L,1,10);
  poller_init(this1, length1);
  obj_type_ZMQ_Poller_push(L, this1);
  return 1;
}

/* method: close */
static int ZMQ_Poller__close__meth(lua_State *L) {
  ZMQ_Poller * this1;
  this1 = obj_type_ZMQ_Poller_delete(L,1);
  poller_cleanup(this1);
  return 0;
}

/* method: add */
static int ZMQ_Poller__add__meth(lua_State *L) {
  ZMQ_Poller * this1;
  short events3;
  int idx1 = 0;
	zmq_pollitem_t *item;
	ZMQ_Socket *sock = NULL;
	socket_t fd = 0;

  this1 = obj_type_ZMQ_Poller_check(L,1);
  events3 = luaL_checkinteger(L,3);
	if(lua_isuserdata(L, 2)) {
		sock = obj_type_ZMQ_Socket_check(L, 2);
	} else if(lua_isnumber(L, 2)) {
		fd = lua_tonumber(L, 2);
	} else {
		return luaL_typerror(L, 2, "number or ZMQ_Socket");
	}
	idx1 = poller_get_free_item(this1);
	item = &(this1->items[idx1]);
	item->socket = sock;
	item->fd = fd;
	item->events = events3;

  lua_pushinteger(L, idx1);
  return 1;
}

/* method: modify */
static int ZMQ_Poller__modify__meth(lua_State *L) {
  ZMQ_Poller * this1;
  short events3;
  int idx1 = 0;
	zmq_pollitem_t *item;
	ZMQ_Socket *sock = NULL;
	socket_t fd = 0;

  this1 = obj_type_ZMQ_Poller_check(L,1);
  events3 = luaL_checkinteger(L,3);
	if(lua_isuserdata(L, 2)) {
		sock = obj_type_ZMQ_Socket_check(L, 2);
		/* find sock in items list. */
		idx1 = poller_find_sock_item(this1, sock);
	} else if(lua_isnumber(L, 2)) {
		fd = lua_tonumber(L, 2);
		/* find fd in items list. */
		idx1 = poller_find_fd_item(this1, fd);
	} else {
		return luaL_typerror(L, 2, "number or ZMQ_Socket");
	}
	if(events3 != 0) {
		/* add/modify. */
		if(idx1 < 0) {
			idx1 = poller_get_free_item(this1);
		}
		item = &(this1->items[idx1]);
		item->socket = sock;
		item->fd = fd;
		item->events = events3;
	} else if(idx1 >= 0) {
		/* no events remove socket/fd. */
		poller_remove_item(this1, idx1);
	}

  lua_pushinteger(L, idx1);
  return 1;
}

/* method: remove */
static int ZMQ_Poller__remove__meth(lua_State *L) {
  ZMQ_Poller * this1;
  int idx1 = 0;
	ZMQ_Socket *sock;
	socket_t fd;

  this1 = obj_type_ZMQ_Poller_check(L,1);
	/* ZMQ_Socket or fd */
	if(lua_isuserdata(L, 2)) {
		sock = obj_type_ZMQ_Socket_check(L, 2);
		/* find sock in items list. */
		idx1 = poller_find_sock_item(this1, sock);
	} else if(lua_isnumber(L, 2)) {
		fd = lua_tonumber(L, 2);
		/* find fd in items list. */
		idx1 = poller_find_fd_item(this1, fd);
	} else {
		return luaL_typerror(L, 2, "number or ZMQ_Socket");
	}
	/* if sock/fd was found. */
	if(idx1 >= 0) {
		poller_remove_item(this1, idx1);
	}

  lua_pushinteger(L, idx1);
  return 1;
}

/* method: poll */
static int ZMQ_Poller__poll__meth(lua_State *L) {
  ZMQ_Poller * this1;
  long timeout2;
  int count1 = 0;
  ZMQ_Error err2 = 0;
  this1 = obj_type_ZMQ_Poller_check(L,1);
  timeout2 = luaL_checkinteger(L,2);
  err2 = poller_poll(this1, timeout2);
	if(err2 > 0) {
		this1->next = 0;
		count1 = err2;
	} else {
		this1->next = -1;
		count1 = 0;
	}

  if(!(-1 == err2)) {
    lua_pushinteger(L, count1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, err2);
  return 2;
}

/* method: next_revents_idx */
static int ZMQ_Poller__next_revents_idx__meth(lua_State *L) {
  ZMQ_Poller * this1;
  int idx1 = 0;
  int revents2 = 0;
  this1 = obj_type_ZMQ_Poller_check(L,1);
  idx1 = poller_next_revents(this1, &(revents2));
  lua_pushinteger(L, idx1);
  lua_pushinteger(L, revents2);
  return 2;
}

/* method: count */
static int ZMQ_Poller__count__meth(lua_State *L) {
  ZMQ_Poller * this1;
  int count1 = 0;
  this1 = obj_type_ZMQ_Poller_check(L,1);
	count1 = this1->count;

  lua_pushinteger(L, count1);
  return 1;
}

/* method: term */
static int ZMQ_Ctx__term__meth(lua_State *L) {
  int this_flags1 = 0;
  ZMQ_Ctx * this1;
  ZMQ_Error rc_zmq_term1 = 0;
  this1 = obj_type_ZMQ_Ctx_delete(L,1,&(this_flags1));
  if(!(this_flags1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  rc_zmq_term1 = zmq_term(this1);
  /* check for error. */
  if((-1 == rc_zmq_term1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_term1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: lightuserdata */
static int ZMQ_Ctx__lightuserdata__meth(lua_State *L) {
  ZMQ_Ctx * this1;
  void * ptr1 = NULL;
  this1 = obj_type_ZMQ_Ctx_check(L,1);
	ptr1 = this1;

  lua_pushlightuserdata(L, ptr1);
  return 1;
}

/* method: socket */
static int ZMQ_Ctx__socket__meth(lua_State *L) {
  ZMQ_Ctx * this1;
  int type2;
  int rc_zmq_socket_flags1 = OBJ_UDATA_FLAG_OWN;
  ZMQ_Socket * rc_zmq_socket1;
  this1 = obj_type_ZMQ_Ctx_check(L,1);
  type2 = luaL_checkinteger(L,2);
  rc_zmq_socket1 = zmq_socket(this1, type2);
  if((NULL == rc_zmq_socket1)) {
    lua_pushnil(L);
    lua_pushstring(L, get_zmq_strerror());
    return 2;
  } else {
    obj_type_ZMQ_Socket_push(L, rc_zmq_socket1, rc_zmq_socket_flags1);
  }
  return 1;
}

/* method: set */
#if (VERSION_3_2|VERSION_4_0)
static int ZMQ_Ctx__set__meth(lua_State *L) {
  ZMQ_Ctx * this1;
  int flag2;
  int value3;
  int rc_zmq_ctx_set1 = 0;
  this1 = obj_type_ZMQ_Ctx_check(L,1);
  flag2 = luaL_checkinteger(L,2);
  value3 = luaL_checkinteger(L,3);
  rc_zmq_ctx_set1 = zmq_ctx_set(this1, flag2, value3);
  lua_pushinteger(L, rc_zmq_ctx_set1);
  return 1;
}
#endif

/* method: get */
#if (VERSION_3_2|VERSION_4_0)
static int ZMQ_Ctx__get__meth(lua_State *L) {
  ZMQ_Ctx * this1;
  int flag2;
  int rc_zmq_ctx_get1 = 0;
  this1 = obj_type_ZMQ_Ctx_check(L,1);
  flag2 = luaL_checkinteger(L,2);
  rc_zmq_ctx_get1 = zmq_ctx_get(this1, flag2);
  lua_pushinteger(L, rc_zmq_ctx_get1);
  return 1;
}
#endif

/* method: start */
static int ZMQ_StopWatch__start__meth(lua_State *L) {
  int this_flags1 = OBJ_UDATA_FLAG_OWN;
  ZMQ_StopWatch * this1;
  this1 = zmq_stopwatch_start();
  obj_type_ZMQ_StopWatch_push(L, this1, this_flags1);
  return 1;
}

/* method: stop */
static int ZMQ_StopWatch__stop__meth(lua_State *L) {
  int this_flags1 = 0;
  ZMQ_StopWatch * this1;
  unsigned long usecs1 = 0;
  this1 = obj_type_ZMQ_StopWatch_delete(L,1,&(this_flags1));
  if(!(this_flags1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
  usecs1 = zmq_stopwatch_stop(this1);
  lua_pushinteger(L, usecs1);
  return 1;
}

/* method: version */
static int zmq__version__func(lua_State *L) {
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

  return 1;
}

/* method: init */
static int zmq__init__func(lua_State *L) {
  int io_threads1;
  int rc_zmq_init_flags1 = OBJ_UDATA_FLAG_OWN;
  ZMQ_Ctx * rc_zmq_init1;
  io_threads1 = luaL_optinteger(L,1,1);
  rc_zmq_init1 = zmq_init(io_threads1);
  if((NULL == rc_zmq_init1)) {
    lua_pushnil(L);
    lua_pushstring(L, get_zmq_strerror());
    return 2;
  } else {
    obj_type_ZMQ_Ctx_push(L, rc_zmq_init1, rc_zmq_init_flags1);
  }
  return 1;
}

/* method: init_ctx */
static int zmq__init_ctx__func(lua_State *L) {
  ZMQ_Ctx * ctx1;
	if(lua_isuserdata(L, 1)) {
		ctx1 = lua_touserdata(L, 1);
	} else {
		return luaL_argerror(L, 1, "expected lightuserdata");
	}

  if((NULL == ctx1)) {
    lua_pushnil(L);
    lua_pushstring(L, get_zmq_strerror());
    return 2;
  } else {
    obj_type_ZMQ_Ctx_push(L, ctx1, 0);
  }
  return 1;
}

/* method: device */
#if (VERSION_2_0|VERSION_3_2)
static int zmq__device__func(lua_State *L) {
  int device1;
  ZMQ_Socket * insock2;
  ZMQ_Socket * outsock3;
  ZMQ_Error rc_zmq_device1 = 0;
  device1 = luaL_checkinteger(L,1);
  insock2 = obj_type_ZMQ_Socket_check(L,2);
  outsock3 = obj_type_ZMQ_Socket_check(L,3);
  rc_zmq_device1 = zmq_device(device1, insock2, outsock3);
  /* check for error. */
  if((-1 == rc_zmq_device1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_device1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: proxy */
#if (VERSION_3_2)
static int zmq__proxy__func(lua_State *L) {
  ZMQ_Socket * frontend1;
  ZMQ_Socket * backend2;
  ZMQ_Socket * capture3;
  ZMQ_Error rc_zmq_proxy1 = 0;
  frontend1 = obj_type_ZMQ_Socket_check(L,1);
  backend2 = obj_type_ZMQ_Socket_check(L,2);
  capture3 = obj_type_ZMQ_Socket_optional(L,3);
  rc_zmq_proxy1 = zmq_proxy(frontend1, backend2, capture3);
  /* check for error. */
  if((-1 == rc_zmq_proxy1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_proxy1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}
#endif

/* method: stopwatch_start */
static int zmq__stopwatch_start__func(lua_State *L) {
  int rc_zmq_stopwatch_start_flags1 = OBJ_UDATA_FLAG_OWN;
  ZMQ_StopWatch * rc_zmq_stopwatch_start1;
  rc_zmq_stopwatch_start1 = zmq_stopwatch_start();
  obj_type_ZMQ_StopWatch_push(L, rc_zmq_stopwatch_start1, rc_zmq_stopwatch_start_flags1);
  return 1;
}

/* method: sleep */
static int zmq__sleep__func(lua_State *L) {
  int seconds_1;
  seconds_1 = luaL_checkinteger(L,1);
  zmq_sleep(seconds_1);
  return 0;
}



static const luaL_Reg obj_ZErrors_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_ZErrors_methods[] = {
  {"description", ZErrors__description__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_ZErrors_metas[] = {
  {"__index", ZErrors____index__meth},
  {NULL, NULL}
};

static const obj_const obj_ZErrors_constants[] = {
#ifdef ENOCSI
  {"ENOCSI", NULL, ENOCSI, CONST_NUMBER},
#endif
#ifdef ECONNREFUSED
  {"ECONNREFUSED", NULL, ECONNREFUSED, CONST_NUMBER},
#endif
#ifdef ENOPKG
  {"ENOPKG", NULL, ENOPKG, CONST_NUMBER},
#endif
#ifdef ENOTCONN
  {"ENOTCONN", NULL, ENOTCONN, CONST_NUMBER},
#endif
#ifdef ENODATA
  {"ENODATA", NULL, ENODATA, CONST_NUMBER},
#endif
#ifdef ETOOMANYREFS
  {"ETOOMANYREFS", NULL, ETOOMANYREFS, CONST_NUMBER},
#endif
#ifdef EADDRINUSE
  {"EADDRINUSE", NULL, EADDRINUSE, CONST_NUMBER},
#endif
#ifdef EL3HLT
  {"EL3HLT", NULL, EL3HLT, CONST_NUMBER},
#endif
#ifdef EPROTO
  {"EPROTO", NULL, EPROTO, CONST_NUMBER},
#endif
#ifdef ECHILD
  {"ECHILD", NULL, ECHILD, CONST_NUMBER},
#endif
#ifdef EPFNOSUPPORT
  {"EPFNOSUPPORT", NULL, EPFNOSUPPORT, CONST_NUMBER},
#endif
#ifdef ECHRNG
  {"ECHRNG", NULL, ECHRNG, CONST_NUMBER},
#endif
#ifdef ENOBUFS
  {"ENOBUFS", NULL, ENOBUFS, CONST_NUMBER},
#endif
#ifdef ENOMEM
  {"ENOMEM", NULL, ENOMEM, CONST_NUMBER},
#endif
#ifdef EDEADLOCK
  {"EDEADLOCK", NULL, EDEADLOCK, CONST_NUMBER},
#endif
#ifdef ETXTBSY
  {"ETXTBSY", NULL, ETXTBSY, CONST_NUMBER},
#endif
#ifdef EBUSY
  {"EBUSY", NULL, EBUSY, CONST_NUMBER},
#endif
#ifdef EDOM
  {"EDOM", NULL, EDOM, CONST_NUMBER},
#endif
#ifdef EPROTONOSUPPORT
  {"EPROTONOSUPPORT", NULL, EPROTONOSUPPORT, CONST_NUMBER},
#endif
#ifdef EMULTIHOP
  {"EMULTIHOP", NULL, EMULTIHOP, CONST_NUMBER},
#endif
#ifdef ENAMETOOLONG
  {"ENAMETOOLONG", NULL, ENAMETOOLONG, CONST_NUMBER},
#endif
#ifdef EMEDIUMTYPE
  {"EMEDIUMTYPE", NULL, EMEDIUMTYPE, CONST_NUMBER},
#endif
#ifdef EIO
  {"EIO", NULL, EIO, CONST_NUMBER},
#endif
#ifdef ELOOP
  {"ELOOP", NULL, ELOOP, CONST_NUMBER},
#endif
#ifdef EMLINK
  {"EMLINK", NULL, EMLINK, CONST_NUMBER},
#endif
#ifdef ESOCKTNOSUPPORT
  {"ESOCKTNOSUPPORT", NULL, ESOCKTNOSUPPORT, CONST_NUMBER},
#endif
#ifdef EDOTDOT
  {"EDOTDOT", NULL, EDOTDOT, CONST_NUMBER},
#endif
#ifdef EREMCHG
  {"EREMCHG", NULL, EREMCHG, CONST_NUMBER},
#endif
#ifdef EDESTADDRREQ
  {"EDESTADDRREQ", NULL, EDESTADDRREQ, CONST_NUMBER},
#endif
#ifdef ENETRESET
  {"ENETRESET", NULL, ENETRESET, CONST_NUMBER},
#endif
#ifdef ENOKEY
  {"ENOKEY", NULL, ENOKEY, CONST_NUMBER},
#endif
#ifdef EROFS
  {"EROFS", NULL, EROFS, CONST_NUMBER},
#endif
#ifdef ENOSR
  {"ENOSR", NULL, ENOSR, CONST_NUMBER},
#endif
#ifdef ESRMNT
  {"ESRMNT", NULL, ESRMNT, CONST_NUMBER},
#endif
#ifdef EBADFD
  {"EBADFD", NULL, EBADFD, CONST_NUMBER},
#endif
#ifdef ENOTRECOVERABLE
  {"ENOTRECOVERABLE", NULL, ENOTRECOVERABLE, CONST_NUMBER},
#endif
#ifdef EWOULDBLOCK
  {"EWOULDBLOCK", NULL, EWOULDBLOCK, CONST_NUMBER},
#endif
#ifdef EPERM
  {"EPERM", NULL, EPERM, CONST_NUMBER},
#endif
#ifdef EL3RST
  {"EL3RST", NULL, EL3RST, CONST_NUMBER},
#endif
#ifdef ENOLINK
  {"ENOLINK", NULL, ENOLINK, CONST_NUMBER},
#endif
#ifdef EAGAIN
  {"EAGAIN", NULL, EAGAIN, CONST_NUMBER},
#endif
#ifdef ESTALE
  {"ESTALE", NULL, ESTALE, CONST_NUMBER},
#endif
#ifdef EMFILE
  {"EMFILE", NULL, EMFILE, CONST_NUMBER},
#endif
#ifdef ENOTTY
  {"ENOTTY", NULL, ENOTTY, CONST_NUMBER},
#endif
#ifdef EIDRM
  {"EIDRM", NULL, EIDRM, CONST_NUMBER},
#endif
#ifdef EPIPE
  {"EPIPE", NULL, EPIPE, CONST_NUMBER},
#endif
#ifdef ENOEXEC
  {"ENOEXEC", NULL, ENOEXEC, CONST_NUMBER},
#endif
#ifdef ELIBEXEC
  {"ELIBEXEC", NULL, ELIBEXEC, CONST_NUMBER},
#endif
#ifdef ESTRPIPE
  {"ESTRPIPE", NULL, ESTRPIPE, CONST_NUMBER},
#endif
#ifdef ENETUNREACH
  {"ENETUNREACH", NULL, ENETUNREACH, CONST_NUMBER},
#endif
#ifdef ESHUTDOWN
  {"ESHUTDOWN", NULL, ESHUTDOWN, CONST_NUMBER},
#endif
#ifdef EBADSLT
  {"EBADSLT", NULL, EBADSLT, CONST_NUMBER},
#endif
#ifdef EBADR
  {"EBADR", NULL, EBADR, CONST_NUMBER},
#endif
#ifdef EOPNOTSUPP
  {"EOPNOTSUPP", NULL, EOPNOTSUPP, CONST_NUMBER},
#endif
#ifdef EISCONN
  {"EISCONN", NULL, EISCONN, CONST_NUMBER},
#endif
#ifdef ERFKILL
  {"ERFKILL", NULL, ERFKILL, CONST_NUMBER},
#endif
#ifdef ENXIO
  {"ENXIO", NULL, ENXIO, CONST_NUMBER},
#endif
#ifdef EINPROGRESS
  {"EINPROGRESS", NULL, EINPROGRESS, CONST_NUMBER},
#endif
#ifdef ENOANO
  {"ENOANO", NULL, ENOANO, CONST_NUMBER},
#endif
#ifdef EBADF
  {"EBADF", NULL, EBADF, CONST_NUMBER},
#endif
#ifdef EILSEQ
  {"EILSEQ", NULL, EILSEQ, CONST_NUMBER},
#endif
#ifdef ENOTSOCK
  {"ENOTSOCK", NULL, ENOTSOCK, CONST_NUMBER},
#endif
#ifdef ESRCH
  {"ESRCH", NULL, ESRCH, CONST_NUMBER},
#endif
#ifdef ETIMEDOUT
  {"ETIMEDOUT", NULL, ETIMEDOUT, CONST_NUMBER},
#endif
#ifdef ELIBACC
  {"ELIBACC", NULL, ELIBACC, CONST_NUMBER},
#endif
#ifdef ENOMSG
  {"ENOMSG", NULL, ENOMSG, CONST_NUMBER},
#endif
#ifdef ENOSTR
  {"ENOSTR", NULL, ENOSTR, CONST_NUMBER},
#endif
#ifdef ENOSPC
  {"ENOSPC", NULL, ENOSPC, CONST_NUMBER},
#endif
#ifdef ECONNABORTED
  {"ECONNABORTED", NULL, ECONNABORTED, CONST_NUMBER},
#endif
#ifdef ETIME
  {"ETIME", NULL, ETIME, CONST_NUMBER},
#endif
#ifdef ENOTBLK
  {"ENOTBLK", NULL, ENOTBLK, CONST_NUMBER},
#endif
#ifdef E2BIG
  {"E2BIG", NULL, E2BIG, CONST_NUMBER},
#endif
#ifdef ECOMM
  {"ECOMM", NULL, ECOMM, CONST_NUMBER},
#endif
#ifdef EUSERS
  {"EUSERS", NULL, EUSERS, CONST_NUMBER},
#endif
#ifdef ETERM
  {"ETERM", NULL, ETERM, CONST_NUMBER},
#endif
#ifdef ENOMEDIUM
  {"ENOMEDIUM", NULL, ENOMEDIUM, CONST_NUMBER},
#endif
#ifdef ENOPROTOOPT
  {"ENOPROTOOPT", NULL, ENOPROTOOPT, CONST_NUMBER},
#endif
#ifdef ENODEV
  {"ENODEV", NULL, ENODEV, CONST_NUMBER},
#endif
#ifdef EXDEV
  {"EXDEV", NULL, EXDEV, CONST_NUMBER},
#endif
#ifdef ENOSYS
  {"ENOSYS", NULL, ENOSYS, CONST_NUMBER},
#endif
#ifdef EFAULT
  {"EFAULT", NULL, EFAULT, CONST_NUMBER},
#endif
#ifdef EL2NSYNC
  {"EL2NSYNC", NULL, EL2NSYNC, CONST_NUMBER},
#endif
#ifdef EMTHREAD
  {"EMTHREAD", NULL, EMTHREAD, CONST_NUMBER},
#endif
#ifdef ENETDOWN
  {"ENETDOWN", NULL, ENETDOWN, CONST_NUMBER},
#endif
#ifdef ERESTART
  {"ERESTART", NULL, ERESTART, CONST_NUMBER},
#endif
#ifdef EBADMSG
  {"EBADMSG", NULL, EBADMSG, CONST_NUMBER},
#endif
#ifdef EHOSTDOWN
  {"EHOSTDOWN", NULL, EHOSTDOWN, CONST_NUMBER},
#endif
#ifdef EINTR
  {"EINTR", NULL, EINTR, CONST_NUMBER},
#endif
#ifdef EOVERFLOW
  {"EOVERFLOW", NULL, EOVERFLOW, CONST_NUMBER},
#endif
#ifdef EADV
  {"EADV", NULL, EADV, CONST_NUMBER},
#endif
#ifdef ELNRNG
  {"ELNRNG", NULL, ELNRNG, CONST_NUMBER},
#endif
#ifdef ELIBBAD
  {"ELIBBAD", NULL, ELIBBAD, CONST_NUMBER},
#endif
#ifdef ERANGE
  {"ERANGE", NULL, ERANGE, CONST_NUMBER},
#endif
#ifdef EBADRQC
  {"EBADRQC", NULL, EBADRQC, CONST_NUMBER},
#endif
#ifdef ENOLCK
  {"ENOLCK", NULL, ENOLCK, CONST_NUMBER},
#endif
#ifdef EEXIST
  {"EEXIST", NULL, EEXIST, CONST_NUMBER},
#endif
#ifdef ECONNRESET
  {"ECONNRESET", NULL, ECONNRESET, CONST_NUMBER},
#endif
#ifdef EHOSTUNREACH
  {"EHOSTUNREACH", NULL, EHOSTUNREACH, CONST_NUMBER},
#endif
#ifdef ENOTUNIQ
  {"ENOTUNIQ", NULL, ENOTUNIQ, CONST_NUMBER},
#endif
#ifdef EKEYREJECTED
  {"EKEYREJECTED", NULL, EKEYREJECTED, CONST_NUMBER},
#endif
#ifdef EKEYREVOKED
  {"EKEYREVOKED", NULL, EKEYREVOKED, CONST_NUMBER},
#endif
#ifdef EKEYEXPIRED
  {"EKEYEXPIRED", NULL, EKEYEXPIRED, CONST_NUMBER},
#endif
#ifdef ECANCELED
  {"ECANCELED", NULL, ECANCELED, CONST_NUMBER},
#endif
#ifdef EINVAL
  {"EINVAL", NULL, EINVAL, CONST_NUMBER},
#endif
#ifdef ENAVAIL
  {"ENAVAIL", NULL, ENAVAIL, CONST_NUMBER},
#endif
#ifdef ENONET
  {"ENONET", NULL, ENONET, CONST_NUMBER},
#endif
#ifdef ELIBSCN
  {"ELIBSCN", NULL, ELIBSCN, CONST_NUMBER},
#endif
#ifdef EFBIG
  {"EFBIG", NULL, EFBIG, CONST_NUMBER},
#endif
#ifdef EREMOTE
  {"EREMOTE", NULL, EREMOTE, CONST_NUMBER},
#endif
#ifdef EDQUOT
  {"EDQUOT", NULL, EDQUOT, CONST_NUMBER},
#endif
#ifdef ENOTEMPTY
  {"ENOTEMPTY", NULL, ENOTEMPTY, CONST_NUMBER},
#endif
#ifdef ENOENT
  {"ENOENT", NULL, ENOENT, CONST_NUMBER},
#endif
#ifdef EISNAM
  {"EISNAM", NULL, EISNAM, CONST_NUMBER},
#endif
#ifdef EREMOTEIO
  {"EREMOTEIO", NULL, EREMOTEIO, CONST_NUMBER},
#endif
#ifdef EACCES
  {"EACCES", NULL, EACCES, CONST_NUMBER},
#endif
#ifdef ENOCOMPATPROTO
  {"ENOCOMPATPROTO", NULL, ENOCOMPATPROTO, CONST_NUMBER},
#endif
#ifdef ENOTNAM
  {"ENOTNAM", NULL, ENOTNAM, CONST_NUMBER},
#endif
#ifdef ESPIPE
  {"ESPIPE", NULL, ESPIPE, CONST_NUMBER},
#endif
#ifdef EUCLEAN
  {"EUCLEAN", NULL, EUCLEAN, CONST_NUMBER},
#endif
#ifdef ELIBMAX
  {"ELIBMAX", NULL, ELIBMAX, CONST_NUMBER},
#endif
#ifdef EDEADLK
  {"EDEADLK", NULL, EDEADLK, CONST_NUMBER},
#endif
#ifdef EALREADY
  {"EALREADY", NULL, EALREADY, CONST_NUMBER},
#endif
#ifdef EFSM
  {"EFSM", NULL, EFSM, CONST_NUMBER},
#endif
#ifdef EBFONT
  {"EBFONT", NULL, EBFONT, CONST_NUMBER},
#endif
#ifdef EXFULL
  {"EXFULL", NULL, EXFULL, CONST_NUMBER},
#endif
#ifdef EL2HLT
  {"EL2HLT", NULL, EL2HLT, CONST_NUMBER},
#endif
#ifdef EUNATCH
  {"EUNATCH", NULL, EUNATCH, CONST_NUMBER},
#endif
#ifdef EISDIR
  {"EISDIR", NULL, EISDIR, CONST_NUMBER},
#endif
#ifdef EMSGSIZE
  {"EMSGSIZE", NULL, EMSGSIZE, CONST_NUMBER},
#endif
#ifdef ENFILE
  {"ENFILE", NULL, ENFILE, CONST_NUMBER},
#endif
#ifdef EADDRNOTAVAIL
  {"EADDRNOTAVAIL", NULL, EADDRNOTAVAIL, CONST_NUMBER},
#endif
#ifdef EAFNOSUPPORT
  {"EAFNOSUPPORT", NULL, EAFNOSUPPORT, CONST_NUMBER},
#endif
#ifdef ENOTDIR
  {"ENOTDIR", NULL, ENOTDIR, CONST_NUMBER},
#endif
#ifdef EPROTOTYPE
  {"EPROTOTYPE", NULL, EPROTOTYPE, CONST_NUMBER},
#endif
#ifdef EOWNERDEAD
  {"EOWNERDEAD", NULL, EOWNERDEAD, CONST_NUMBER},
#endif
#ifdef EBADE
  {"EBADE", NULL, EBADE, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_ZErrors_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_zmq_msg_t_pub_funcs[] = {
  {"init", zmq_msg_t__init__meth},
  {"init_size", zmq_msg_t__init_size__meth},
  {"init_data", zmq_msg_t__init_data__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_zmq_msg_t_methods[] = {
  {"close", zmq_msg_t__close__meth},
  {"move", zmq_msg_t__move__meth},
  {"copy", zmq_msg_t__copy__meth},
  {"set_data", zmq_msg_t__set_data__meth},
  {"data", zmq_msg_t__data__meth},
  {"set_size", zmq_msg_t__set_size__meth},
  {"size", zmq_msg_t__size__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_zmq_msg_t_metas[] = {
  {"__gc", zmq_msg_t__delete__meth},
  {"__tostring", zmq_msg_t____tostring__meth},
  {"__eq", obj_simple_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_zmq_msg_t_bases[] = {
  {-1, NULL}
};

static const obj_field obj_zmq_msg_t_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_zmq_msg_t_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_zmq_msg_t_implements[] = {
  { "BufferIF", &(zmq_msg_t_Buffer) },

  { "MutableBufferIF", &(zmq_msg_t_MutableBuffer) },

  {NULL, NULL}
};

static const luaL_Reg obj_ZMQ_Socket_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_ZMQ_Socket_methods[] = {
  {"close", ZMQ_Socket__close__meth},
  {"bind", ZMQ_Socket__bind__meth},
#if (VERSION_3_2|VERSION_4_0)
  {"unbind", ZMQ_Socket__unbind__meth},
#endif
  {"connect", ZMQ_Socket__connect__meth},
#if (VERSION_3_2|VERSION_4_0)
  {"disconnect", ZMQ_Socket__disconnect__meth},
#endif
  {"setopt", ZMQ_Socket__setopt__meth},
  {"getopt", ZMQ_Socket__getopt__meth},
  {"send_msg", ZMQ_Socket__send_msg__meth},
  {"send", ZMQ_Socket__send__meth},
  {"recv_msg", ZMQ_Socket__recv_msg__meth},
  {"recv", ZMQ_Socket__recv__meth},
#if (VERSION_3_2|VERSION_4_0)
  {"monitor", ZMQ_Socket__monitor__meth},
#endif
#if (VERSION_3_2|VERSION_4_0)
  {"recv_event", ZMQ_Socket__recv_event__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"hwm", ZMQ_Socket__hwm__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"set_hwm", ZMQ_Socket__set_hwm__meth},
#endif
#if (VERSION_2_0)
  {"swap", ZMQ_Socket__swap__meth},
#endif
#if (VERSION_2_0)
  {"set_swap", ZMQ_Socket__set_swap__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"affinity", ZMQ_Socket__affinity__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"set_affinity", ZMQ_Socket__set_affinity__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"identity", ZMQ_Socket__identity__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"set_identity", ZMQ_Socket__set_identity__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"subscribe", ZMQ_Socket__subscribe__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"unsubscribe", ZMQ_Socket__unsubscribe__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"rate", ZMQ_Socket__rate__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"set_rate", ZMQ_Socket__set_rate__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"recovery_ivl", ZMQ_Socket__recovery_ivl__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"set_recovery_ivl", ZMQ_Socket__set_recovery_ivl__meth},
#endif
#if (VERSION_2_0)
  {"mcast_loop", ZMQ_Socket__mcast_loop__meth},
#endif
#if (VERSION_2_0)
  {"set_mcast_loop", ZMQ_Socket__set_mcast_loop__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"sndbuf", ZMQ_Socket__sndbuf__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"set_sndbuf", ZMQ_Socket__set_sndbuf__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"rcvbuf", ZMQ_Socket__rcvbuf__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"set_rcvbuf", ZMQ_Socket__set_rcvbuf__meth},
#endif
#if (VERSION_2_0|VERSION_3_0|VERSION_4_0)
  {"rcvmore", ZMQ_Socket__rcvmore__meth},
#endif
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
  {"fd", ZMQ_Socket__fd__meth},
#endif
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
  {"events", ZMQ_Socket__events__meth},
#endif
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
  {"type", ZMQ_Socket__type__meth},
#endif
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
  {"linger", ZMQ_Socket__linger__meth},
#endif
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
  {"set_linger", ZMQ_Socket__set_linger__meth},
#endif
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
  {"reconnect_ivl", ZMQ_Socket__reconnect_ivl__meth},
#endif
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
  {"set_reconnect_ivl", ZMQ_Socket__set_reconnect_ivl__meth},
#endif
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
  {"backlog", ZMQ_Socket__backlog__meth},
#endif
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
  {"set_backlog", ZMQ_Socket__set_backlog__meth},
#endif
#if (VERSION_2_1)
  {"recovery_ivl_msec", ZMQ_Socket__recovery_ivl_msec__meth},
#endif
#if (VERSION_2_1)
  {"set_recovery_ivl_msec", ZMQ_Socket__set_recovery_ivl_msec__meth},
#endif
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
  {"reconnect_ivl_max", ZMQ_Socket__reconnect_ivl_max__meth},
#endif
#if (VERSION_2_1|VERSION_3_0|VERSION_4_0)
  {"set_reconnect_ivl_max", ZMQ_Socket__set_reconnect_ivl_max__meth},
#endif
#if (VERSION_3_0|VERSION_4_0)
  {"maxmsgsize", ZMQ_Socket__maxmsgsize__meth},
#endif
#if (VERSION_3_0|VERSION_4_0)
  {"set_maxmsgsize", ZMQ_Socket__set_maxmsgsize__meth},
#endif
#if (VERSION_3_0|VERSION_4_0)
  {"sndhwm", ZMQ_Socket__sndhwm__meth},
#endif
#if (VERSION_3_0|VERSION_4_0)
  {"set_sndhwm", ZMQ_Socket__set_sndhwm__meth},
#endif
#if (VERSION_3_0|VERSION_4_0)
  {"rcvhwm", ZMQ_Socket__rcvhwm__meth},
#endif
#if (VERSION_3_0|VERSION_4_0)
  {"set_rcvhwm", ZMQ_Socket__set_rcvhwm__meth},
#endif
#if (VERSION_3_0|VERSION_4_0)
  {"multicast_hops", ZMQ_Socket__multicast_hops__meth},
#endif
#if (VERSION_3_0|VERSION_4_0)
  {"set_multicast_hops", ZMQ_Socket__set_multicast_hops__meth},
#endif
#if (VERSION_2_2|VERSION_3_0|VERSION_4_0)
  {"rcvtimeo", ZMQ_Socket__rcvtimeo__meth},
#endif
#if (VERSION_2_2|VERSION_3_0|VERSION_4_0)
  {"set_rcvtimeo", ZMQ_Socket__set_rcvtimeo__meth},
#endif
#if (VERSION_2_2|VERSION_3_0|VERSION_4_0)
  {"sndtimeo", ZMQ_Socket__sndtimeo__meth},
#endif
#if (VERSION_2_2|VERSION_3_0|VERSION_4_0)
  {"set_sndtimeo", ZMQ_Socket__set_sndtimeo__meth},
#endif
#if (VERSION_3_0|VERSION_4_0)
  {"ipv4only", ZMQ_Socket__ipv4only__meth},
#endif
#if (VERSION_3_0|VERSION_4_0)
  {"set_ipv4only", ZMQ_Socket__set_ipv4only__meth},
#endif
#if (VERSION_4_0)
  {"last_endpoint", ZMQ_Socket__last_endpoint__meth},
#endif
#if (VERSION_4_0)
  {"router_mandatory", ZMQ_Socket__router_mandatory__meth},
#endif
#if (VERSION_4_0)
  {"tcp_keepalive", ZMQ_Socket__tcp_keepalive__meth},
#endif
#if (VERSION_4_0)
  {"set_tcp_keepalive", ZMQ_Socket__set_tcp_keepalive__meth},
#endif
#if (VERSION_4_0)
  {"tcp_keepalive_cnt", ZMQ_Socket__tcp_keepalive_cnt__meth},
#endif
#if (VERSION_4_0)
  {"set_tcp_keepalive_cnt", ZMQ_Socket__set_tcp_keepalive_cnt__meth},
#endif
#if (VERSION_4_0)
  {"tcp_keepalive_idle", ZMQ_Socket__tcp_keepalive_idle__meth},
#endif
#if (VERSION_4_0)
  {"set_tcp_keepalive_idle", ZMQ_Socket__set_tcp_keepalive_idle__meth},
#endif
#if (VERSION_4_0)
  {"tcp_keepalive_intvl", ZMQ_Socket__tcp_keepalive_intvl__meth},
#endif
#if (VERSION_4_0)
  {"set_tcp_keepalive_intvl", ZMQ_Socket__set_tcp_keepalive_intvl__meth},
#endif
#if (VERSION_4_0)
  {"tcp_accept_filter", ZMQ_Socket__tcp_accept_filter__meth},
#endif
#if (VERSION_4_0)
  {"immediate", ZMQ_Socket__immediate__meth},
#endif
#if (VERSION_4_0)
  {"set_immediate", ZMQ_Socket__set_immediate__meth},
#endif
#if (VERSION_4_0)
  {"xpub_verbose", ZMQ_Socket__xpub_verbose__meth},
#endif
#if (VERSION_4_0)
  {"router_raw", ZMQ_Socket__router_raw__meth},
#endif
#if (VERSION_4_0)
  {"ipv6", ZMQ_Socket__ipv6__meth},
#endif
#if (VERSION_4_0)
  {"set_ipv6", ZMQ_Socket__set_ipv6__meth},
#endif
#if (VERSION_4_0)
  {"mechanism", ZMQ_Socket__mechanism__meth},
#endif
#if (VERSION_4_0)
  {"plain_server", ZMQ_Socket__plain_server__meth},
#endif
#if (VERSION_4_0)
  {"set_plain_server", ZMQ_Socket__set_plain_server__meth},
#endif
#if (VERSION_4_0)
  {"plain_username", ZMQ_Socket__plain_username__meth},
#endif
#if (VERSION_4_0)
  {"set_plain_username", ZMQ_Socket__set_plain_username__meth},
#endif
#if (VERSION_4_0)
  {"plain_password", ZMQ_Socket__plain_password__meth},
#endif
#if (VERSION_4_0)
  {"set_plain_password", ZMQ_Socket__set_plain_password__meth},
#endif
#if (VERSION_4_0)
  {"curve_server", ZMQ_Socket__curve_server__meth},
#endif
#if (VERSION_4_0)
  {"set_curve_server", ZMQ_Socket__set_curve_server__meth},
#endif
#if (VERSION_4_0)
  {"curve_publickey", ZMQ_Socket__curve_publickey__meth},
#endif
#if (VERSION_4_0)
  {"set_curve_publickey", ZMQ_Socket__set_curve_publickey__meth},
#endif
#if (VERSION_4_0)
  {"curve_secretkey", ZMQ_Socket__curve_secretkey__meth},
#endif
#if (VERSION_4_0)
  {"set_curve_secretkey", ZMQ_Socket__set_curve_secretkey__meth},
#endif
#if (VERSION_4_0)
  {"curve_serverkey", ZMQ_Socket__curve_serverkey__meth},
#endif
#if (VERSION_4_0)
  {"set_curve_serverkey", ZMQ_Socket__set_curve_serverkey__meth},
#endif
#if (VERSION_4_0)
  {"probe_router", ZMQ_Socket__probe_router__meth},
#endif
#if (VERSION_4_0)
  {"req_correlate", ZMQ_Socket__req_correlate__meth},
#endif
#if (VERSION_4_0)
  {"req_relaxed", ZMQ_Socket__req_relaxed__meth},
#endif
#if (VERSION_4_0)
  {"conflate", ZMQ_Socket__conflate__meth},
#endif
#if (VERSION_4_0)
  {"set_conflate", ZMQ_Socket__set_conflate__meth},
#endif
#if (VERSION_4_0)
  {"zap_domain", ZMQ_Socket__zap_domain__meth},
#endif
#if (VERSION_4_0)
  {"set_zap_domain", ZMQ_Socket__set_zap_domain__meth},
#endif
  {NULL, NULL}
};

static const luaL_Reg obj_ZMQ_Socket_metas[] = {
  {"__gc", ZMQ_Socket__close__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_ZMQ_Socket_bases[] = {
  {-1, NULL}
};

static const obj_field obj_ZMQ_Socket_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_ZMQ_Socket_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_ZMQ_Socket_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_ZMQ_Poller_pub_funcs[] = {
  {"new", ZMQ_Poller__new__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_ZMQ_Poller_methods[] = {
  {"close", ZMQ_Poller__close__meth},
  {"add", ZMQ_Poller__add__meth},
  {"modify", ZMQ_Poller__modify__meth},
  {"remove", ZMQ_Poller__remove__meth},
  {"poll", ZMQ_Poller__poll__meth},
  {"next_revents_idx", ZMQ_Poller__next_revents_idx__meth},
  {"count", ZMQ_Poller__count__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_ZMQ_Poller_metas[] = {
  {"__gc", ZMQ_Poller__close__meth},
  {"__tostring", obj_simple_udata_default_tostring},
  {"__eq", obj_simple_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_ZMQ_Poller_bases[] = {
  {-1, NULL}
};

static const obj_field obj_ZMQ_Poller_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_ZMQ_Poller_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_ZMQ_Poller_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_ZMQ_Ctx_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_ZMQ_Ctx_methods[] = {
  {"term", ZMQ_Ctx__term__meth},
  {"lightuserdata", ZMQ_Ctx__lightuserdata__meth},
  {"socket", ZMQ_Ctx__socket__meth},
#if (VERSION_3_2|VERSION_4_0)
  {"set", ZMQ_Ctx__set__meth},
#endif
#if (VERSION_3_2|VERSION_4_0)
  {"get", ZMQ_Ctx__get__meth},
#endif
  {NULL, NULL}
};

static const luaL_Reg obj_ZMQ_Ctx_metas[] = {
  {"__gc", ZMQ_Ctx__term__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_ZMQ_Ctx_bases[] = {
  {-1, NULL}
};

static const obj_field obj_ZMQ_Ctx_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_ZMQ_Ctx_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_ZMQ_Ctx_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg obj_ZMQ_StopWatch_pub_funcs[] = {
  {"start", ZMQ_StopWatch__start__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_ZMQ_StopWatch_methods[] = {
  {"stop", ZMQ_StopWatch__stop__meth},
  {NULL, NULL}
};

static const luaL_Reg obj_ZMQ_StopWatch_metas[] = {
  {"__gc", ZMQ_StopWatch__stop__meth},
  {"__tostring", obj_udata_default_tostring},
  {"__eq", obj_udata_default_equal},
  {NULL, NULL}
};

static const obj_base obj_ZMQ_StopWatch_bases[] = {
  {-1, NULL}
};

static const obj_field obj_ZMQ_StopWatch_fields[] = {
  {NULL, 0, 0, 0}
};

static const obj_const obj_ZMQ_StopWatch_constants[] = {
  {NULL, NULL, 0.0 , 0}
};

static const reg_impl obj_ZMQ_StopWatch_implements[] = {
  {NULL, NULL}
};

static const luaL_Reg zmq_function[] = {
  {"version", zmq__version__func},
  {"init", zmq__init__func},
  {"init_ctx", zmq__init_ctx__func},
#if (VERSION_2_0|VERSION_3_2)
  {"device", zmq__device__func},
#endif
#if (VERSION_3_2)
  {"proxy", zmq__proxy__func},
#endif
  {"stopwatch_start", zmq__stopwatch_start__func},
  {"sleep", zmq__sleep__func},
  {NULL, NULL}
};

static const obj_const zmq_constants[] = {
#ifdef ZMQ_UNSUBSCRIBE
  {"UNSUBSCRIBE", NULL, ZMQ_UNSUBSCRIBE, CONST_NUMBER},
#endif
#ifdef ZMQ_CURVE_SERVER
  {"CURVE_SERVER", NULL, ZMQ_CURVE_SERVER, CONST_NUMBER},
#endif
#ifdef ZMQ_XPUB_VERBOSE
  {"XPUB_VERBOSE", NULL, ZMQ_XPUB_VERBOSE, CONST_NUMBER},
#endif
#ifdef ZMQ_ZAP_DOMAIN
  {"ZAP_DOMAIN", NULL, ZMQ_ZAP_DOMAIN, CONST_NUMBER},
#endif
#ifdef ZMQ_FORWARDER
  {"FORWARDER", NULL, ZMQ_FORWARDER, CONST_NUMBER},
#endif
#ifdef ZMQ_POLL_MSEC
  {"POLL_MSEC", NULL, ZMQ_POLL_MSEC, CONST_NUMBER},
#endif
#ifdef ZMQ_RCVMORE
  {"RCVMORE", NULL, ZMQ_RCVMORE, CONST_NUMBER},
#endif
#ifdef ZMQ_IDENTITY
  {"IDENTITY", NULL, ZMQ_IDENTITY, CONST_NUMBER},
#endif
#ifdef ZMQ_REQ_RELAXED
  {"REQ_RELAXED", NULL, ZMQ_REQ_RELAXED, CONST_NUMBER},
#endif
#ifdef ZMQ_EVENT_MONITOR_STOPPED
  {"EVENT_MONITOR_STOPPED", NULL, ZMQ_EVENT_MONITOR_STOPPED, CONST_NUMBER},
#endif
#ifdef ZMQ_EVENT_ALL
  {"EVENT_ALL", NULL, ZMQ_EVENT_ALL, CONST_NUMBER},
#endif
#ifdef ZMQ_SUBSCRIBE
  {"SUBSCRIBE", NULL, ZMQ_SUBSCRIBE, CONST_NUMBER},
#endif
#ifdef ZMQ_EVENT_ACCEPTED
  {"EVENT_ACCEPTED", NULL, ZMQ_EVENT_ACCEPTED, CONST_NUMBER},
#endif
#ifdef ZMQ_MAX_VSM_SIZE
  {"MAX_VSM_SIZE", NULL, ZMQ_MAX_VSM_SIZE, CONST_NUMBER},
#endif
#ifdef ZMQ_MAX_SOCKETS
  {"MAX_SOCKETS", NULL, ZMQ_MAX_SOCKETS, CONST_NUMBER},
#endif
#ifdef ZMQ_PLAIN
  {"PLAIN", NULL, ZMQ_PLAIN, CONST_NUMBER},
#endif
#ifdef ZMQ_XREQ
  {"XREQ", NULL, ZMQ_XREQ, CONST_NUMBER},
#endif
#ifdef ZMQ_EVENT_CLOSE_FAILED
  {"EVENT_CLOSE_FAILED", NULL, ZMQ_EVENT_CLOSE_FAILED, CONST_NUMBER},
#endif
#ifdef ZMQ_POLLIN
  {"POLLIN", NULL, ZMQ_POLLIN, CONST_NUMBER},
#endif
#ifdef ZMQ_EVENT_ACCEPT_FAILED
  {"EVENT_ACCEPT_FAILED", NULL, ZMQ_EVENT_ACCEPT_FAILED, CONST_NUMBER},
#endif
#ifdef ZMQ_RATE
  {"RATE", NULL, ZMQ_RATE, CONST_NUMBER},
#endif
#ifdef ZMQ_SUB
  {"SUB", NULL, ZMQ_SUB, CONST_NUMBER},
#endif
#ifdef ZMQ_RECONNECT_IVL_MSEC
  {"RECONNECT_IVL_MSEC", NULL, ZMQ_RECONNECT_IVL_MSEC, CONST_NUMBER},
#endif
#ifdef ZMQ_PUB
  {"PUB", NULL, ZMQ_PUB, CONST_NUMBER},
#endif
#ifdef ZMQ_RCVHWM
  {"RCVHWM", NULL, ZMQ_RCVHWM, CONST_NUMBER},
#endif
#ifdef ZMQ_VSM
  {"VSM", NULL, ZMQ_VSM, CONST_NUMBER},
#endif
#ifdef ZMQ_PLAIN_SERVER
  {"PLAIN_SERVER", NULL, ZMQ_PLAIN_SERVER, CONST_NUMBER},
#endif
#ifdef ZMQ_NOBLOCK
  {"NOBLOCK", NULL, ZMQ_NOBLOCK, CONST_NUMBER},
#endif
#ifdef ZMQ_MSG_MORE
  {"MSG_MORE", NULL, ZMQ_MSG_MORE, CONST_NUMBER},
#endif
#ifdef ZMQ_SNDMORE
  {"SNDMORE", NULL, ZMQ_SNDMORE, CONST_NUMBER},
#endif
#ifdef ZMQ_RECONNECT_IVL_MAX
  {"RECONNECT_IVL_MAX", NULL, ZMQ_RECONNECT_IVL_MAX, CONST_NUMBER},
#endif
#ifdef ZMQ_PULL
  {"PULL", NULL, ZMQ_PULL, CONST_NUMBER},
#endif
#ifdef ZMQ_XREP
  {"XREP", NULL, ZMQ_XREP, CONST_NUMBER},
#endif
#ifdef ZMQ_POLLERR
  {"POLLERR", NULL, ZMQ_POLLERR, CONST_NUMBER},
#endif
#ifdef ZMQ_PAIR
  {"PAIR", NULL, ZMQ_PAIR, CONST_NUMBER},
#endif
#ifdef ZMQ_IPV6
  {"IPV6", NULL, ZMQ_IPV6, CONST_NUMBER},
#endif
#ifdef ZMQ_TCP_KEEPALIVE_CNT
  {"TCP_KEEPALIVE_CNT", NULL, ZMQ_TCP_KEEPALIVE_CNT, CONST_NUMBER},
#endif
#ifdef ZMQ_PLAIN_USERNAME
  {"PLAIN_USERNAME", NULL, ZMQ_PLAIN_USERNAME, CONST_NUMBER},
#endif
#ifdef ZMQ_REP
  {"REP", NULL, ZMQ_REP, CONST_NUMBER},
#endif
#ifdef ZMQ_XPUB
  {"XPUB", NULL, ZMQ_XPUB, CONST_NUMBER},
#endif
#ifdef ZMQ_DONTWAIT
  {"DONTWAIT", NULL, ZMQ_DONTWAIT, CONST_NUMBER},
#endif
#ifdef ZMQ_SNDBUF
  {"SNDBUF", NULL, ZMQ_SNDBUF, CONST_NUMBER},
#endif
#ifdef ZMQ_LINGER
  {"LINGER", NULL, ZMQ_LINGER, CONST_NUMBER},
#endif
#ifdef ZMQ_EVENT_LISTENING
  {"EVENT_LISTENING", NULL, ZMQ_EVENT_LISTENING, CONST_NUMBER},
#endif
#ifdef ZMQ_EVENT_CONNECTED
  {"EVENT_CONNECTED", NULL, ZMQ_EVENT_CONNECTED, CONST_NUMBER},
#endif
#ifdef ZMQ_DELIMITER
  {"DELIMITER", NULL, ZMQ_DELIMITER, CONST_NUMBER},
#endif
#ifdef ZMQ_RCVBUF
  {"RCVBUF", NULL, ZMQ_RCVBUF, CONST_NUMBER},
#endif
#ifdef ZMQ_EVENTS
  {"EVENTS", NULL, ZMQ_EVENTS, CONST_NUMBER},
#endif
#ifdef ZMQ_DEALER
  {"DEALER", NULL, ZMQ_DEALER, CONST_NUMBER},
#endif
#ifdef ZMQ_TYPE
  {"TYPE", NULL, ZMQ_TYPE, CONST_NUMBER},
#endif
#ifdef ZMQ_IMMEDIATE
  {"IMMEDIATE", NULL, ZMQ_IMMEDIATE, CONST_NUMBER},
#endif
#ifdef ZMQ_MULTICAST_HOPS
  {"MULTICAST_HOPS", NULL, ZMQ_MULTICAST_HOPS, CONST_NUMBER},
#endif
#ifdef ZMQ_RECOVERY_IVL
  {"RECOVERY_IVL", NULL, ZMQ_RECOVERY_IVL, CONST_NUMBER},
#endif
#ifdef ZMQ_NULL
  {"NULL", NULL, ZMQ_NULL, CONST_NUMBER},
#endif
#ifdef ZMQ_SWAP
  {"SWAP", NULL, ZMQ_SWAP, CONST_NUMBER},
#endif
#ifdef ZMQ_XSUB
  {"XSUB", NULL, ZMQ_XSUB, CONST_NUMBER},
#endif
#ifdef ZMQ_AFFINITY
  {"AFFINITY", NULL, ZMQ_AFFINITY, CONST_NUMBER},
#endif
#ifdef ZMQ_RECONNECT_IVL
  {"RECONNECT_IVL", NULL, ZMQ_RECONNECT_IVL, CONST_NUMBER},
#endif
#ifdef ZMQ_EVENT_CONNECT_RETRIED
  {"EVENT_CONNECT_RETRIED", NULL, ZMQ_EVENT_CONNECT_RETRIED, CONST_NUMBER},
#endif
#ifdef ZMQ_TCP_KEEPALIVE
  {"TCP_KEEPALIVE", NULL, ZMQ_TCP_KEEPALIVE, CONST_NUMBER},
#endif
#ifdef ZMQ_LAST_ENDPOINT
  {"LAST_ENDPOINT", NULL, ZMQ_LAST_ENDPOINT, CONST_NUMBER},
#endif
#ifdef ZMQ_SNDTIMEO
  {"SNDTIMEO", NULL, ZMQ_SNDTIMEO, CONST_NUMBER},
#endif
#ifdef ZMQ_QUEUE
  {"QUEUE", NULL, ZMQ_QUEUE, CONST_NUMBER},
#endif
#ifdef ZMQ_ROUTER
  {"ROUTER", NULL, ZMQ_ROUTER, CONST_NUMBER},
#endif
#ifdef ZMQ_CONFLATE
  {"CONFLATE", NULL, ZMQ_CONFLATE, CONST_NUMBER},
#endif
#ifdef ZMQ_TCP_KEEPALIVE_IDLE
  {"TCP_KEEPALIVE_IDLE", NULL, ZMQ_TCP_KEEPALIVE_IDLE, CONST_NUMBER},
#endif
#ifdef ZMQ_PUSH
  {"PUSH", NULL, ZMQ_PUSH, CONST_NUMBER},
#endif
#ifdef ZMQ_MAXMSGSIZE
  {"MAXMSGSIZE", NULL, ZMQ_MAXMSGSIZE, CONST_NUMBER},
#endif
#ifdef ZMQ_ROUTER_MANDATORY
  {"ROUTER_MANDATORY", NULL, ZMQ_ROUTER_MANDATORY, CONST_NUMBER},
#endif
#ifdef ZMQ_FD
  {"FD", NULL, ZMQ_FD, CONST_NUMBER},
#endif
#ifdef ZMQ_SNDLABEL
  {"SNDLABEL", NULL, ZMQ_SNDLABEL, CONST_NUMBER},
#endif
#ifdef ZMQ_TCP_KEEPALIVE_INTVL
  {"TCP_KEEPALIVE_INTVL", NULL, ZMQ_TCP_KEEPALIVE_INTVL, CONST_NUMBER},
#endif
#ifdef ZMQ_PLAIN_PASSWORD
  {"PLAIN_PASSWORD", NULL, ZMQ_PLAIN_PASSWORD, CONST_NUMBER},
#endif
#ifdef ZMQ_POLLOUT
  {"POLLOUT", NULL, ZMQ_POLLOUT, CONST_NUMBER},
#endif
#ifdef ZMQ_MCAST_LOOP
  {"MCAST_LOOP", NULL, ZMQ_MCAST_LOOP, CONST_NUMBER},
#endif
#ifdef ZMQ_CURVE
  {"CURVE", NULL, ZMQ_CURVE, CONST_NUMBER},
#endif
#ifdef ZMQ_RCVTIMEO
  {"RCVTIMEO", NULL, ZMQ_RCVTIMEO, CONST_NUMBER},
#endif
#ifdef ZMQ_TCP_ACCEPT_FILTER
  {"TCP_ACCEPT_FILTER", NULL, ZMQ_TCP_ACCEPT_FILTER, CONST_NUMBER},
#endif
#ifdef ZMQ_ROUTER_RAW
  {"ROUTER_RAW", NULL, ZMQ_ROUTER_RAW, CONST_NUMBER},
#endif
#ifdef ZMQ_SNDHWM
  {"SNDHWM", NULL, ZMQ_SNDHWM, CONST_NUMBER},
#endif
#ifdef ZMQ_REQ
  {"REQ", NULL, ZMQ_REQ, CONST_NUMBER},
#endif
#ifdef ZMQ_CURVE_SERVERKEY
  {"CURVE_SERVERKEY", NULL, ZMQ_CURVE_SERVERKEY, CONST_NUMBER},
#endif
#ifdef ZMQ_MSG_SHARED
  {"MSG_SHARED", NULL, ZMQ_MSG_SHARED, CONST_NUMBER},
#endif
#ifdef ZMQ_PROBE_ROUTER
  {"PROBE_ROUTER", NULL, ZMQ_PROBE_ROUTER, CONST_NUMBER},
#endif
#ifdef ZMQ_REQ_CORRELATE
  {"REQ_CORRELATE", NULL, ZMQ_REQ_CORRELATE, CONST_NUMBER},
#endif
#ifdef ZMQ_RCVLABEL
  {"RCVLABEL", NULL, ZMQ_RCVLABEL, CONST_NUMBER},
#endif
#ifdef ZMQ_EVENT_DISCONNECTED
  {"EVENT_DISCONNECTED", NULL, ZMQ_EVENT_DISCONNECTED, CONST_NUMBER},
#endif
#ifdef ZMQ_IO_THREADS
  {"IO_THREADS", NULL, ZMQ_IO_THREADS, CONST_NUMBER},
#endif
#ifdef ZMQ_EVENT_CLOSED
  {"EVENT_CLOSED", NULL, ZMQ_EVENT_CLOSED, CONST_NUMBER},
#endif
#ifdef ZMQ_STREAMER
  {"STREAMER", NULL, ZMQ_STREAMER, CONST_NUMBER},
#endif
#ifdef ZMQ_MECHANISM
  {"MECHANISM", NULL, ZMQ_MECHANISM, CONST_NUMBER},
#endif
#ifdef ZMQ_CURVE_SECRETKEY
  {"CURVE_SECRETKEY", NULL, ZMQ_CURVE_SECRETKEY, CONST_NUMBER},
#endif
#ifdef ZMQ_HWM
  {"HWM", NULL, ZMQ_HWM, CONST_NUMBER},
#endif
#ifdef ZMQ_CURVE_PUBLICKEY
  {"CURVE_PUBLICKEY", NULL, ZMQ_CURVE_PUBLICKEY, CONST_NUMBER},
#endif
#ifdef ZMQ_EVENT_CONNECT_DELAYED
  {"EVENT_CONNECT_DELAYED", NULL, ZMQ_EVENT_CONNECT_DELAYED, CONST_NUMBER},
#endif
#ifdef ZMQ_EVENT_BIND_FAILED
  {"EVENT_BIND_FAILED", NULL, ZMQ_EVENT_BIND_FAILED, CONST_NUMBER},
#endif
#ifdef ZMQ_BACKLOG
  {"BACKLOG", NULL, ZMQ_BACKLOG, CONST_NUMBER},
#endif
  {NULL, NULL, 0.0 , 0}
};



static const reg_sub_module reg_sub_modules[] = {
  { &(obj_type_ZErrors), REG_META, obj_ZErrors_pub_funcs, obj_ZErrors_methods, obj_ZErrors_metas, NULL, NULL, obj_ZErrors_constants, NULL, 0},
  { &(obj_type_zmq_msg_t), REG_OBJECT, obj_zmq_msg_t_pub_funcs, obj_zmq_msg_t_methods, obj_zmq_msg_t_metas, obj_zmq_msg_t_bases, obj_zmq_msg_t_fields, obj_zmq_msg_t_constants, obj_zmq_msg_t_implements, 0},
  { &(obj_type_ZMQ_Socket), REG_OBJECT, obj_ZMQ_Socket_pub_funcs, obj_ZMQ_Socket_methods, obj_ZMQ_Socket_metas, obj_ZMQ_Socket_bases, obj_ZMQ_Socket_fields, obj_ZMQ_Socket_constants, obj_ZMQ_Socket_implements, 0},
  { &(obj_type_ZMQ_Poller), REG_OBJECT, obj_ZMQ_Poller_pub_funcs, obj_ZMQ_Poller_methods, obj_ZMQ_Poller_metas, obj_ZMQ_Poller_bases, obj_ZMQ_Poller_fields, obj_ZMQ_Poller_constants, obj_ZMQ_Poller_implements, 0},
  { &(obj_type_ZMQ_Ctx), REG_OBJECT, obj_ZMQ_Ctx_pub_funcs, obj_ZMQ_Ctx_methods, obj_ZMQ_Ctx_metas, obj_ZMQ_Ctx_bases, obj_ZMQ_Ctx_fields, obj_ZMQ_Ctx_constants, obj_ZMQ_Ctx_implements, 0},
  { &(obj_type_ZMQ_StopWatch), REG_OBJECT, obj_ZMQ_StopWatch_pub_funcs, obj_ZMQ_StopWatch_methods, obj_ZMQ_StopWatch_metas, obj_ZMQ_StopWatch_bases, obj_ZMQ_StopWatch_fields, obj_ZMQ_StopWatch_constants, obj_ZMQ_StopWatch_implements, 0},
  {NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0}
};





#if LUAJIT_FFI
static const ffi_export_symbol zmq_ffi_export[] = {
  {NULL, { NULL } }
};
#endif




static const luaL_Reg submodule_libs[] = {
  {NULL, NULL}
};



static void create_object_instance_cache(lua_State *L) {
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_rawget(L, LUA_REGISTRYINDEX);  /* check if weak table exists already. */
	if(!lua_isnil(L, -1)) {
		lua_pop(L, 1); /* pop weak table. */
		return;
	}
	lua_pop(L, 1); /* pop nil. */
	/* create weak table for object instance references. */
	lua_pushlightuserdata(L, obj_udata_weak_ref_key); /* key for weak table. */
	lua_newtable(L);               /* weak table. */
	lua_newtable(L);               /* metatable for weak table. */
	lua_pushliteral(L, "__mode");
	lua_pushliteral(L, "v");
	lua_rawset(L, -3);             /* metatable.__mode = 'v'  weak values. */
	lua_setmetatable(L, -2);       /* add metatable to weak table. */
	lua_rawset(L, LUA_REGISTRYINDEX);  /* create reference to weak table. */
}

LUA_NOBJ_API int luaopen_zmq(lua_State *L) {
	const reg_sub_module *reg = reg_sub_modules;
	const luaL_Reg *submodules = submodule_libs;
	int priv_table = -1;

	/* register interfaces */
	obj_register_interfaces(L, obj_interfaces);

	/* private table to hold reference to object metatables. */
	lua_newtable(L);
	priv_table = lua_gettop(L);
	lua_pushlightuserdata(L, obj_udata_private_key);
	lua_pushvalue(L, priv_table);
	lua_rawset(L, LUA_REGISTRYINDEX);  /* store private table in registry. */

	/* create object cache. */
	create_object_instance_cache(L);

	/* module table. */
#if REG_MODULES_AS_GLOBALS
	luaL_register(L, "zmq", zmq_function);
#else
	lua_newtable(L);
	nobj_setfuncs(L, zmq_function, 0);
#endif

	/* register module constants. */
	obj_type_register_constants(L, zmq_constants, -1, 0);

	for(; submodules->func != NULL ; submodules++) {
		lua_pushcfunction(L, submodules->func);
		lua_pushstring(L, submodules->name);
		lua_call(L, 1, 0);
	}

	/* register objects */
	for(; reg->type != NULL ; reg++) {
		lua_newtable(L); /* create public API table for object. */
		lua_pushvalue(L, -1); /* dup. object's public API table. */
		lua_setfield(L, -3, reg->type->name); /* module["<object_name>"] = <object public API> */
#if REG_OBJECTS_AS_GLOBALS
		lua_pushvalue(L, -1);                 /* dup value. */
		lua_setglobal(L, reg->type->name);    /* global: <object_name> = <object public API> */
#endif
		obj_type_register(L, reg, priv_table);
	}

#if LUAJIT_FFI
	if(nobj_check_ffi_support(L)) {
		nobj_try_loading_ffi(L, "zmq.nobj.ffi.lua", zmq_ffi_lua_code,
			zmq_ffi_export, priv_table);
	}
#endif

	/* Cache reference to zmq.ZErrors table for errno->string convertion. */
	lua_pushlightuserdata(L, zmq_ZErrors_key);
	lua_getfield(L, -2, "ZErrors");
	lua_rawset(L, LUA_REGISTRYINDEX);



	return 1;
}


