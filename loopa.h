#ifndef LOOPA_H
#define LOOPA_H

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#ifndef LOOPA_ACTUATOR_TYPE
#define LOOPA_ACTUATOR_TYPE double
#endif // !LOOPA_ACTUATOR_TYPE

#ifndef LOOPA_SENSOR_TYPE
#define LOOPA_SENSOR_TYPE double
#endif // !LOOPA_SENSOR_TYPE

lua_State* loopa_init(char* filename);
void loopa_send_sensor(lua_State* L, LOOPA_SENSOR_TYPE sensor);
LOOPA_ACTUATOR_TYPE loopa_get_actuator(lua_State* L);
void loopa_close(lua_State* L);

#endif // LOOPA_H

// -------------------------------------------------------

#ifdef LOOPA_IMPLEMENTATION

lua_State* loopa_init(char* filename) {
  lua_State* L = luaL_newstate();
  luaL_dofile(L, filename);
  luaL_openlibs(L);
  lua_getglobal(L, "Controller");
  lua_getfield(L, -1, "init");
  lua_pcall(L, 0, 1, 0);
  lua_setglobal(L, "mycontroller");
  return L;
}

void loopa_send_sensor(lua_State* L, LOOPA_SENSOR_TYPE sensor) {
  lua_getglobal(L, "Controller");
  lua_getfield(L, -1, "recv_sensor");
  lua_getglobal(L, "mycontroller");
  lua_pushnumber(L, sensor);
  lua_pcall(L, 2, 0, 0);
  lua_pop(L, 2);
}

LOOPA_ACTUATOR_TYPE loopa_get_actuator(lua_State* L) {
  lua_getglobal(L, "Controller");
  lua_getfield(L, -1, "send_actuator");
  lua_getglobal(L, "mycontroller");
  lua_pcall(L, 1, 1, 0);
  LOOPA_ACTUATOR_TYPE result = (LOOPA_ACTUATOR_TYPE)lua_tonumber(L, -1);
  lua_pop(L, 1);
  return result;
}

void loopa_close(lua_State* L) {
  lua_close(L);
}


#endif // LOOPA_IMPLEMENTATION
