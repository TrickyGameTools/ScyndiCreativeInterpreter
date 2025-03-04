// License:
// 
// Scyndi's Creative Interpreter
// Events API for scripts
// 
// 
// 
// 	(c) Jeroen P. Broks, 2023, 2024, 2025
// 
// 		This program is free software: you can redistribute it and/or modify
// 		it under the terms of the GNU General Public License as published by
// 		the Free Software Foundation, either version 3 of the License, or
// 		(at your option) any later version.
// 
// 		This program is distributed in the hope that it will be useful,
// 		but WITHOUT ANY WARRANTY; without even the implied warranty of
// 		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// 		GNU General Public License for more details.
// 		You should have received a copy of the GNU General Public License
// 		along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// 	Please note that some references to data like pictures or audio, do not automatically
// 	fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 25.03.04
// End License

#include <TQSE.hpp>
#include <TQSG.hpp>
#include <SlyvQCol.hpp>

#include "../SCI_Script.hpp"

using namespace Slyvina;
using namespace Units;
using namespace TQSE;
using namespace TQSG;


namespace Scyndi_CI {

	static int API_Events_Poll(lua_State* L) { Poll(); return 0; }
	static int API_Events_AppTerminate(lua_State* L) { lua_pushboolean(L, AppTerminate()); return 1; }
	static int API_Events_KeyHit(lua_State* L) { lua_pushboolean(L, KeyHit((SDL_KeyCode)luaL_checkinteger(L, 1))); return 1; }
	static int API_Events_KeyDown(lua_State* L) { lua_pushboolean(L, KeyDown((SDL_KeyCode)luaL_checkinteger(L, 1))); return 1; }
	static int API_Events_GetKey(lua_State* L) { lua_pushinteger(L, GetKey()); return 1; }

	static int API_Events_MouseX(lua_State* L) {
		auto TrueX{ luaL_checkinteger(L,1) };
		if (TrueX || ScreenWidth(false) == ScreenWidth(true))
			lua_pushinteger(L, MouseX());
		else
			lua_pushinteger(L, floor(((double)MouseX() / ScreenWidth(true)) * ScreenWidth(false)));
		return 1;
	}

	static int API_Events_MouseY(lua_State* L) {
		auto TrueY{ luaL_checkinteger(L,1) };
		if (TrueY || ScreenHeight(false) == ScreenHeight(true))
			lua_pushinteger(L, MouseY());
		else
			lua_pushinteger(L, floor(((double)MouseY() / ScreenHeight(true)) * ScreenHeight(false)));
		return 1;

	}

	static int API_Events_MouseDown(lua_State* L) {
		lua_pushboolean(L, MouseDown(luaL_optinteger(L, 1, 1)));
		return 1;
	}

	static int API_Events_MouseReleased(lua_State* L) {
		lua_pushboolean(L, MouseReleased(luaL_optinteger(L, 1, 1)));
		return 1;
	}

	static int API_Events_MouseHit(lua_State* L) {
		lua_pushboolean(L, MouseHit(luaL_optinteger(L, 1, 1)));
		return 1;
	}

	static int API_Events_Flush(lua_State* L) { Flush(); return 0; }

	static int API_Events_Yes(lua_State* L) {
		lua_pushboolean(L, Yes(luaL_checkstring(L, 1)));
		return 1;
	}

	static int API_Events_KeyFromName(lua_State* L) {
		auto name{ luaL_checkstring(L,1) };
		SDL_Keycode ret{ SDL_GetKeyFromName(name) };
		if (ret == SDLK_UNKNOWN) { QCol->Warn(TrSPrintF("Key name '%s' was not recognized!\7",name)); }
		lua_pushinteger(L, (int)ret);
		return 1;
	}

	static int API_Events_KeyName(lua_State* L) {
		auto code{ luaL_checkinteger(L,1) };
		lua_pushstring(L,SDL_GetKeyName(code));
		return 1;
	}

	static int API_Events_MouseButtonFromName(lua_State* L) {
		auto n{ Trim(Upper(luaL_checkstring(L,1))) };
		auto c{ 0 };
		if (n == "LEFT" || n == "LINKS" || n == "GAUCHE") c = SDL_BUTTON_LEFT;
		else if (n == "RIGHT" || n == "RECHTS" || n == "DROIT") c = SDL_BUTTON_RIGHT;
		else if (n == "CENTER" || n=="MIDDLE" || n == "MIDDEN" || n == "CENTRE") c = SDL_BUTTON_MIDDLE;
		lua_pushinteger(L, c);
		return 1;
	}

	static int API_Sleep(lua_State* L) {
		SDL_Delay(luaL_checkinteger(L, 1));
		return 0;
	}

	static int API_Events_GetChar(lua_State *L) {
		//char ret[2];
		//ret[0] = (char)GetChar();
		//ret[1] = 0;
		//lua_pushstring(L,ret);
		lua_pushinteger(L,(int)GetChar());
		return 1;
	}

	void Init_API_Events() {
		std::map<std::string, lua_CFunction>IAPI{
			{"Poll", API_Events_Poll},
			{ "AppTerminate",API_Events_AppTerminate },
			{ "KeyHit",API_Events_KeyHit },
			{ "KeyDown",API_Events_KeyDown },
			{ "GetKey",API_Events_GetKey },
			{ "GetChar", API_Events_GetChar },
			{ "MouseX",API_Events_MouseX },
			{ "MouseY",API_Events_MouseY },
			{ "MouseDown",API_Events_MouseDown },
			{ "MouseHit",API_Events_MouseHit },
			{ "MouseReleased",API_Events_MouseReleased },
			{ "KeyFromName",API_Events_KeyFromName },
			{ "KeyName",API_Events_KeyName },
			{ "Flush",API_Events_Flush },
			{ "Yes",API_Events_Yes },
			{ "MouseButtonFromName",API_Events_MouseButtonFromName },
			{ "Sleep", API_Sleep}
		};
		//for (auto&debug:IAPI) std::cout << "\x07DEBUG EVENTS: "<<debug.first<<"\n";
		InstallAPI("Events", IAPI);
	}
}
