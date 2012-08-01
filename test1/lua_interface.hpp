#ifndef LUA_INTERFACE_HPP
#define LUA_INTERFACE_HPP


#include "stdafx.h"
#include <lua.hpp>
//#include "l_request.hpp"
#include "reply.hpp"
namespace http {
	namespace server3 {

		class LuaInterface{
		private:
			lua_State* L;
		public:
			LuaInterface(){
				L = luaL_newstate();
				luaL_openlibs(L);
				//l_register_request(L);
			}

			bool do_bussiness(const request i_request, reply &o_reply, std::string bussiness){
				//i_request->uri = "sdf";
				o_reply.content = "hello";
				return 1;
			}

			~LuaInterface(){
				lua_close(L);
			}
		};
	}
}

#endif