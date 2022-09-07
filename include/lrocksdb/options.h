#ifndef LROCKSDB_OPTIONS_H
#define LROCKSDB_OPTIONS_H
#include <string.h>
#include <luajit/lauxlib.h>
#include "lrocksdb/types.h"
#include "lrocksdb/helpers.h"

lrocksdb_options_t *lrocksdb_get_options(lua_State *L, int index);
int lrocksdb_options_reg(lua_State *L);
int lrocksdb_options_create(lua_State *L);

/* read options */
lrocksdb_readoptions_t *lrocksdb_get_readoptions(lua_State *L, int index);
int lrocksdb_readoptions_reg(lua_State *L);
int lrocksdb_readoptions_create(lua_State *L);

/* write options */
lrocksdb_writeoptions_t *lrocksdb_get_writeoptions(lua_State *L, int index);
int lrocksdb_writeoptions_reg(lua_State *L);
int lrocksdb_writeoptions_create(lua_State *L);

/* flush options */

/* restore options */
lrocksdb_restoreoptions_t *lrocksdb_get_restoreoptions(lua_State *L, int index);
int lrocksdb_restoreoptions_reg(lua_State *L);
int lrocksdb_restoreoptions_create(lua_State *L);
#endif

