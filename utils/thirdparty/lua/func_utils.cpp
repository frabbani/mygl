#include "func_utils.h"
#include "lua_table.h"
#include "lua_function.h"

namespace luacpp {

ValueConverter::operator LuaObject() const {
    return LuaObject(m_l, m_index);
}

ValueConverter::operator LuaTable() const {
    return LuaTable(m_l, m_index);
}

ValueConverter::operator LuaFunction() const {
    return LuaFunction(m_l, m_index);
}

void PushValue(lua_State* l, const LuaRefObject& obj) {
    lua_rawgeti(l, LUA_REGISTRYINDEX, obj.GetRefIndex());
}

void PushValue(lua_State* l, const LuaObject& obj) {
    lua_rawgeti(l, LUA_REGISTRYINDEX, obj.GetRefIndex());
}

void PushValue(lua_State* l, const LuaTable& tbl) {
    lua_rawgeti(l, LUA_REGISTRYINDEX, tbl.GetRefIndex());
}

void PushValue(lua_State* l, const LuaFunction& func) {
    lua_rawgeti(l, LUA_REGISTRYINDEX, func.GetRefIndex());
}

} // namespace luacpp
