// LuaDllTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "lua.hpp"
#ifdef _DEBUG
#pragma comment(lib,"LuaD.lib")
#else
#pragma comment(lib,"Lua.lib")
#endif // _DEBUG

#include "LuaBridge/LuaBridge.h"

class A
{
public:
  void func(int a, int b)
  {
    std::cout << a << "+" << b << "=" << a + b << std::endl;
  }
};


void RegisterFunc(lua_State *l)
{
  luabridge::getGlobalNamespace(l).beginClass<A>("A").addFunction("ff", &A::func).endClass();
  A *a = new A;
  luabridge::setGlobal(l, a, "aa");
}

void PushParam(lua_State* l, int& push_count) {}
template<typename First, typename...Rest>
void PushParam(lua_State* l, int& push_count, First&& first, Rest&& ...rest)
{
  push_count++;
  luabridge::push<First>(l, first);
  PushParam(l, push_count, std::forward<Rest>(rest)...);
}

template<typename R,typename ...Args>
std::pair<bool,R> CallLuaFunc(lua_State *l,std::string const&func_name,Args&&...args)
{
  lua_getglobal(l,func_name.c_str());
  if (lua_isfunction(l, -1)) {
    int param_count = 0;
    PushParam(l, param_count, std::forward<Args>(args)...);
    lua_pcall(l, param_count, 1, 0);
    return { true,luabridge::Stack<R>::get(l,-1) };
  }

  return { false,{} };
}

int main()
{
  auto l = luaL_newstate();
  luaL_openlibs(l);
  RegisterFunc(l);
  luaL_dofile(l, "a.lua");

  std::cout << CallLuaFunc<int>(l, "haha", 1, 2, 3, 4).second << std::endl;
  lua_close(l);
}

