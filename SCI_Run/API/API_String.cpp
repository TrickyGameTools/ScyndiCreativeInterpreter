// Lic:
// Scyndi's Creative Interpreter
// Super String
// 
// 
// 
// (c) Jeroen P. Broks, 2023
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// Please note that some references to data like pictures or audio, do not automatically
// fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 23.12.24
// EndLic

#include <SlyvMD5.hpp>
#include "../SCI_Script.hpp"

using namespace Slyvina;
using namespace Units;
using namespace Lunatic;


namespace Scyndi_CI {

	static VecString LastSplit{nullptr};

	static int API_StReplace(lua_State* L) {
		auto
			HayStack{ Lunatic_CheckString(L,1) },
			Needle{ Lunatic_CheckString(L,2) },
			Substitute{ Lunatic_CheckString(L,3) };
		Lunatic_PushString(L, StReplace(HayStack, Needle, Substitute));
		return 1;

	}

	static int API_Repeat(lua_State* L) {
		Lunatic_PushString(L,
			Repeat(
				Lunatic_CheckString(L, 1),
				(uint64)luaL_checkinteger(L, 1)
			)
		);
		return 1;
	}

	static int API_MD5(lua_State* L) {
		Lunatic_PushString(L,
			md5(Lunatic_CheckString(L, 1))
		);
		return 1;
	}

	static int API_StripAll(lua_State* L) {
		Lunatic_PushString(L,
			StripAll(luaL_checkstring(L, 1)));
		return 1;
	}

	static int API_StripExt(lua_State* L) {
		Lunatic_PushString(L,
			StripExt(luaL_checkstring(L, 1)));
		return 1;
	}

	static int API_StripDir(lua_State* L) {
		Lunatic_PushString(L,
			StripDir(luaL_checkstring(L, 1)));
		return 1;
	}

	static int API_ExtExt(lua_State* L) {
		Lunatic_PushString(L,
			ExtractExt(luaL_checkstring(L, 1)));
		return 1;
	}

	static int API_ExtDir(lua_State* L) {
		Lunatic_PushString(L,
			ExtractDir(luaL_checkstring(L, 1)));
		return 1;
	}

	static int API_Split(lua_State* L) {
		LastSplit = Split(
			Lunatic_CheckString(L, 1),
			Lunatic_OptString(L, 2, " ")[0]
		);
		lua_pushinteger(L, LastSplit->size());
		return 1;
	}

	static int API_SplitVal(lua_State* L) {
		auto idx{ luaL_checknumber(L,1) };
		if (!LastSplit) return 0;
		if (idx >= LastSplit->size()) return 0;
		Lunatic_PushString(L, (*LastSplit)[idx]);
		return 1;
	}

	void Init_API_String() {
		std::map<std::string, lua_CFunction> IAPI{
			{ "Replace", API_StReplace},
			{ "Repeat",API_Repeat },
			{ "MD5",API_MD5 },
			{ "StripAll",API_StripAll },
			{ "StripExt",API_StripExt },
			{ "StripDir",API_StripDir },
			{ "ExtractExt",API_ExtExt },
			{ "ExtractDir",API_ExtDir },
			{ "Split",API_Split },
			{ "SplitVal",API_SplitVal }
		};
		InstallAPI("SString", IAPI);
	}
}