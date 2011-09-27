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

#include <string.h>
#include "zmq.h"
#include "zmq_utils.h"


#define REG_PACKAGE_IS_CONSTRUCTOR 0
#define REG_OBJECTS_AS_GLOBALS 0
#define OBJ_DATA_HIDDEN_METATABLE 1
#define LUAJIT_FFI 1
#define USE_FIELD_GET_SET_METHODS 0



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
#define false 1
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

#ifndef obj_type_free
#define obj_type_free(type, obj) do { \
	assert_obj_type(type, obj); \
	free((obj)); \
} while(0)
#endif

#ifndef obj_type_new
#define obj_type_new(type, obj) do { \
	assert_obj_type(type, obj); \
	(obj) = malloc(sizeof(type)); \
} while(0)
#endif

typedef struct obj_type obj_type;

typedef void (*base_caster_t)(void **obj);

typedef void (*dyn_caster_t)(void **obj, obj_type **type);

#define OBJ_TYPE_FLAG_WEAK_REF (1<<0)
#define OBJ_TYPE_SIMPLE (1<<1)
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

typedef struct reg_sub_module {
	obj_type        *type;
	int             is_package;
	const luaL_reg  *pub_funcs;
	const luaL_reg  *methods;
	const luaL_reg  *metas;
	const obj_base  *bases;
	const obj_field *fields;
	const obj_const *constants;
} reg_sub_module;

#define OBJ_UDATA_FLAG_OWN (1<<0)
#define OBJ_UDATA_FLAG_LOOKUP (1<<1)
#define OBJ_UDATA_LAST_FLAG (OBJ_UDATA_FLAG_LOOKUP)
typedef struct obj_udata {
	void     *obj;
	uint32_t flags;  /**< lua_own:1bit */
} obj_udata;

/* use static pointer as key to weak userdata table. */
static char *obj_udata_weak_ref_key = "obj_udata_weak_ref_key";

#if LUAJIT_FFI
typedef struct ffi_export_symbol {
	const char *name;
	void       *sym;
} ffi_export_symbol;
#endif


typedef int ZMQ_Error;

static void error_code__ZMQ_Error__push(lua_State *L, ZMQ_Error err);


static obj_type obj_types[] = {
#define obj_type_id_zmq_msg_t 0
#define obj_type_zmq_msg_t (obj_types[obj_type_id_zmq_msg_t])
  { NULL, 0, OBJ_TYPE_SIMPLE, "zmq_msg_t" },
#define obj_type_id_ZMQ_Socket 1
#define obj_type_ZMQ_Socket (obj_types[obj_type_id_ZMQ_Socket])
  { NULL, 1, OBJ_TYPE_FLAG_WEAK_REF, "ZMQ_Socket" },
#define obj_type_id_ZMQ_Poller 2
#define obj_type_ZMQ_Poller (obj_types[obj_type_id_ZMQ_Poller])
  { NULL, 2, OBJ_TYPE_SIMPLE, "ZMQ_Poller" },
#define obj_type_id_ZMQ_Ctx 3
#define obj_type_ZMQ_Ctx (obj_types[obj_type_id_ZMQ_Ctx])
  { NULL, 3, OBJ_TYPE_FLAG_WEAK_REF, "ZMQ_Ctx" },
#define obj_type_id_ZMQ_StopWatch 4
#define obj_type_ZMQ_StopWatch (obj_types[obj_type_id_ZMQ_StopWatch])
  { NULL, 4, OBJ_TYPE_FLAG_WEAK_REF, "ZMQ_StopWatch" },
  {NULL, -1, 0, NULL},
};


#ifndef REG_PACKAGE_IS_CONSTRUCTOR
#define REG_PACKAGE_IS_CONSTRUCTOR 1
#endif

#ifndef REG_OBJECTS_AS_GLOBALS
#define REG_OBJECTS_AS_GLOBALS 0
#endif

#ifndef OBJ_DATA_HIDDEN_METATABLE
#define OBJ_DATA_HIDDEN_METATABLE 1
#endif

static FUNC_UNUSED obj_udata *obj_udata_toobj(lua_State *L, int _index) {
	obj_udata *ud;
	size_t len;

	/* make sure it's a userdata value. */
	ud = (obj_udata *)lua_touserdata(L, _index);
	if(ud == NULL) {
		luaL_typerror(L, _index, "userdata"); /* is not a userdata value. */
	}
	/* verify userdata size. */
	len = lua_objlen(L, _index);
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
	if(lua_rawequal(L, -1, -2)) {
		*obj = ud->obj;
		/* same type no casting needed. */
		return 1;
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

static FUNC_UNUSED void *obj_udata_luadelete(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj_udata *ud = obj_udata_luacheck_internal(L, _index, &(obj), type, 0);
	if(ud == NULL) return NULL;
	*flags = ud->flags;
	/* null userdata. */
	ud->obj = NULL;
	ud->flags = 0;
	return obj;
}

static FUNC_UNUSED void obj_udata_luapush(lua_State *L, void *obj, obj_type *type, int flags) {
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
	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
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

	/* create new userdata. */
	ud = (obj_udata *)lua_newuserdata(L, sizeof(obj_udata));

	/* init. obj_udata. */
	ud->obj = obj;
	ud->flags = flags;
	/* get obj_type metatable. */
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
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
			if(lua_rawequal(L, -1, -2)) {
				lua_pop(L, 2); /* pop both metatables. */
				return ud;
			}
		}
	}
	luaL_typerror(L, _index, type->name); /* is not a userdata value. */
	return NULL;
}

static FUNC_UNUSED void * obj_simple_udata_luadelete(lua_State *L, int _index, obj_type *type, int *flags) {
	void *obj;
	obj = obj_simple_udata_luacheck(L, _index, type);
	*flags = OBJ_UDATA_FLAG_OWN;
	/* clear the metatable to invalidate userdata. */
	lua_pushnil(L);
	lua_setmetatable(L, _index);
	return obj;
}

static FUNC_UNUSED void *obj_simple_udata_luapush(lua_State *L, void *obj, int size, obj_type *type)
{
	/* create new userdata. */
	void *ud = lua_newuserdata(L, size);
	memcpy(ud, obj, size);
	/* get obj_type metatable. */
	lua_pushlightuserdata(L, type);
	lua_rawget(L, LUA_REGISTRYINDEX); /* type's metatable. */
	lua_setmetatable(L, -2);

	return ud;
}

/* default simple object equal method. */
static FUNC_UNUSED int obj_simple_udata_default_equal(lua_State *L) {
	void *ud1 = obj_simple_udata_toobj(L, 1);
	size_t len1 = lua_objlen(L, 1);
	void *ud2 = obj_simple_udata_toobj(L, 2);
	size_t len2 = lua_objlen(L, 2);

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

static void obj_type_register_constants(lua_State *L, const obj_const *constants, int tab_idx) {
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
		lua_rawset(L, tab_idx - 2);
		constants++;
	}
}

static void obj_type_register_package(lua_State *L, const reg_sub_module *type_reg) {
	const luaL_reg *reg_list = type_reg->pub_funcs;

	/* create public functions table. */
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register functions */
		luaL_register(L, NULL, reg_list);
	}

	obj_type_register_constants(L, type_reg->constants, -1);

	lua_pop(L, 1);  /* drop package table */
}

