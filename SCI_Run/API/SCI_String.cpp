// Lic:
// Scyndi's Creative Interpreter
// Super String API
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
// Version: 23.01.18
// EndLic
#include "../SCI_Script.hpp"

using namespace Slyvina;
using namespace Units;
using namespace Lunatic;


namespace Scyndi_CI {

	int API_StReplace(lua_State* L) {
		auto
			HayStack{ Lunatic_CheckString(L,1) },
			Needle{ Lunatic_CheckString(L,2) },
			Substitute{ Lunatic_CheckString(L,3) };
		Lunatic_PushString(L, StReplace(HayStack, Needle, Substitute));
		return 1;

	}

	int API_Repeat(lua_State* L) {
		Lunatic_PushString(L,
			Repeat(
				Lunatic_CheckString(L, 1),
				(uint64)luaL_checkinteger(L, 1)
			)
		);
		return 1;
	}

	void Init_SCI_String() {
		std::map<std::string, lua_CFunction> IAPI{
			{"Replace",API_StReplace},
			{"Repeat",API_Repeat}
		};
		InstallAPI("SString", IAPI);
	}
}