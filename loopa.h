#ifndef LOOPA_H
#define LOOPA_H

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <sys/stat.h>
#include <stdlib.h>

#ifndef LOOPA_ACTUATOR_TYPE
#define LOOPA_ACTUATOR_TYPE double
#endif // !LOOPA_ACTUATOR_TYPE

#ifndef LOOPA_SENSOR_TYPE
#define LOOPA_SENSOR_TYPE double
#endif // !LOOPA_SENSOR_TYPE

#ifndef LOOPA_CLASS_NAME
#define LOOPA_CLASS_NAME "Controller"
#endif // !LOOPA_CLASS_NAME

#ifndef LOOPA_CONTROLLER_VAR_NAME
#define LOOPA_CONTROLLER_VAR_NAME "mycontroller"
#endif // !LOOPA_CONTROLLER_VAR_NAME

typedef struct {
  lua_State* lua_state;
  time_t mtime;
} loopa_State;

loopa_State* loopa_init(char* filename);
void loopa_try_reload(loopa_State* L, char* filename);
void loopa_send_sensor(loopa_State* L, LOOPA_SENSOR_TYPE sensor);
LOOPA_ACTUATOR_TYPE loopa_recv_actuator(loopa_State* L);
void loopa_close(loopa_State* L);

#endif // LOOPA_H

// -------------------------------------------------------

#ifdef LOOPA_IMPLEMENTATION

static lua_State* __loopa_init_lua(char* filename) {
  lua_State* L = luaL_newstate();
  luaL_dofile(L, filename);
  luaL_openlibs(L);
  lua_getglobal(L, LOOPA_CLASS_NAME);
  lua_getfield(L, -1, "init");
  lua_pcall(L, 0, 1, 0);
  lua_setglobal(L, LOOPA_CONTROLLER_VAR_NAME);
  return L;
}

loopa_State* loopa_init(char* filename) {
  loopa_State* L = malloc(sizeof(loopa_State));
  L->lua_state = __loopa_init_lua(filename);
  struct stat stat_struct;
  if (stat(filename, &stat_struct) < 0) {
    perror(filename);
  }
  L->mtime = stat_struct.st_mtime;
  return L;
}

void loopa_try_reload(loopa_State* L, char* filename) {
  struct stat stat_struct;
  if (stat(filename, &stat_struct) < 0) {
    perror(filename);
  }
  time_t mtime = stat_struct.st_mtime;
  if (mtime > L->mtime) {
    lua_close(L->lua_state);
    L->lua_state = __loopa_init_lua(filename);
    L->mtime = mtime;
  }
}


void loopa_send_sensor(loopa_State* L, LOOPA_SENSOR_TYPE sensor) {
  lua_getglobal(L->lua_state, LOOPA_CLASS_NAME);
  lua_getfield(L->lua_state, -1, "recv_sensor");
  lua_getglobal(L->lua_state, LOOPA_CONTROLLER_VAR_NAME);
  lua_pushnumber(L->lua_state, sensor);
  lua_pcall(L->lua_state, 2, 0, 0);
  lua_pop(L->lua_state, 2);
}

LOOPA_ACTUATOR_TYPE loopa_recv_actuator(loopa_State* L) {
  lua_getglobal(L->lua_state, LOOPA_CLASS_NAME);
  lua_getfield(L->lua_state, -1, "send_actuator");
  lua_getglobal(L->lua_state, LOOPA_CONTROLLER_VAR_NAME);
  lua_pcall(L->lua_state, 1, 1, 0);
  LOOPA_ACTUATOR_TYPE result = (LOOPA_ACTUATOR_TYPE)lua_tonumber(L->lua_state, -1);
  lua_pop(L->lua_state, 1);
  return result;
}

void loopa_close(loopa_State* L) {
  lua_close(L->lua_state);
  free(L);
}

#endif // LOOPA_IMPLEMENTATION