static void obj_type_register(lua_State *L, const reg_sub_module *type_reg, int priv_table) {
	const luaL_reg *reg_list;
	obj_type *type = type_reg->type;
	const obj_base *base = type_reg->bases;

	if(type_reg->is_package == 1) {
		obj_type_register_package(L, type_reg);
		return;
	}

	/* create public functions table. */
	reg_list = type_reg->pub_funcs;
	if(reg_list != NULL && reg_list[0].name != NULL) {
		/* register "constructors" as to object's public API */
		luaL_register(L, NULL, reg_list); /* fill public API table. */

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

	luaL_register(L, NULL, type_reg->methods); /* fill methods table. */

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

#if LUAJIT_FFI
	/* add metatable to 'priv_table' */
	lua_pushstring(L, type->name);
	lua_pushvalue(L, -2); /* dup metatable. */
	lua_rawset(L, priv_table);    /* priv_table["<object_name>"] = metatable */
#else
	(void)priv_table;
#endif

	luaL_register(L, NULL, type_reg->metas); /* fill metatable */

	/* add obj_bases to metatable. */
	while(base->id >= 0) {
		lua_pushlightuserdata(L, (void *)base);
		lua_rawseti(L, -2, base->id);
		base++;
	}

	obj_type_register_constants(L, type_reg->constants, -2);

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

#if LUAJIT_FFI
static int nobj_udata_new_ffi(lua_State *L) {
	size_t size = luaL_checkinteger(L, 1);
	void *ud;
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_settop(L, 2);
	/* create userdata. */
	ud = lua_newuserdata(L, size);
	lua_replace(L, 1);
	/* set userdata's metatable. */
	lua_setmetatable(L, 1);
	return 1;
}

static int nobj_try_loading_ffi(lua_State *L, const char *ffi_mod_name,
		const char *ffi_init_code, const ffi_export_symbol *ffi_exports, int priv_table)
{
	int err;

	/* export symbols to priv_table. */
	while(ffi_exports->name != NULL) {
		lua_pushstring(L, ffi_exports->name);
		lua_pushlightuserdata(L, ffi_exports->sym);
		lua_settable(L, priv_table);
		ffi_exports++;
	}
	err = luaL_loadbuffer(L, ffi_init_code, strlen(ffi_init_code), ffi_mod_name);
	if(0 == err) {
		lua_pushvalue(L, -2); /* dup C module's table. */
		lua_pushvalue(L, priv_table); /* move priv_table to top of stack. */
		lua_remove(L, priv_table);
		lua_pushcfunction(L, nobj_udata_new_ffi);
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


#define obj_type_zmq_msg_t_check(L, _index) \
	(zmq_msg_t *)obj_simple_udata_luacheck(L, _index, &(obj_type_zmq_msg_t))
#define obj_type_zmq_msg_t_delete(L, _index, flags) \
	(zmq_msg_t *)obj_simple_udata_luadelete(L, _index, &(obj_type_zmq_msg_t), flags)
#define obj_type_zmq_msg_t_push(L, obj, flags) \
	obj_simple_udata_luapush(L, obj, sizeof(zmq_msg_t), &(obj_type_zmq_msg_t))

#define obj_type_ZMQ_Socket_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_ZMQ_Socket))
#define obj_type_ZMQ_Socket_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_ZMQ_Socket), flags)
#define obj_type_ZMQ_Socket_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_ZMQ_Socket), flags)

#define obj_type_ZMQ_Poller_check(L, _index) \
	(ZMQ_Poller *)obj_simple_udata_luacheck(L, _index, &(obj_type_ZMQ_Poller))
#define obj_type_ZMQ_Poller_delete(L, _index, flags) \
	(ZMQ_Poller *)obj_simple_udata_luadelete(L, _index, &(obj_type_ZMQ_Poller), flags)
#define obj_type_ZMQ_Poller_push(L, obj, flags) \
	obj_simple_udata_luapush(L, obj, sizeof(ZMQ_Poller), &(obj_type_ZMQ_Poller))

#define obj_type_ZMQ_Ctx_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_ZMQ_Ctx))
#define obj_type_ZMQ_Ctx_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_ZMQ_Ctx), flags)
#define obj_type_ZMQ_Ctx_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_ZMQ_Ctx), flags)

#define obj_type_ZMQ_StopWatch_check(L, _index) \
	obj_udata_luacheck(L, _index, &(obj_type_ZMQ_StopWatch))
#define obj_type_ZMQ_StopWatch_delete(L, _index, flags) \
	obj_udata_luadelete_weak(L, _index, &(obj_type_ZMQ_StopWatch), flags)
#define obj_type_ZMQ_StopWatch_push(L, obj, flags) \
	obj_udata_luapush_weak(L, (void *)obj, &(obj_type_ZMQ_StopWatch), flags)




static const char zmq_ffi_lua_code[] = "local error = error\n"
"local type = type\n"
"local tonumber = tonumber\n"
"local tostring = tostring\n"
"local rawset = rawset\n"
"\n"
"-- try loading luajit's ffi\n"
"local stat, ffi=pcall(require,\"ffi\")\n"
"if not stat then\n"
"	return\n"
"end\n"
"\n"
"local _M, _priv, udata_new = ...\n"
"\n"
"local band = bit.band\n"
"local d_getmetatable = debug.getmetatable\n"
"local d_setmetatable = debug.setmetatable\n"
"\n"
"local OBJ_UDATA_FLAG_OWN		= 1\n"
"local OBJ_UDATA_FLAG_LOOKUP	= 2\n"
"local OBJ_UDATA_LAST_FLAG		= OBJ_UDATA_FLAG_LOOKUP\n"
"\n"
"local OBJ_TYPE_FLAG_WEAK_REF	= 1\n"
"local OBJ_TYPE_SIMPLE					= 2\n"
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
"]])\n"
"\n"
"-- cache mapping of cdata to userdata\n"
"local weak_objects = setmetatable({}, { __mode = \"v\" })\n"
"\n"
"local function obj_udata_luacheck_internal(obj, type_mt, not_delete)\n"
"	local obj_mt = d_getmetatable(obj)\n"
"	if obj_mt == type_mt then\n"
"		-- convert userdata to cdata.\n"
"		return ffi.cast(\"obj_udata *\", obj)\n"
"	end\n"
"	if not_delete then\n"
"		error(\"(expected `\" .. type_mt['.name'] .. \"`, got \" .. type(obj) .. \")\", 3)\n"
"	end\n"
"end\n"
"\n"
"local function obj_udata_luacheck(obj, type_mt)\n"
"	local ud = obj_udata_luacheck_internal(obj, type_mt, true)\n"
"	return ud.obj\n"
"end\n"
"\n"
"local function obj_udata_to_cdata(objects, ud_obj, c_type, ud_mt)\n"
"	-- convert userdata to cdata.\n"
"	local c_obj = ffi.cast(c_type, obj_udata_luacheck(ud_obj, ud_mt))\n"
"	-- cache converted cdata\n"
"	rawset(objects, ud_obj, c_obj)\n"
"	return c_obj\n"
"end\n"
"\n"
"local function obj_udata_luadelete(ud_obj, type_mt)\n"
"	local ud = obj_udata_luacheck_internal(ud_obj, type_mt, false)\n"
"	if not ud then return nil, 0 end\n"
"	local obj, flags = ud.obj, ud.flags\n"
"	-- null userdata.\n"
"	ud.obj = nil\n"
"	ud.flags = 0\n"
"	-- invalid userdata, by setting the metatable to nil.\n"
"	d_setmetatable(ud_obj, nil)\n"
"	return obj, flags\n"
"end\n"
"\n"
"local function obj_udata_luapush(obj, type_mt, obj_type, flags)\n"
"	if obj == nil then return end\n"
"\n"
"	-- apply type's dynamic caster.\n"
"	if obj_type.dcaster ~= nil then\n"
"		local obj_ptr = ffi.new(\"void *[1]\", obj)\n"
"		local type_ptr = ffi.new(\"obj_type *[1]\", obj_type)\n"
"		obj_type.dcaster(obj_ptr, type_ptr)\n"
"		obj = obj_ptr[1]\n"
"		type = type_ptr[1]\n"
"	end\n"
"\n"
"	-- create new userdata\n"
"	local ud_obj = udata_new(ffi.sizeof\"obj_udata\", type_mt)\n"
"	local ud = ffi.cast(\"obj_udata *\", ud_obj)\n"
"	-- init. object\n"
"	ud.obj = obj\n"
"	ud.flags = flags\n"
"\n"
"	return ud_obj\n"
"end\n"
"\n"
"local function obj_udata_luadelete_weak(ud_obj, type_mt)\n"
"	local ud = obj_udata_luacheck_internal(ud_obj, type_mt, false)\n"
"	if not ud then return nil, 0 end\n"
"	local obj, flags = ud.obj, ud.flags\n"
"	-- null userdata.\n"
"	ud.obj = nil\n"
"	ud.flags = 0\n"
"	-- invalid userdata, by setting the metatable to nil.\n"
"	d_setmetatable(ud_obj, nil)\n"
"	-- remove object from weak ref. table.\n"
"	local obj_key = tonumber(ffi.cast('uintptr_t', obj))\n"
"	weak_objects[obj_key] = nil\n"
"	return obj, flags\n"
"end\n"
"\n"
"local function obj_udata_luapush_weak(obj, type_mt, obj_type, flags)\n"
"	if obj == nil then return end\n"
"\n"
"	-- apply type's dynamic caster.\n"
"	if obj_type.dcaster ~= nil then\n"
"		local obj_ptr = ffi.new(\"void *[1]\", obj)\n"
"		local type_ptr = ffi.new(\"obj_type *[1]\", obj_type)\n"
"		obj_type.dcaster(obj_ptr, type_ptr)\n"
"		obj = obj_ptr[1]\n"
"		type = type_ptr[1]\n"
"	end\n"
"\n"
"	-- lookup object in weak ref. table.\n"
"	local obj_key = tonumber(ffi.cast('uintptr_t', obj))\n"
"	local ud_obj = weak_objects[obj_key]\n"
"	if ud_obj ~= nil then return ud_obj end\n"
"\n"
"	-- create new userdata\n"
"	ud_obj = udata_new(ffi.sizeof\"obj_udata\", type_mt)\n"
"	local ud = ffi.cast(\"obj_udata *\", ud_obj)\n"
"	-- init. object\n"
"	ud.obj = obj\n"
"	ud.flags = flags\n"
"\n"
"	-- cache weak reference to object.\n"
"	weak_objects[obj_key] = ud_obj\n"
"\n"
"	return ud_obj\n"
"end\n"
"\n"
"local function obj_simple_udata_luacheck(ud_obj, type_mt)\n"
"	local obj_mt = d_getmetatable(ud_obj)\n"
"	if obj_mt == type_mt then\n"
"		-- convert userdata to cdata.\n"
"		return ffi.cast(\"void *\", ud_obj)\n"
"	end\n"
"	error(\"(expected `\" .. type_mt['.name'] .. \"`, got \" .. type(ud_obj) .. \")\", 3)\n"
"end\n"
"\n"
"local function obj_simple_udata_to_cdata(objects, ud_obj, c_type, ud_mt)\n"
"	-- convert userdata to cdata.\n"
"	local c_obj = ffi.cast(c_type, obj_simple_udata_luacheck(ud_obj, ud_mt))\n"
"	-- cache converted cdata\n"
"	rawset(objects, ud_obj, c_obj)\n"
"	return c_obj\n"
"end\n"
"\n"
"local function obj_simple_udata_luadelete(ud_obj, type_mt)\n"
"	local c_obj = obj_simple_udata_luacheck(ud_obj, type_mt)\n"
"	-- invalid userdata, by setting the metatable to nil.\n"
"	d_setmetatable(ud_obj, nil)\n"
"	return c_obj, OBJ_UDATA_FLAG_OWN\n"
"end\n"
"\n"
"local function obj_simple_udata_luapush(c_obj, size, type_mt)\n"
"	if c_obj == nil then return end\n"
"\n"
"	-- create new userdata\n"
"	local ud_obj = udata_new(size, type_mt)\n"
"	local cdata = ffi.cast(\"void *\", ud_obj)\n"
"	-- init. object\n"
"	ffi.copy(cdata, c_obj, size)\n"
"\n"
"	return ud_obj, cdata\n"
"end\n"
"\n"
"ffi.cdef[[\n"
"typedef struct zmq_msg_t zmq_msg_t;\n"
"typedef struct ZMQ_Socket ZMQ_Socket;\n"
"typedef struct ZMQ_Poller ZMQ_Poller;\n"
"typedef struct ZMQ_Ctx ZMQ_Ctx;\n"
"typedef struct ZMQ_StopWatch ZMQ_StopWatch;\n"
"\n"
"]]\n"
"\n"
"ffi.cdef[[\n"
"typedef const char * (*get_zmq_strerror_func)();\n"
"\n"
"typedef int ZMQ_Error;\n"
"\n"
"\n"
"struct zmq_msg_t\n"
"{\n"
"	void *content;\n"
"	unsigned char flags;\n"
"	unsigned char vsm_size;\n"
"	unsigned char vsm_data [30]; /* that '30' is from 'MAX_VSM_SIZE' */\n"
"};\n"
"\n"
"typedef void (zmq_free_fn) (void *data, void *hint);\n"
"\n"
"int zmq_msg_init (zmq_msg_t *msg);\n"
"int zmq_msg_init_size (zmq_msg_t *msg, size_t size);\n"
"int zmq_msg_init_data (zmq_msg_t *msg, void *data, size_t size, zmq_free_fn *ffn, void *hint);\n"
"\n"
"\n"
"ZMQ_Error zmq_msg_close(zmq_msg_t * this1);\n"
"\n"
"ZMQ_Error zmq_msg_close(zmq_msg_t * this1);\n"
"\n"
"ZMQ_Error zmq_msg_move(zmq_msg_t * this1, zmq_msg_t * src2);\n"
"\n"
"ZMQ_Error zmq_msg_copy(zmq_msg_t * this1, zmq_msg_t * src2);\n"
"\n"
"void * zmq_msg_data(zmq_msg_t * this1);\n"
"\n"
"size_t zmq_msg_size(zmq_msg_t * this1);\n"
"\n"
"ZMQ_Error zmq_close(ZMQ_Socket * this1);\n"
"\n"
"ZMQ_Error zmq_bind(ZMQ_Socket * this1, const char * addr2);\n"
"\n"
"ZMQ_Error zmq_connect(ZMQ_Socket * this1, const char * addr2);\n"
"\n"
"int zmq_setsockopt (void *s, int option, const void *optval, size_t optvallen);\n"
"int zmq_getsockopt (void *s, int option, void *optval, size_t *optvallen);\n"
"\n"
"ZMQ_Error zmq_send(ZMQ_Socket * this1, zmq_msg_t * msg2, int flags3);\n"
"\n"
"typedef ZMQ_Error (*simple_zmq_send_func)(ZMQ_Socket *sock, const char *data, size_t data_len, int flags);\n"
"\n"
"ZMQ_Error zmq_recv(ZMQ_Socket * this1, zmq_msg_t * msg2, int flags3);\n"
"\n"
"typedef int socket_t;\n"
"typedef struct zmq_pollitem_t {\n"
"	ZMQ_Socket *socket;\n"
"	socket_t fd;\n"
"	short events;\n"
"	short revents;\n"
"} zmq_pollitem_t;\n"
"\n"
"int zmq_poll(zmq_pollitem_t *items, int nitems, long timeout);\n"
"\n"
"struct ZMQ_Poller {\n"
"	zmq_pollitem_t *items;\n"
"	int    next;\n"
"	int    count;\n"
"	int    free_list;\n"
"	int    len;\n"
"};\n"
"\n"
"typedef int (*poller_find_sock_item_func)(ZMQ_Poller *poller, ZMQ_Socket *sock);\n"
"\n"
"typedef int (*poller_find_fd_item_func)(ZMQ_Poller *poller, socket_t fd);\n"
"\n"
"typedef int (*poller_get_free_item_func)(ZMQ_Poller *poller);\n"
"\n"
"typedef int (*poller_poll_func)(ZMQ_Poller *poller, long timeout);\n"
"\n"
"typedef void (*poller_remove_item_func)(ZMQ_Poller *poller, int idx);\n"
"\n"
"ZMQ_Error zmq_term(ZMQ_Ctx * this1);\n"
"\n"
"ZMQ_Socket * zmq_socket(ZMQ_Ctx * this1, int type2);\n"
"\n"
"ZMQ_StopWatch * zmq_stopwatch_start();\n"
"\n"
"unsigned long zmq_stopwatch_stop(ZMQ_StopWatch * this1);\n"
"\n"
"ZMQ_Ctx * zmq_init(int io_threads1);\n"
"\n"
"ZMQ_Error zmq_device(int device1, ZMQ_Socket * insock2, ZMQ_Socket * outsock3);\n"
"\n"
"ZMQ_StopWatch * zmq_stopwatch_start();\n"
"\n"
"void zmq_sleep(int seconds_1);\n"
"\n"
"\n"
"]]\n"
"\n"
"local _pub = {}\n"
"local _meth = {}\n"
"for obj_name,mt in pairs(_priv) do\n"
"	if type(mt) == 'table' and mt.__index then\n"
"		_meth[obj_name] = mt.__index\n"
"	end\n"
"end\n"
"_pub.zmq = _M\n"
"for obj_name,pub in pairs(_M) do\n"
"	_pub[obj_name] = pub\n"
"end\n"
"\n"
"\n"
"local obj_type_zmq_msg_t_check\n"
"local obj_type_zmq_msg_t_delete\n"
"local obj_type_zmq_msg_t_push\n"
"\n"
"(function()\n"
"local zmq_msg_t_mt = _priv.zmq_msg_t\n"
"local zmq_msg_t_objects = setmetatable({}, { __mode = \"k\",\n"
"__index = function(objects, ud_obj)\n"
"	return obj_simple_udata_to_cdata(objects, ud_obj, \"zmq_msg_t *\", zmq_msg_t_mt)\n"
"end,\n"
"})\n"
"function obj_type_zmq_msg_t_check(ud_obj)\n"
"	return zmq_msg_t_objects[ud_obj]\n"
"end\n"
"\n"
"function obj_type_zmq_msg_t_delete(ud_obj)\n"
"	zmq_msg_t_objects[ud_obj] = nil\n"
"	return obj_simple_udata_luadelete(ud_obj, zmq_msg_t_mt)\n"
"end\n"
"\n"
"local zmq_msg_t_sizeof = ffi.sizeof\"zmq_msg_t\"\n"
"function obj_type_zmq_msg_t_push(c_obj)\n"
"	local ud_obj, cdata = obj_simple_udata_luapush(c_obj, zmq_msg_t_sizeof, zmq_msg_t_mt)\n"
"	zmq_msg_t_objects[ud_obj] = cdata\n"
"	return ud_obj\n"
"end\n"
"end)()\n"
"\n"
"\n"
"local obj_type_ZMQ_Socket_check\n"
"local obj_type_ZMQ_Socket_delete\n"
"local obj_type_ZMQ_Socket_push\n"
"\n"
"(function()\n"
"local ZMQ_Socket_mt = _priv.ZMQ_Socket\n"
"local ZMQ_Socket_objects = setmetatable({}, { __mode = \"k\",\n"
"__index = function(objects, ud_obj)\n"
"	return obj_udata_to_cdata(objects, ud_obj, \"ZMQ_Socket *\", ZMQ_Socket_mt)\n"
"end,\n"
"})\n"
"function obj_type_ZMQ_Socket_check(ud_obj)\n"
"	return ZMQ_Socket_objects[ud_obj]\n"
"end\n"
"\n"
"function obj_type_ZMQ_Socket_delete(ud_obj)\n"
"	ZMQ_Socket_objects[ud_obj] = nil\n"
"	return obj_udata_luadelete_weak(ud_obj, ZMQ_Socket_mt)\n"
"end\n"
"\n"
"local ZMQ_Socket_type = ffi.cast(\"obj_type *\", ZMQ_Socket_mt[\".type\"])\n"
"function obj_type_ZMQ_Socket_push(c_obj, flags)\n"
"	local ud_obj = obj_udata_luapush_weak(c_obj, ZMQ_Socket_mt, ZMQ_Socket_type, flags)\n"
"	ZMQ_Socket_objects[ud_obj] = c_obj\n"
"	return ud_obj\n"
"end\n"
"end)()\n"
"\n"
"\n"
"local obj_type_ZMQ_Poller_check\n"
"local obj_type_ZMQ_Poller_delete\n"
"local obj_type_ZMQ_Poller_push\n"
"\n"
"(function()\n"
"local ZMQ_Poller_mt = _priv.ZMQ_Poller\n"
"local ZMQ_Poller_objects = setmetatable({}, { __mode = \"k\",\n"
"__index = function(objects, ud_obj)\n"
"	return obj_simple_udata_to_cdata(objects, ud_obj, \"ZMQ_Poller *\", ZMQ_Poller_mt)\n"
"end,\n"
"})\n"
"function obj_type_ZMQ_Poller_check(ud_obj)\n"
"	return ZMQ_Poller_objects[ud_obj]\n"
"end\n"
"\n"
"function obj_type_ZMQ_Poller_delete(ud_obj)\n"
"	ZMQ_Poller_objects[ud_obj] = nil\n"
"	return obj_simple_udata_luadelete(ud_obj, ZMQ_Poller_mt)\n"
"end\n"
"\n"
"local ZMQ_Poller_sizeof = ffi.sizeof\"ZMQ_Poller\"\n"
"function obj_type_ZMQ_Poller_push(c_obj)\n"
"	local ud_obj, cdata = obj_simple_udata_luapush(c_obj, ZMQ_Poller_sizeof, ZMQ_Poller_mt)\n"
"	ZMQ_Poller_objects[ud_obj] = cdata\n"
"	return ud_obj\n"
"end\n"
"end)()\n"
"\n"
"\n"
"local obj_type_ZMQ_Ctx_check\n"
"local obj_type_ZMQ_Ctx_delete\n"
"local obj_type_ZMQ_Ctx_push\n"
"\n"
"(function()\n"
"local ZMQ_Ctx_mt = _priv.ZMQ_Ctx\n"
"local ZMQ_Ctx_objects = setmetatable({}, { __mode = \"k\",\n"
"__index = function(objects, ud_obj)\n"
"	return obj_udata_to_cdata(objects, ud_obj, \"ZMQ_Ctx *\", ZMQ_Ctx_mt)\n"
"end,\n"
"})\n"
"function obj_type_ZMQ_Ctx_check(ud_obj)\n"
"	return ZMQ_Ctx_objects[ud_obj]\n"
"end\n"
"\n"
"function obj_type_ZMQ_Ctx_delete(ud_obj)\n"
"	ZMQ_Ctx_objects[ud_obj] = nil\n"
"	return obj_udata_luadelete_weak(ud_obj, ZMQ_Ctx_mt)\n"
"end\n"
"\n"
"local ZMQ_Ctx_type = ffi.cast(\"obj_type *\", ZMQ_Ctx_mt[\".type\"])\n"
"function obj_type_ZMQ_Ctx_push(c_obj, flags)\n"
"	local ud_obj = obj_udata_luapush_weak(c_obj, ZMQ_Ctx_mt, ZMQ_Ctx_type, flags)\n"
"	ZMQ_Ctx_objects[ud_obj] = c_obj\n"
"	return ud_obj\n"
"end\n"
"end)()\n"
"\n"
"\n"
"local obj_type_ZMQ_StopWatch_check\n"
"local obj_type_ZMQ_StopWatch_delete\n"
"local obj_type_ZMQ_StopWatch_push\n"
"\n"
"(function()\n"
"local ZMQ_StopWatch_mt = _priv.ZMQ_StopWatch\n"
"local ZMQ_StopWatch_objects = setmetatable({}, { __mode = \"k\",\n"
"__index = function(objects, ud_obj)\n"
"	return obj_udata_to_cdata(objects, ud_obj, \"ZMQ_StopWatch *\", ZMQ_StopWatch_mt)\n"
"end,\n"
"})\n"
"function obj_type_ZMQ_StopWatch_check(ud_obj)\n"
"	return ZMQ_StopWatch_objects[ud_obj]\n"
"end\n"
"\n"
"function obj_type_ZMQ_StopWatch_delete(ud_obj)\n"
"	ZMQ_StopWatch_objects[ud_obj] = nil\n"
"	return obj_udata_luadelete_weak(ud_obj, ZMQ_StopWatch_mt)\n"
"end\n"
"\n"
"local ZMQ_StopWatch_type = ffi.cast(\"obj_type *\", ZMQ_StopWatch_mt[\".type\"])\n"
"function obj_type_ZMQ_StopWatch_push(c_obj, flags)\n"
"	local ud_obj = obj_udata_luapush_weak(c_obj, ZMQ_StopWatch_mt, ZMQ_StopWatch_type, flags)\n"
"	ZMQ_StopWatch_objects[ud_obj] = c_obj\n"
"	return ud_obj\n"
"end\n"
"end)()\n"
"\n"
"\n"
"local get_zmq_strerror = ffi.new(\"get_zmq_strerror_func\", _priv[\"get_zmq_strerror\"])\n"
"\n"
"local simple_zmq_send = ffi.new(\"simple_zmq_send_func\", _priv[\"simple_zmq_send\"])\n"
"\n"
"local poller_find_sock_item = ffi.new(\"poller_find_sock_item_func\", _priv[\"poller_find_sock_item\"])\n"
"\n"
"local poller_find_fd_item = ffi.new(\"poller_find_fd_item_func\", _priv[\"poller_find_fd_item\"])\n"
"\n"
"local poller_get_free_item = ffi.new(\"poller_get_free_item_func\", _priv[\"poller_get_free_item\"])\n"
"\n"
"local poller_poll = ffi.new(\"poller_poll_func\", _priv[\"poller_poll\"])\n"
"\n"
"local poller_remove_item = ffi.new(\"poller_remove_item_func\", _priv[\"poller_remove_item\"])\n"
"\n"
"local os_lib_table = {\n"
"	[\"Windows\"] = \"libzmq\",\n"
"}\n"
"local C = ffi.load(os_lib_table[ffi.os] or \"zmq\")\n"
"\n"
"local C_get_zmq_strerror = get_zmq_strerror\n"
"-- make nicer wrapper for exported error function.\n"
"local function get_zmq_strerror()\n"
"	return ffi.string(C_get_zmq_strerror())\n"
"end\n"
"\n"
"local function error_code__ZMQ_Error__push(err)\n"
"  local err_str\n"
"	if(-1 == err) then\n"
"		err_str = get_zmq_strerror();\n"
"	end\n"
"\n"
"	return err_str\n"
"end\n"
"\n"
"\n"
"-- Start \"zmq_msg_t\" FFI interface\n"
"-- method: __gc\n"
"function _priv.zmq_msg_t.__gc(self)\n"
"  local this1,this_flags1 = obj_type_zmq_msg_t_delete(self)\n"
"  if(band(this_flags1,OBJ_UDATA_FLAG_OWN) == 0) then return end\n"
"  local rc_zmq_msg_close1\n"
"  rc_zmq_msg_close1 = C.zmq_msg_close(this1)\n"
"  -- check for error.\n"
"  local rc_zmq_msg_close1_err\n"
"  if (-1 == rc_zmq_msg_close1) then\n"
"    rc_zmq_msg_close1_err =   error_code__ZMQ_Error__push(rc_zmq_msg_close1)\n"
"    rc_zmq_msg_close1 = nil\n"
"  else\n"
"    rc_zmq_msg_close1 = true\n"
"  end\n"
"  return rc_zmq_msg_close1, rc_zmq_msg_close1_err\n"
"end\n"
"\n"
"-- method: close\n"
"function _meth.zmq_msg_t.close(self)\n"
"  local this1 = obj_type_zmq_msg_t_check(self)\n"
"  local rc_zmq_msg_close1\n"
"  rc_zmq_msg_close1 = C.zmq_msg_close(this1)\n"
"  -- check for error.\n"
"  local rc_zmq_msg_close1_err\n"
"  if (-1 == rc_zmq_msg_close1) then\n"
"    rc_zmq_msg_close1_err =   error_code__ZMQ_Error__push(rc_zmq_msg_close1)\n"
"    rc_zmq_msg_close1 = nil\n"
"  else\n"
"    rc_zmq_msg_close1 = true\n"
"  end\n"
"  return rc_zmq_msg_close1, rc_zmq_msg_close1_err\n"
"end\n"
"\n"
"-- method: move\n"
"function _meth.zmq_msg_t.move(self, src2)\n"
"  local this1 = obj_type_zmq_msg_t_check(self)\n"
"  src2 = obj_type_zmq_msg_t_check(src2)\n"
"  local rc_zmq_msg_move1\n"
"  rc_zmq_msg_move1 = C.zmq_msg_move(this1, src2)\n"
"  -- check for error.\n"
"  local rc_zmq_msg_move1_err\n"
"  if (-1 == rc_zmq_msg_move1) then\n"
"    rc_zmq_msg_move1_err =   error_code__ZMQ_Error__push(rc_zmq_msg_move1)\n"
"    rc_zmq_msg_move1 = nil\n"
"  else\n"
"    rc_zmq_msg_move1 = true\n"
"  end\n"
"  return rc_zmq_msg_move1, rc_zmq_msg_move1_err\n"
"end\n"
"\n"
"-- method: copy\n"
"function _meth.zmq_msg_t.copy(self, src2)\n"
"  local this1 = obj_type_zmq_msg_t_check(self)\n"
"  src2 = obj_type_zmq_msg_t_check(src2)\n"
"  local rc_zmq_msg_copy1\n"
"  rc_zmq_msg_copy1 = C.zmq_msg_copy(this1, src2)\n"
"  -- check for error.\n"
"  local rc_zmq_msg_copy1_err\n"
"  if (-1 == rc_zmq_msg_copy1) then\n"
"    rc_zmq_msg_copy1_err =   error_code__ZMQ_Error__push(rc_zmq_msg_copy1)\n"
"    rc_zmq_msg_copy1 = nil\n"
"  else\n"
"    rc_zmq_msg_copy1 = true\n"
"  end\n"
"  return rc_zmq_msg_copy1, rc_zmq_msg_copy1_err\n"
"end\n"
"\n"
"-- method: set_data\n"
"function _meth.zmq_msg_t.set_data(self, data2)\n"
"  local this1 = obj_type_zmq_msg_t_check(self)\n"
"  local data_len2 = #data2\n"
"  local err1\n"
"	-- check message data size.\n"
"	if (C.zmq_msg_size(this1) ~= data_len2) then\n"
"		-- need to resize message.\n"
"		C.zmq_msg_close(this1); -- close old message, to free old data.\n"
"		err1 = C.zmq_msg_init_size(this1, data_len2); -- re-initialize message.\n"
"		if (0 ~= err1) then\n"
"			error(\"set_data() failed: \" .. get_zmq_strerror());\n"
"		end\n"
"	end\n"
"	-- copy data into message\n"
"	ffi.copy(C.zmq_msg_data(this1), data2, data_len2);\n"
"\n"
"  -- check for error.\n"
"  local err1_err\n"
"  if (-1 == err1) then\n"
"    err1_err =   error_code__ZMQ_Error__push(err1)\n"
"    err1 = nil\n"
"  else\n"
"    err1 = true\n"
"  end\n"
"  return err1, err1_err\n"
"end\n"
"\n"
"-- method: data\n"
"function _meth.zmq_msg_t.data(self)\n"
"  local this1 = obj_type_zmq_msg_t_check(self)\n"
"  local rc_zmq_msg_data1\n"
"  rc_zmq_msg_data1 = C.zmq_msg_data(this1)\n"
"  rc_zmq_msg_data1 = rc_zmq_msg_data1\n"
"  return rc_zmq_msg_data1\n"
"end\n"
"\n"
"-- method: set_size\n"
"function _meth.zmq_msg_t.set_size(self, size2)\n"
"  local this1 = obj_type_zmq_msg_t_check(self)\n"
"  \n"
"  local err1\n"
"	-- check message data size.\n"
"	if (C.zmq_msg_size(this1) ~= size2) then\n"
"		-- need to resize message.\n"
"		C.zmq_msg_close(this1); -- close old message, to free old data.\n"
"		err1 = C.zmq_msg_init_size(this1, size2); -- re-initialize message.\n"
"		if (0 ~= err1) then\n"
"			error(\"set_size() failed: \" .. get_zmq_strerror());\n"
"		end\n"
"	end\n"
"\n"
"  -- check for error.\n"
"  local err1_err\n"
"  if (-1 == err1) then\n"
"    err1_err =   error_code__ZMQ_Error__push(err1)\n"
"    err1 = nil\n"
"  else\n"
"    err1 = true\n"
"  end\n"
"  return err1, err1_err\n"
"end\n"
"\n"
"-- method: size\n"
"function _meth.zmq_msg_t.size(self)\n"
"  local this1 = obj_type_zmq_msg_t_check(self)\n"
"  local rc_zmq_msg_size1\n"
"  rc_zmq_msg_size1 = C.zmq_msg_size(this1)\n"
"  rc_zmq_msg_size1 = rc_zmq_msg_size1\n"
"  return rc_zmq_msg_size1\n"
"end\n"
"\n"
"-- method: __tostring\n"
"function _priv.zmq_msg_t.__tostring(self)\n"
"  local this1 = obj_type_zmq_msg_t_check(self)\n"
"  local data_len1 = 0\n"
"  local data1\n"
"	data1 = C.zmq_msg_data(this1);\n"
"	data_len1 = C.zmq_msg_size(this1);\n"
"\n"
"  data1 = ((nil ~= data1) and ffi.string(data1,data_len1))\n"
"  return data1\n"
"end\n"
"\n"
"-- End \"zmq_msg_t\" FFI interface\n"
"\n"
"\n"
"-- Start \"ZMQ_Socket\" FFI interface\n"
"-- method: close\n"
"function _meth.ZMQ_Socket.close(self)\n"
"  local this1,this_flags1 = obj_type_ZMQ_Socket_delete(self)\n"
"  if(band(this_flags1,OBJ_UDATA_FLAG_OWN) == 0) then return end\n"
"  local rc_zmq_close1\n"
"  rc_zmq_close1 = C.zmq_close(this1)\n"
"  -- check for error.\n"
"  local rc_zmq_close1_err\n"
"  if (-1 == rc_zmq_close1) then\n"
"    rc_zmq_close1_err =   error_code__ZMQ_Error__push(rc_zmq_close1)\n"
"    rc_zmq_close1 = nil\n"
"  else\n"
"    rc_zmq_close1 = true\n"
"  end\n"
"  return rc_zmq_close1, rc_zmq_close1_err\n"
"end\n"
"\n"
"-- method: bind\n"
"function _meth.ZMQ_Socket.bind(self, addr2)\n"
"  local this1 = obj_type_ZMQ_Socket_check(self)\n"
"  local addr_len2 = #addr2\n"
"  local rc_zmq_bind1\n"
"  rc_zmq_bind1 = C.zmq_bind(this1, addr2)\n"
"  -- check for error.\n"
"  local rc_zmq_bind1_err\n"
"  if (-1 == rc_zmq_bind1) then\n"
"    rc_zmq_bind1_err =   error_code__ZMQ_Error__push(rc_zmq_bind1)\n"
"    rc_zmq_bind1 = nil\n"
"  else\n"
"    rc_zmq_bind1 = true\n"
"  end\n"
"  return rc_zmq_bind1, rc_zmq_bind1_err\n"
"end\n"
"\n"
"-- method: connect\n"
"function _meth.ZMQ_Socket.connect(self, addr2)\n"
"  local this1 = obj_type_ZMQ_Socket_check(self)\n"
"  local addr_len2 = #addr2\n"
"  local rc_zmq_connect1\n"
"  rc_zmq_connect1 = C.zmq_connect(this1, addr2)\n"
"  -- check for error.\n"
"  local rc_zmq_connect1_err\n"
"  if (-1 == rc_zmq_connect1) then\n"
"    rc_zmq_connect1_err =   error_code__ZMQ_Error__push(rc_zmq_connect1)\n"
"    rc_zmq_connect1 = nil\n"
"  else\n"
"    rc_zmq_connect1 = true\n"
"  end\n"
"  return rc_zmq_connect1, rc_zmq_connect1_err\n"
"end\n"
"\n"
"local option_types = {\n"
"[zmq.HWM] = 'uint64_t[1]',\n"
"[zmq.SWAP] = 'int64_t[1]',\n"
"[zmq.AFFINITY] = 'uint64_t[1]',\n"
"[zmq.IDENTITY] = 'string',\n"
"[zmq.SUBSCRIBE] = 'string',\n"
"[zmq.UNSUBSCRIBE] = 'string',\n"
"[zmq.RATE] = 'int64_t[1]',\n"
"[zmq.RECOVERY_IVL] = 'int64_t[1]',\n"
"[zmq.MCAST_LOOP] = 'int64_t[1]',\n"
"[zmq.SNDBUF] = 'uint64_t[1]',\n"
"[zmq.RCVBUF] = 'uint64_t[1]',\n"
"[zmq.RCVMORE] = 'int64_t[1]',\n"
"[zmq.FD] = 'int[1]',\n"
"[zmq.EVENTS] = 'uint32_t[1]',\n"
"[zmq.TYPE] = 'int[1]',\n"
"[zmq.LINGER] = 'int[1]',\n"
"[zmq.RECONNECT_IVL] = 'int[1]',\n"
"[zmq.BACKLOG] = 'int[1]',\n"
"}\n"
"local option_len = {}\n"
"local option_tmps = {}\n"
"for k,v in pairs(option_types) do\n"
"  if v ~= 'string' then\n"
"    option_len[k] = ffi.sizeof(v)\n"
"    option_tmps[k] = ffi.new(v, 0)\n"
"  end\n"
"end\n"
"\n"
"\n"
"-- method: setopt\n"
"function _meth.ZMQ_Socket.setopt(self, opt2, val3)\n"
"  local this1 = obj_type_ZMQ_Socket_check(self)\n"
"  \n"
"  local err1\n"
"	local ctype = option_types[opt2]\n"
"	local tval\n"
"	local tval_len = 0\n"
"	if ctype == 'string' then\n"
"		tval = tostring(val3)\n"
"		tval_len = #val3\n"
"	else\n"
"		tval = option_tmps[opt2]\n"
"		tval[0] = val3\n"
"		tval_len = option_len[opt2]\n"
"	end\n"
"	err1 = C.zmq_setsockopt(this1, opt2, tval, tval_len)\n"
"\n"
"  -- check for error.\n"
"  local err1_err\n"
"  if (-1 == err1) then\n"
"    err1_err =   error_code__ZMQ_Error__push(err1)\n"
"    err1 = nil\n"
"  else\n"
"    err1 = true\n"
"  end\n"
"  return err1, err1_err\n"
"end\n"
"\n"
"local tmp_val_len = ffi.new('size_t[1]', 4)\n"
"\n"
"-- method: getopt\n"
"function _meth.ZMQ_Socket.getopt(self, opt2)\n"
"  local this1 = obj_type_ZMQ_Socket_check(self)\n"
"  \n"
"  local val1\n"
"  local err2\n"
"	local ctype = option_types[opt2]\n"
"	local val\n"
"	local val_len = tmp_val_len\n"
"	if ctype == 'string' then\n"
"		val_len[0] = 255\n"
"		val = ffi.new('uint8_t[?]', val_len[0])\n"
"		ffi.fill(val, val_len[0])\n"
"	else\n"
"		val = option_tmps[opt2]\n"
"		val[0] = 0\n"
"		val_len[0] = option_len[opt2]\n"
"	end\n"
"	err2 = C.zmq_getsockopt(this1, opt2, val, val_len)\n"
"	if err2 == 0 then\n"
"		if ctype == 'string' then\n"
"			val_len = val_len[0]\n"
"			return ffi.string(val, val_len)\n"
"		else\n"
"			return tonumber(val[0])\n"
"		end\n"
"	end\n"
"\n"
"  err2 =   error_code__ZMQ_Error__push(err2)\n"
"  return val1, err2\n"
"end\n"
"\n"
"-- temp. values for 'events' function.\n"
"local events_tmp = ffi.new('uint32_t[1]', 0)\n"
"local events_tmp_size = ffi.sizeof('uint32_t')\n"
"local events_tmp_len = ffi.new('size_t[1]', events_tmp_size)\n"
"local ZMQ_EVENTS = _M.EVENTS\n"
"\n"
"-- method: events\n"
"function _meth.ZMQ_Socket.events(self)\n"
"  local this1 = obj_type_ZMQ_Socket_check(self)\n"
"  local events1\n"
"  local err2\n"
"	events_tmp_len[0] = events_tmp_size\n"
"	err2 = C.zmq_getsockopt(this1, ZMQ_EVENTS, events_tmp, events_tmp_len);\n"
"	events1 = events_tmp[0]\n"
"\n"
"  if not (-1 == err2) then\n"
"    events1 = events1\n"
"  else\n"
"    events1 = nil\n"
"  end\n"
"  err2 =   error_code__ZMQ_Error__push(err2)\n"
"  return events1, err2\n"
"end\n"
"\n"
"-- method: send_msg\n"
"function _meth.ZMQ_Socket.send_msg(self, msg2, flags3)\n"
"  local this1 = obj_type_ZMQ_Socket_check(self)\n"
"  msg2 = obj_type_zmq_msg_t_check(msg2)\n"
"    flags3 = flags3 or 0\n"
"  local rc_zmq_send1\n"
"  rc_zmq_send1 = C.zmq_send(this1, msg2, flags3)\n"
"  -- check for error.\n"
"  local rc_zmq_send1_err\n"
"  if (-1 == rc_zmq_send1) then\n"
"    rc_zmq_send1_err =   error_code__ZMQ_Error__push(rc_zmq_send1)\n"
"    rc_zmq_send1 = nil\n"
"  else\n"
"    rc_zmq_send1 = true\n"
"  end\n"
"  return rc_zmq_send1, rc_zmq_send1_err\n"
"end\n"
"\n"
"-- method: send\n"
"function _meth.ZMQ_Socket.send(self, data2, flags3)\n"
"  local this1 = obj_type_ZMQ_Socket_check(self)\n"
"  local data_len2 = #data2\n"
"    flags3 = flags3 or 0\n"
"  local err1\n"
"	err1 = simple_zmq_send(this1, data2, data_len2, flags3);\n"
"\n"
"  -- check for error.\n"
"  local err1_err\n"
"  if (-1 == err1) then\n"
"    err1_err =   error_code__ZMQ_Error__push(err1)\n"
"    err1 = nil\n"
"  else\n"
"    err1 = true\n"
"  end\n"
"  return err1, err1_err\n"
"end\n"
"\n"
"-- method: recv_msg\n"
"function _meth.ZMQ_Socket.recv_msg(self, msg2, flags3)\n"
"  local this1 = obj_type_ZMQ_Socket_check(self)\n"
"  msg2 = obj_type_zmq_msg_t_check(msg2)\n"
"    flags3 = flags3 or 0\n"
"  local rc_zmq_recv1\n"
"  rc_zmq_recv1 = C.zmq_recv(this1, msg2, flags3)\n"
"  -- check for error.\n"
"  local rc_zmq_recv1_err\n"
"  if (-1 == rc_zmq_recv1) then\n"
"    rc_zmq_recv1_err =   error_code__ZMQ_Error__push(rc_zmq_recv1)\n"
"    rc_zmq_recv1 = nil\n"
"  else\n"
"    rc_zmq_recv1 = true\n"
"  end\n"
"  return rc_zmq_recv1, rc_zmq_recv1_err\n"
"end\n"
"\n"
"local tmp_msg = ffi.new('zmq_msg_t')\n"
"\n"
"-- method: recv\n"
"function _meth.ZMQ_Socket.recv(self, flags2)\n"
"  local this1 = obj_type_ZMQ_Socket_check(self)\n"
"    flags2 = flags2 or 0\n"
"  local data_len1 = 0\n"
"  local data1\n"
"  local err2\n"
"	local msg = tmp_msg\n"
"	-- initialize blank message.\n"
"	if C.zmq_msg_init(msg) < 0 then\n"
"		return nil, get_zmq_strerror()\n"
"	end\n"
"\n"
"	-- receive message\n"
"	err2 = C.zmq_recv(this1, msg, flags2)\n"
"	if 0 == err2 then\n"
"		local data = ffi.string(C.zmq_msg_data(msg), C.zmq_msg_size(msg))\n"
"		-- close message\n"
"		C.zmq_msg_close(msg)\n"
"		return data\n"
"	end\n"
"\n"
"  if not (-1 == err2) then\n"
"    data1 = ((nil ~= data1) and ffi.string(data1,data_len1))\n"
"  else\n"
"    data1 = nil\n"
"  end\n"
"  err2 =   error_code__ZMQ_Error__push(err2)\n"
"	-- close message\n"
"	C.zmq_msg_close(msg)\n"
"\n"
"  return data1, err2\n"
"end\n"
"\n"
"-- End \"ZMQ_Socket\" FFI interface\n"
"\n"
"\n"
"-- Start \"ZMQ_Poller\" FFI interface\n"
"-- method: poll\n"
"function _meth.ZMQ_Poller.poll(self, timeout2)\n"
"  local this1 = obj_type_ZMQ_Poller_check(self)\n"
"  \n"
"  local err1\n"
"	-- poll for events\n"
"	err1 = poller_poll(this1, timeout2)\n"
"	if(err1 > 0) then\n"
"		this1.next = 0\n"
"	else\n"
"		this1.next = -1\n"
"	end\n"
"\n"
"  -- check for error.\n"
"  local err1_err\n"
"  if (-1 == err1) then\n"
"    err1_err =   error_code__ZMQ_Error__push(err1)\n"
"    err1 = nil\n"
"  else\n"
"    err1 = true\n"
"  end\n"
"  return err1, err1_err\n"
"end\n"
"\n"
"-- method: next_revents\n"
"function _meth.ZMQ_Poller.next_revents(self)\n"
"  local this1 = obj_type_ZMQ_Poller_check(self)\n"
"  local sock1\n"
"  local revents2\n"
"	local sock\n"
"	local idx = this1.next\n"
"	if (idx < 0) then return nil, -1 end\n"
"	local count = this1.count\n"
"	-- find next item with pending events.\n"
"	while (idx < count and this1.items[idx].revents == 0) do\n"
"		idx = idx + 1\n"
"		if (idx >= count) then\n"
"			idx = -1\n"
"			break\n"
"		end\n"
"	end\n"
"	-- did we find a pending event?\n"
"	if(idx >= 0) then\n"
"		-- push the event's sock/fd.\n"
"		if(this1.items[idx].socket ~= nil) then\n"
"			sock = obj_type_ZMQ_Socket_push(this1.items[idx].socket, 0)\n"
"		else\n"
"			sock = tonumber(this1.items[idx].fd)\n"
"		end\n"
"		revents2 = this1.items[idx].revents\n"
"		-- is this the last event.\n"
"		idx = idx + 1\n"
"		if (idx >= count) then\n"
"			idx = -1\n"
"		end\n"
"		this1.next = idx\n"
"		return sock, revents2\n"
"	end\n"
"	this1.next = idx\n"
"\n"
"  revents2 = revents2\n"
"  return sock1, revents2\n"
"end\n"
"\n"
"-- method: count\n"
"function _meth.ZMQ_Poller.count(self)\n"
"  local this1 = obj_type_ZMQ_Poller_check(self)\n"
"  local count1\n"
"	count1 = this1.count;\n"
"\n"
"  count1 = count1\n"
"  return count1\n"
"end\n"
"\n"
"-- End \"ZMQ_Poller\" FFI interface\n"
"\n"
"\n"
"-- Start \"ZMQ_Ctx\" FFI interface\n"
"-- method: term\n"
"function _meth.ZMQ_Ctx.term(self)\n"
"  local this1,this_flags1 = obj_type_ZMQ_Ctx_delete(self)\n"
"  if(band(this_flags1,OBJ_UDATA_FLAG_OWN) == 0) then return end\n"
"  local rc_zmq_term1\n"
"  rc_zmq_term1 = C.zmq_term(this1)\n"
"  -- check for error.\n"
"  local rc_zmq_term1_err\n"
"  if (-1 == rc_zmq_term1) then\n"
"    rc_zmq_term1_err =   error_code__ZMQ_Error__push(rc_zmq_term1)\n"
"    rc_zmq_term1 = nil\n"
"  else\n"
"    rc_zmq_term1 = true\n"
"  end\n"
"  return rc_zmq_term1, rc_zmq_term1_err\n"
"end\n"
"\n"
"-- method: socket\n"
"function _meth.ZMQ_Ctx.socket(self, type2)\n"
"  local this1 = obj_type_ZMQ_Ctx_check(self)\n"
"  \n"
"  local rc_zmq_socket_flags1 = OBJ_UDATA_FLAG_OWN\n"
"  local rc_zmq_socket1\n"
"  rc_zmq_socket1 = C.zmq_socket(this1, type2)\n"
"  local rc_zmq_socket1_err\n"
"  if (nil == rc_zmq_socket1) then\n"
"    rc_zmq_socket1_err =   get_zmq_strerror()\n"
"  else\n"
"    rc_zmq_socket1 =   obj_type_ZMQ_Socket_push(rc_zmq_socket1, rc_zmq_socket_flags1)\n"
"  end\n"
"  return rc_zmq_socket1, rc_zmq_socket1_err\n"
"end\n"
"\n"
"-- End \"ZMQ_Ctx\" FFI interface\n"
"\n"
"\n"
"-- Start \"ZMQ_StopWatch\" FFI interface\n"
"-- method: start\n"
"function _pub.ZMQ_StopWatch.start()\n"
"  local this_flags1 = OBJ_UDATA_FLAG_OWN\n"
"  local this1\n"
"  this1 = C.zmq_stopwatch_start()\n"
"  this1 =   obj_type_ZMQ_StopWatch_push(this1, this_flags1)\n"
"  return this1\n"
"end\n"
"\n"
"-- method: stop\n"
"function _meth.ZMQ_StopWatch.stop(self)\n"
"  local this1,this_flags1 = obj_type_ZMQ_StopWatch_delete(self)\n"
"  if(band(this_flags1,OBJ_UDATA_FLAG_OWN) == 0) then return end\n"
"  local usecs1\n"
"  usecs1 = C.zmq_stopwatch_stop(this1)\n"
"  usecs1 = tonumber(usecs1)\n"
"  return usecs1\n"
"end\n"
"\n"
"-- End \"ZMQ_StopWatch\" FFI interface\n"
"\n"
"-- method: init\n"
"function _pub.zmq.init(io_threads1)\n"
"  \n"
"  local rc_zmq_init_flags1 = OBJ_UDATA_FLAG_OWN\n"
"  local rc_zmq_init1\n"
"  rc_zmq_init1 = C.zmq_init(io_threads1)\n"
"  local rc_zmq_init1_err\n"
"  if (nil == rc_zmq_init1) then\n"
"    rc_zmq_init1_err =   get_zmq_strerror()\n"
"  else\n"
"    rc_zmq_init1 =   obj_type_ZMQ_Ctx_push(rc_zmq_init1, rc_zmq_init_flags1)\n"
"  end\n"
"  return rc_zmq_init1, rc_zmq_init1_err\n"
"end\n"
"\n"
"-- method: init_ctx\n"
"function _pub.zmq.init_ctx(ptr1)\n"
"  local ctx1\n"
"	local p_type = type(ptr1)\n"
"	if p_type == 'userdata' then\n"
"		ctx1 = ffi.cast('void *', ptr1);\n"
"	elseif p_type == 'cdata' then\n"
"		ctx1 = ptr1;\n"
"	else\n"
"		return error(\"expected lightuserdata/cdata<void *>\");\n"
"	end\n"
"\n"
"  local ctx1_err\n"
"  if (nil == ctx1) then\n"
"    ctx1_err =   get_zmq_strerror()\n"
"  else\n"
"    ctx1 =   obj_type_ZMQ_Ctx_push(ctx1, 0)\n"
"  end\n"
"  return ctx1, ctx1_err\n"
"end\n"
"\n"
"-- method: device\n"
"function _pub.zmq.device(device1, insock2, outsock3)\n"
"  \n"
"  insock2 = obj_type_ZMQ_Socket_check(insock2)\n"
"  outsock3 = obj_type_ZMQ_Socket_check(outsock3)\n"
"  local rc_zmq_device1\n"
"  rc_zmq_device1 = C.zmq_device(device1, insock2, outsock3)\n"
"  -- check for error.\n"
"  local rc_zmq_device1_err\n"
"  if (-1 == rc_zmq_device1) then\n"
"    rc_zmq_device1_err =   error_code__ZMQ_Error__push(rc_zmq_device1)\n"
"    rc_zmq_device1 = nil\n"
"  else\n"
"    rc_zmq_device1 = true\n"
"  end\n"
"  return rc_zmq_device1, rc_zmq_device1_err\n"
"end\n"
"\n"
"-- method: stopwatch_start\n"
"function _pub.zmq.stopwatch_start()\n"
"  local rc_zmq_stopwatch_start_flags1 = OBJ_UDATA_FLAG_OWN\n"
"  local rc_zmq_stopwatch_start1\n"
"  rc_zmq_stopwatch_start1 = C.zmq_stopwatch_start()\n"
"  rc_zmq_stopwatch_start1 =   obj_type_ZMQ_StopWatch_push(rc_zmq_stopwatch_start1, rc_zmq_stopwatch_start_flags1)\n"
"  return rc_zmq_stopwatch_start1\n"
"end\n"
"\n"
"-- method: sleep\n"
"function _pub.zmq.sleep(seconds_1)\n"
"  \n"
"  C.zmq_sleep(seconds_1)\n"
"  return \n"
"end\n"
"\n"
"";

/* detect zmq version >= 2.1.0 */
#define VERSION_2_1 0
#if defined(ZMQ_VERSION)
#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION(2,1,0))
#undef VERSION_2_1
#define VERSION_2_1 1
#endif
#endif

/* detect really old ZeroMQ 2.0.x series. */
#if !defined(ZMQ_RCVMORE)
#error "Your version of ZeroMQ is too old.  Please upgrade to version 2.1 or to the latest 2.0.x"
#endif

typedef struct ZMQ_Socket ZMQ_Socket;

#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

/* socket option types. */
#define OPT_TYPE_NONE		0
#define OPT_TYPE_INT		1
#define OPT_TYPE_UINT32	2
#define OPT_TYPE_UINT64	3
#define OPT_TYPE_INT64	4
#define OPT_TYPE_STR		5
#define OPT_TYPE_FD			6

static const int opt_types[] = {
	OPT_TYPE_NONE,		/*  0 unused */
	OPT_TYPE_UINT64,	/*  1 ZMQ_HWM */
	OPT_TYPE_NONE,		/*  2 unused */
	OPT_TYPE_INT64,		/*  3 ZMQ_SWAP */
	OPT_TYPE_UINT64,	/*  4 ZMQ_AFFINITY */
	OPT_TYPE_STR,			/*  5 ZMQ_IDENTITY */
	OPT_TYPE_STR,			/*  6 ZMQ_SUBSCRIBE */
	OPT_TYPE_STR,			/*  7 ZMQ_UNSUBSCRIBE */
	OPT_TYPE_INT64,		/*  8 ZMQ_RATE */
	OPT_TYPE_INT64,		/*  9 ZMQ_RECOVERY_IVL */
	OPT_TYPE_INT64,		/* 10 ZMQ_MCAST_LOOP */
	OPT_TYPE_UINT64,	/* 11 ZMQ_SNDBUF */
	OPT_TYPE_UINT64,	/* 12 ZMQ_RCVBUF */
	OPT_TYPE_INT64,		/* 13 ZMQ_RCVMORE */

#if VERSION_2_1
	OPT_TYPE_FD,			/* 14 ZMQ_FD */
	OPT_TYPE_UINT32,	/* 15 ZMQ_EVENTS */
	OPT_TYPE_INT,			/* 16 ZMQ_TYPE */
	OPT_TYPE_INT,			/* 17 ZMQ_LINGER */
	OPT_TYPE_INT,			/* 18 ZMQ_RECONNECT_IVL */
	OPT_TYPE_INT,			/* 19 ZMQ_BACKLOG */
#endif
};
#if VERSION_2_1
#define MAX_OPTS ZMQ_BACKLOG
#else
#define MAX_OPTS ZMQ_RCVMORE
#endif


static ZMQ_Error simple_zmq_send(ZMQ_Socket *sock, const char *data, size_t data_len, int flags) {
	ZMQ_Error err;
	zmq_msg_t msg;
	/* initialize message */
	err = zmq_msg_init_size(&msg, data_len);
	if(0 == err) {
		/* fill message */
		memcpy(zmq_msg_data(&msg), data, data_len);
		/* send message */
		err = zmq_send(sock, &msg, flags);
		/* close message */
		zmq_msg_close(&msg);
	}
	return err;
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

static int poller_find_sock_item(ZMQ_Poller *poller, ZMQ_Socket *sock) {
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

static int poller_find_fd_item(ZMQ_Poller *poller, socket_t fd) {
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

static void poller_remove_item(ZMQ_Poller *poller, int idx) {
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

static int poller_get_free_item(ZMQ_Poller *poller) {
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

static int poller_poll(ZMQ_Poller *poller, long timeout) {
	int count;
	/* remove free slots from items list. */
	count = poller_compact_items(poller);
	/* poll for events. */
	return zmq_poll(poller->items, count, timeout);
}


typedef struct ZMQ_Ctx ZMQ_Ctx;

typedef struct ZMQ_StopWatch ZMQ_StopWatch;

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




static void error_code__ZMQ_Error__push(lua_State *L, ZMQ_Error err) {
  const char *err_str = NULL;
	if(-1 == err) {
		err_str = get_zmq_strerror();
	}

	if(err_str) {
		lua_pushstring(L, err_str);
	} else {
		lua_pushnil(L);
	}
}

/* method: init */
static int zmq_msg_t__init__meth(lua_State *L) {
  int this_flags1 = OBJ_UDATA_FLAG_OWN;
  zmq_msg_t * this1;
  ZMQ_Error err2 = 0;
	zmq_msg_t tmp;
	this1 = &tmp;
	err2 = zmq_msg_init(this1);

  if(!(-1 == err2)) {
    obj_type_zmq_msg_t_push(L, this1, this_flags1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, err2);
  return 2;
}

/* method: init_size */
static int zmq_msg_t__init_size__meth(lua_State *L) {
  size_t size1 = luaL_checkinteger(L,1);
  int this_flags1 = OBJ_UDATA_FLAG_OWN;
  zmq_msg_t * this1;
  ZMQ_Error err2 = 0;
	zmq_msg_t tmp;
	this1 = &tmp;
	err2 = zmq_msg_init_size(this1, size1);

  if(!(-1 == err2)) {
    obj_type_zmq_msg_t_push(L, this1, this_flags1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, err2);
  return 2;
}

/* method: init_data */
static int zmq_msg_t__init_data__meth(lua_State *L) {
  size_t data_len1;
  const char * data1 = luaL_checklstring(L,1,&(data_len1));
  int this_flags1 = OBJ_UDATA_FLAG_OWN;
  zmq_msg_t * this1;
  ZMQ_Error err2 = 0;
	zmq_msg_t tmp;
	this1 = &tmp;
	err2 = zmq_msg_init_size(this1, data_len1);
	if(0 == err2) {
		/* fill message */
		memcpy(zmq_msg_data(this1), data1, data_len1);
	}

  if(!(-1 == err2)) {
    obj_type_zmq_msg_t_push(L, this1, this_flags1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, err2);
  return 2;
}

/* method: delete */
static int zmq_msg_t__delete__meth(lua_State *L) {
  int this_flags1 = 0;
  zmq_msg_t * this1 = obj_type_zmq_msg_t_delete(L,1,&(this_flags1));
  ZMQ_Error rc_zmq_msg_close1 = 0;
  if(!(this_flags1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
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
  zmq_msg_t * this1 = obj_type_zmq_msg_t_check(L,1);
  ZMQ_Error rc_zmq_msg_close1 = 0;
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
  zmq_msg_t * this1 = obj_type_zmq_msg_t_check(L,1);
  zmq_msg_t * src2 = obj_type_zmq_msg_t_check(L,2);
  ZMQ_Error rc_zmq_msg_move1 = 0;
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
  zmq_msg_t * this1 = obj_type_zmq_msg_t_check(L,1);
  zmq_msg_t * src2 = obj_type_zmq_msg_t_check(L,2);
  ZMQ_Error rc_zmq_msg_copy1 = 0;
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
  zmq_msg_t * this1 = obj_type_zmq_msg_t_check(L,1);
  size_t data_len2;
  const char * data2 = luaL_checklstring(L,2,&(data_len2));
  ZMQ_Error err1 = 0;
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
  zmq_msg_t * this1 = obj_type_zmq_msg_t_check(L,1);
  void * rc_zmq_msg_data1 = NULL;
  rc_zmq_msg_data1 = zmq_msg_data(this1);
  lua_pushlightuserdata(L, rc_zmq_msg_data1);
  return 1;
}

/* method: set_size */
static int zmq_msg_t__set_size__meth(lua_State *L) {
  zmq_msg_t * this1 = obj_type_zmq_msg_t_check(L,1);
  size_t size2 = luaL_checkinteger(L,2);
  ZMQ_Error err1 = 0;
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
  zmq_msg_t * this1 = obj_type_zmq_msg_t_check(L,1);
  size_t rc_zmq_msg_size1 = 0;
  rc_zmq_msg_size1 = zmq_msg_size(this1);
  lua_pushinteger(L, rc_zmq_msg_size1);
  return 1;
}

/* method: __tostring */
static int zmq_msg_t____tostring__meth(lua_State *L) {
  zmq_msg_t * this1 = obj_type_zmq_msg_t_check(L,1);
  size_t data_len1 = 0;
  const char * data1 = NULL;
	data1 = zmq_msg_data(this1);
	data_len1 = zmq_msg_size(this1);

  if(data1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, data1,data_len1);
  return 1;
}

/* method: close */
static int ZMQ_Socket__close__meth(lua_State *L) {
  int this_flags1 = 0;
  ZMQ_Socket * this1 = obj_type_ZMQ_Socket_delete(L,1,&(this_flags1));
  ZMQ_Error rc_zmq_close1 = 0;
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
  ZMQ_Socket * this1 = obj_type_ZMQ_Socket_check(L,1);
  size_t addr_len2;
  const char * addr2 = luaL_checklstring(L,2,&(addr_len2));
  ZMQ_Error rc_zmq_bind1 = 0;
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

/* method: connect */
static int ZMQ_Socket__connect__meth(lua_State *L) {
  ZMQ_Socket * this1 = obj_type_ZMQ_Socket_check(L,1);
  size_t addr_len2;
  const char * addr2 = luaL_checklstring(L,2,&(addr_len2));
  ZMQ_Error rc_zmq_connect1 = 0;
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

/* method: setopt */
static int ZMQ_Socket__setopt__meth(lua_State *L) {
  ZMQ_Socket * this1 = obj_type_ZMQ_Socket_check(L,1);
  uint32_t opt2 = luaL_checkinteger(L,2);
  ZMQ_Error err1 = 0;
	size_t val_len;
	const void *val;

#if VERSION_2_1
	socket_t fd_val;
#endif
	int int_val;
	uint32_t uint32_val;
	uint64_t uint64_val;
	int64_t int64_val;

	if(opt2 > MAX_OPTS) {
		return luaL_argerror(L, 2, "Invalid socket option.");
	}

	switch(opt_types[opt2]) {
#if VERSION_2_1
	case OPT_TYPE_FD:
		fd_val = luaL_checklong(L, 3);
		val = &fd_val;
		val_len = sizeof(fd_val);
		break;
#endif
	case OPT_TYPE_INT:
		int_val = luaL_checklong(L, 3);
		val = &int_val;
		val_len = sizeof(int_val);
		break;
	case OPT_TYPE_UINT32:
		uint32_val = luaL_checklong(L, 3);
		val = &uint32_val;
		val_len = sizeof(uint32_val);
		break;
	case OPT_TYPE_UINT64:
		uint64_val = luaL_checklong(L, 3);
		val = &uint64_val;
		val_len = sizeof(uint64_val);
		break;
	case OPT_TYPE_INT64:
		int64_val = luaL_checklong(L, 3);
		val = &int64_val;
		val_len = sizeof(int64_val);
		break;
	case OPT_TYPE_STR:
		val = luaL_checklstring(L, 3, &(val_len));
		break;
	default:
		printf("Invalid socket option type, this shouldn't happen.\n");
		abort();
		break;
	}
	err1 = zmq_setsockopt(this1, opt2, val, val_len);

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
  ZMQ_Socket * this1 = obj_type_ZMQ_Socket_check(L,1);
  uint32_t opt2 = luaL_checkinteger(L,2);
  ZMQ_Error err2 = 0;
	size_t val_len;

#if VERSION_2_1
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
#if VERSION_2_1
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
	case OPT_TYPE_STR:
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

/* method: events */
static int ZMQ_Socket__events__meth(lua_State *L) {
  ZMQ_Socket * this1 = obj_type_ZMQ_Socket_check(L,1);
  uint32_t events1 = 0;
  ZMQ_Error err2 = 0;
#if VERSION_2_1
	size_t val_len = sizeof(events1);
	err2 = zmq_getsockopt(this1, ZMQ_EVENTS, &(events1), &val_len);
#else
	luaL_error(L, "'events' method only supported in 0MQ version >= 2.1");
#endif

  if(!(-1 == err2)) {
    lua_pushinteger(L, events1);
  } else {
    lua_pushnil(L);
  }
  error_code__ZMQ_Error__push(L, err2);
  return 2;
}

/* method: send_msg */
static int ZMQ_Socket__send_msg__meth(lua_State *L) {
  ZMQ_Socket * this1 = obj_type_ZMQ_Socket_check(L,1);
  zmq_msg_t * msg2 = obj_type_zmq_msg_t_check(L,2);
  int flags3 = luaL_optinteger(L,3,0);
  ZMQ_Error rc_zmq_send1 = 0;
  rc_zmq_send1 = zmq_send(this1, msg2, flags3);
  /* check for error. */
  if((-1 == rc_zmq_send1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_send1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: send */
static int ZMQ_Socket__send__meth(lua_State *L) {
  ZMQ_Socket * this1 = obj_type_ZMQ_Socket_check(L,1);
  size_t data_len2;
  const char * data2 = luaL_checklstring(L,2,&(data_len2));
  int flags3 = luaL_optinteger(L,3,0);
  ZMQ_Error err1 = 0;
	err1 = simple_zmq_send(this1, data2, data_len2, flags3);

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

/* method: recv_msg */
static int ZMQ_Socket__recv_msg__meth(lua_State *L) {
  ZMQ_Socket * this1 = obj_type_ZMQ_Socket_check(L,1);
  zmq_msg_t * msg2 = obj_type_zmq_msg_t_check(L,2);
  int flags3 = luaL_optinteger(L,3,0);
  ZMQ_Error rc_zmq_recv1 = 0;
  rc_zmq_recv1 = zmq_recv(this1, msg2, flags3);
  /* check for error. */
  if((-1 == rc_zmq_recv1)) {
    lua_pushnil(L);
      error_code__ZMQ_Error__push(L, rc_zmq_recv1);
  } else {
    lua_pushboolean(L, 1);
    lua_pushnil(L);
  }
  return 2;
}

/* method: recv */
static int ZMQ_Socket__recv__meth(lua_State *L) {
  ZMQ_Socket * this1 = obj_type_ZMQ_Socket_check(L,1);
  int flags2 = luaL_optinteger(L,2,0);
  size_t data_len1 = 0;
  const char * data1 = NULL;
  ZMQ_Error err2 = 0;
	zmq_msg_t msg;
	/* initialize message */
	err2 = zmq_msg_init(&msg);
	if(0 == err2) {
		/* receive message */
		err2 = zmq_recv(this1, &msg, flags2);
		if(0 == err2) {
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

/* method: new */
static int ZMQ_Poller__new__meth(lua_State *L) {
  unsigned int length1 = luaL_optinteger(L,1,10);
  int this_flags1 = OBJ_UDATA_FLAG_OWN;
  ZMQ_Poller * this1;
	ZMQ_Poller poller;
	this1 = &poller;
	this1->items = (zmq_pollitem_t *)calloc(length1, sizeof(zmq_pollitem_t));
	this1->next = -1;
	this1->count = 0;
	this1->len = length1;
	this1->free_list = -1;

  obj_type_ZMQ_Poller_push(L, this1, this_flags1);
  return 1;
}

/* method: close */
static int ZMQ_Poller__close__meth(lua_State *L) {
  int this_flags1 = 0;
  ZMQ_Poller * this1 = obj_type_ZMQ_Poller_delete(L,1,&(this_flags1));
  if(!(this_flags1 & OBJ_UDATA_FLAG_OWN)) { return 0; }
	free(this1->items);
	this1->items = NULL;
	this1->next = -1;
	this1->count = 0;
	this1->len = 0;
	this1->free_list = -1;

  return 0;
}

/* method: add */
static int ZMQ_Poller__add__meth(lua_State *L) {
  ZMQ_Poller * this1 = obj_type_ZMQ_Poller_check(L,1);
  short events3 = luaL_checkinteger(L,3);
  int idx1 = 0;
	zmq_pollitem_t *item;
	ZMQ_Socket *sock = NULL;
	socket_t fd = 0;

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
  ZMQ_Poller * this1 = obj_type_ZMQ_Poller_check(L,1);
  short events3 = luaL_checkinteger(L,3);
  int idx1 = 0;
	zmq_pollitem_t *item;
	ZMQ_Socket *sock = NULL;
	socket_t fd = 0;

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
  ZMQ_Poller * this1 = obj_type_ZMQ_Poller_check(L,1);
	ZMQ_Socket *sock;
	socket_t fd;
	int idx;

	/* ZMQ_Socket or fd */
	if(lua_isuserdata(L, 2)) {
		sock = obj_type_ZMQ_Socket_check(L, 2);
		/* find sock in items list. */
		idx = poller_find_sock_item(this1, sock);
	} else if(lua_isnumber(L, 2)) {
		fd = lua_tonumber(L, 2);
		/* find fd in items list. */
		idx = poller_find_fd_item(this1, fd);
	} else {
		return luaL_typerror(L, 2, "number or ZMQ_Socket");
	}
	/* if sock/fd was found. */
	if(idx >= 0) {
		poller_remove_item(this1, idx);
	}

  return 0;
}

/* method: poll */
static int ZMQ_Poller__poll__meth(lua_State *L) {
  ZMQ_Poller * this1 = obj_type_ZMQ_Poller_check(L,1);
  long timeout2 = luaL_checkinteger(L,2);
  ZMQ_Error err1 = 0;
	/* poll for events */
	err1 = poller_poll(this1, timeout2);
	if(err1 > 0) {
		this1->next = 0;
	} else {
		this1->next = -1;
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

/* method: next_revents */
static int ZMQ_Poller__next_revents__meth(lua_State *L) {
  ZMQ_Poller * this1 = obj_type_ZMQ_Poller_check(L,1);
  short revents2 = 0;
	zmq_pollitem_t *items;
	int count;
	int idx;

	revents2 = -1;
	idx = this1->next;
	if(idx >= 0) {
		count = this1->count;
		items = this1->items;
		/* find next item with pending events. */
		while(idx < count && items[idx].revents == 0) ++idx;
		/* did we find a pending event? */
		if(idx < count) {
			/* push the event's sock/fd. */
			if(items[idx].socket != NULL) {
				obj_type_ZMQ_Socket_push(L, items[idx].socket, 0);
			} else {
				lua_pushnumber(L, items[idx].fd);
			}
			revents2 = items[idx].revents;
			/* is this the last event. */
			++idx;
			this1->next = (idx < count) ? idx : -1;
		}
	}
	if(revents2 < 0) {
		/* no more pending events. */
		lua_pushnil(L);
		this1->next = -1;
	}

  lua_pushinteger(L, revents2);
  return 2;
}

/* method: count */
static int ZMQ_Poller__count__meth(lua_State *L) {
  ZMQ_Poller * this1 = obj_type_ZMQ_Poller_check(L,1);
  int count1 = 0;
	count1 = this1->count;

  lua_pushinteger(L, count1);
  return 1;
}

/* method: term */
static int ZMQ_Ctx__term__meth(lua_State *L) {
  int this_flags1 = 0;
  ZMQ_Ctx * this1 = obj_type_ZMQ_Ctx_delete(L,1,&(this_flags1));
  ZMQ_Error rc_zmq_term1 = 0;
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
  ZMQ_Ctx * this1 = obj_type_ZMQ_Ctx_check(L,1);
  void * ptr1 = NULL;
	ptr1 = this1;

  lua_pushlightuserdata(L, ptr1);
  return 1;
}

/* method: socket */
static int ZMQ_Ctx__socket__meth(lua_State *L) {
  ZMQ_Ctx * this1 = obj_type_ZMQ_Ctx_check(L,1);
  int type2 = luaL_checkinteger(L,2);
  int rc_zmq_socket_flags1 = OBJ_UDATA_FLAG_OWN;
  ZMQ_Socket * rc_zmq_socket1;
  rc_zmq_socket1 = zmq_socket(this1, type2);
  if((NULL == rc_zmq_socket1)) {
    lua_pushnil(L);
    lua_pushstring(L, get_zmq_strerror());
  } else {
    obj_type_ZMQ_Socket_push(L, rc_zmq_socket1, rc_zmq_socket_flags1);
  }
  return 1;
}

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
  ZMQ_StopWatch * this1 = obj_type_ZMQ_StopWatch_delete(L,1,&(this_flags1));
  unsigned long usecs1 = 0;
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
  int io_threads1 = luaL_checkinteger(L,1);
  int rc_zmq_init_flags1 = OBJ_UDATA_FLAG_OWN;
  ZMQ_Ctx * rc_zmq_init1;
  rc_zmq_init1 = zmq_init(io_threads1);
  if((NULL == rc_zmq_init1)) {
    lua_pushnil(L);
    lua_pushstring(L, get_zmq_strerror());
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
  } else {
    obj_type_ZMQ_Ctx_push(L, ctx1, 0);
  }
  return 1;
}

/* method: device */
static int zmq__device__func(lua_State *L) {
  int device1 = luaL_checkinteger(L,1);
  ZMQ_Socket * insock2 = obj_type_ZMQ_Socket_check(L,2);
  ZMQ_Socket * outsock3 = obj_type_ZMQ_Socket_check(L,3);
  ZMQ_Error rc_zmq_device1 = 0;
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
  int seconds_1 = luaL_checkinteger(L,1);
  zmq_sleep(seconds_1);
  return 0;
}

/* method: dump_ffi */
static int zmq__dump_ffi__func(lua_State *L) {
  size_t ffi_code_len1 = 0;
  const char * ffi_code1 = NULL;
	ffi_code1 = zmq_ffi_lua_code;
	ffi_code_len1 = sizeof(zmq_ffi_lua_code) - 1;

  if(ffi_code1 == NULL) lua_pushnil(L);  else lua_pushlstring(L, ffi_code1,ffi_code_len1);
  return 1;
}



static const luaL_reg obj_zmq_msg_t_pub_funcs[] = {
  {"init", zmq_msg_t__init__meth},
  {"init_size", zmq_msg_t__init_size__meth},
  {"init_data", zmq_msg_t__init_data__meth},
  {NULL, NULL}
};

static const luaL_reg obj_zmq_msg_t_methods[] = {
  {"close", zmq_msg_t__close__meth},
  {"move", zmq_msg_t__move__meth},
  {"copy", zmq_msg_t__copy__meth},
  {"set_data", zmq_msg_t__set_data__meth},
  {"data", zmq_msg_t__data__meth},
  {"set_size", zmq_msg_t__set_size__meth},
  {"size", zmq_msg_t__size__meth},
  {NULL, NULL}
};

static const luaL_reg obj_zmq_msg_t_metas[] = {
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

static const luaL_reg obj_ZMQ_Socket_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_reg obj_ZMQ_Socket_methods[] = {
  {"close", ZMQ_Socket__close__meth},
  {"bind", ZMQ_Socket__bind__meth},
  {"connect", ZMQ_Socket__connect__meth},
  {"setopt", ZMQ_Socket__setopt__meth},
  {"getopt", ZMQ_Socket__getopt__meth},
  {"events", ZMQ_Socket__events__meth},
  {"send_msg", ZMQ_Socket__send_msg__meth},
  {"send", ZMQ_Socket__send__meth},
  {"recv_msg", ZMQ_Socket__recv_msg__meth},
  {"recv", ZMQ_Socket__recv__meth},
  {NULL, NULL}
};

static const luaL_reg obj_ZMQ_Socket_metas[] = {
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

static const luaL_reg obj_ZMQ_Poller_pub_funcs[] = {
  {"new", ZMQ_Poller__new__meth},
  {NULL, NULL}
};

static const luaL_reg obj_ZMQ_Poller_methods[] = {
  {"close", ZMQ_Poller__close__meth},
  {"add", ZMQ_Poller__add__meth},
  {"modify", ZMQ_Poller__modify__meth},
  {"remove", ZMQ_Poller__remove__meth},
  {"poll", ZMQ_Poller__poll__meth},
  {"next_revents", ZMQ_Poller__next_revents__meth},
  {"count", ZMQ_Poller__count__meth},
  {NULL, NULL}
};

static const luaL_reg obj_ZMQ_Poller_metas[] = {
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

static const luaL_reg obj_ZMQ_Ctx_pub_funcs[] = {
  {NULL, NULL}
};

static const luaL_reg obj_ZMQ_Ctx_methods[] = {
  {"term", ZMQ_Ctx__term__meth},
  {"lightuserdata", ZMQ_Ctx__lightuserdata__meth},
  {"socket", ZMQ_Ctx__socket__meth},
  {NULL, NULL}
};

static const luaL_reg obj_ZMQ_Ctx_metas[] = {
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

static const luaL_reg obj_ZMQ_StopWatch_pub_funcs[] = {
  {"start", ZMQ_StopWatch__start__meth},
  {NULL, NULL}
};

static const luaL_reg obj_ZMQ_StopWatch_methods[] = {
  {"stop", ZMQ_StopWatch__stop__meth},
  {NULL, NULL}
};

static const luaL_reg obj_ZMQ_StopWatch_metas[] = {
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

static const luaL_reg zmq_function[] = {
  {"version", zmq__version__func},
  {"init", zmq__init__func},
  {"init_ctx", zmq__init_ctx__func},
  {"device", zmq__device__func},
  {"stopwatch_start", zmq__stopwatch_start__func},
  {"sleep", zmq__sleep__func},
  {"dump_ffi", zmq__dump_ffi__func},
  {NULL, NULL}
};

static const obj_const zmq_constants[] = {
  {"TYPE", NULL, 16, CONST_NUMBER},
  {"RCVMORE", NULL, 13, CONST_NUMBER},
  {"LINGER", NULL, 17, CONST_NUMBER},
  {"SWAP", NULL, 3, CONST_NUMBER},
  {"MSG_SHARED", NULL, 128, CONST_NUMBER},
  {"SNDBUF", NULL, 11, CONST_NUMBER},
  {"STREAMER", NULL, 1, CONST_NUMBER},
  {"NOBLOCK", NULL, 1, CONST_NUMBER},
  {"RCVBUF", NULL, 12, CONST_NUMBER},
  {"FORWARDER", NULL, 2, CONST_NUMBER},
  {"RATE", NULL, 8, CONST_NUMBER},
  {"IDENTITY", NULL, 5, CONST_NUMBER},
  {"SUB", NULL, 2, CONST_NUMBER},
  {"FD", NULL, 14, CONST_NUMBER},
  {"PUB", NULL, 1, CONST_NUMBER},
  {"DELIMITER", NULL, 31, CONST_NUMBER},
  {"BACKLOG", NULL, 19, CONST_NUMBER},
  {"SNDMORE", NULL, 2, CONST_NUMBER},
  {"POLLIN", NULL, 1, CONST_NUMBER},
  {"REP", NULL, 4, CONST_NUMBER},
  {"POLLERR", NULL, 4, CONST_NUMBER},
  {"MAX_VSM_SIZE", NULL, 30, CONST_NUMBER},
  {"PUSH", NULL, 8, CONST_NUMBER},
  {"HWM", NULL, 1, CONST_NUMBER},
  {"MSG_MORE", NULL, 1, CONST_NUMBER},
  {"REQ", NULL, 3, CONST_NUMBER},
  {"UNSUBSCRIBE", NULL, 7, CONST_NUMBER},
  {"PULL", NULL, 7, CONST_NUMBER},
  {"PAIR", NULL, 0, CONST_NUMBER},
  {"QUEUE", NULL, 3, CONST_NUMBER},
  {"EVENTS", NULL, 15, CONST_NUMBER},
  {"XREQ", NULL, 5, CONST_NUMBER},
  {"XREP", NULL, 6, CONST_NUMBER},
  {"SUBSCRIBE", NULL, 6, CONST_NUMBER},
  {"MCAST_LOOP", NULL, 10, CONST_NUMBER},
  {"VSM", NULL, 32, CONST_NUMBER},
  {"RECOVERY_IVL", NULL, 9, CONST_NUMBER},
  {"RECONNECT_IVL", NULL, 18, CONST_NUMBER},
  {"POLLOUT", NULL, 2, CONST_NUMBER},
  {"AFFINITY", NULL, 4, CONST_NUMBER},
  {NULL, NULL, 0.0 , 0}
};

static const ffi_export_symbol zmq_ffi_export[] = {
{ "get_zmq_strerror", get_zmq_strerror },
{ "simple_zmq_send", simple_zmq_send },
{ "poller_find_sock_item", poller_find_sock_item },
{ "poller_find_fd_item", poller_find_fd_item },
{ "poller_get_free_item", poller_get_free_item },
{ "poller_poll", poller_poll },
{ "poller_remove_item", poller_remove_item },
  {NULL, NULL}
};



static const reg_sub_module reg_sub_modules[] = {
  { &(obj_type_zmq_msg_t), 0, obj_zmq_msg_t_pub_funcs, obj_zmq_msg_t_methods, obj_zmq_msg_t_metas, obj_zmq_msg_t_bases, obj_zmq_msg_t_fields, obj_zmq_msg_t_constants},
  { &(obj_type_ZMQ_Socket), 0, obj_ZMQ_Socket_pub_funcs, obj_ZMQ_Socket_methods, obj_ZMQ_Socket_metas, obj_ZMQ_Socket_bases, obj_ZMQ_Socket_fields, obj_ZMQ_Socket_constants},
  { &(obj_type_ZMQ_Poller), 0, obj_ZMQ_Poller_pub_funcs, obj_ZMQ_Poller_methods, obj_ZMQ_Poller_metas, obj_ZMQ_Poller_bases, obj_ZMQ_Poller_fields, obj_ZMQ_Poller_constants},
  { &(obj_type_ZMQ_Ctx), 0, obj_ZMQ_Ctx_pub_funcs, obj_ZMQ_Ctx_methods, obj_ZMQ_Ctx_metas, obj_ZMQ_Ctx_bases, obj_ZMQ_Ctx_fields, obj_ZMQ_Ctx_constants},
  { &(obj_type_ZMQ_StopWatch), 0, obj_ZMQ_StopWatch_pub_funcs, obj_ZMQ_StopWatch_methods, obj_ZMQ_StopWatch_metas, obj_ZMQ_StopWatch_bases, obj_ZMQ_StopWatch_fields, obj_ZMQ_StopWatch_constants},
  {NULL, 0, NULL, NULL, NULL, NULL, NULL, NULL}
};







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

#if LUAJIT_FFI
	/* private table to hold reference to object metatables. */
	lua_newtable(L);
	priv_table = lua_gettop(L);
#endif

	/* create object cache. */
	create_object_instance_cache(L);

	/* module table. */
	luaL_register(L, "zmq", zmq_function);

	/* register module constants. */
	obj_type_register_constants(L, zmq_constants, -1);

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
	nobj_try_loading_ffi(L, "zmq", zmq_ffi_lua_code,
		zmq_ffi_export, priv_table);
#endif
	return 1;
}


