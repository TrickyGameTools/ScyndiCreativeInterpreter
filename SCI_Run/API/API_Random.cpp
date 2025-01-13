// License:
// 
// Scyndi's Creative Interpreter
// Global Random (not finished)
// 
// 
// 
// 	(c) Jeroen P. Broks, 2024, 2025
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
// Version: 25.01.13
// End License
// Lic:
// Scyndi's Creative Interpreter
// Global Random (not finished)
// 
// 
// 
// (c) Jeroen P. Broks, 2024
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
// Version: 24.09.24
// EndLic
#include <Slyvina.hpp>
#include <SlyvRandom.hpp>

#include "../SCI_Script.hpp"

using namespace Slyvina;
using namespace Units;

namespace Scyndi_CI {

	static int R_LastSetSeed(lua_State* L) {
		lua_pushinteger(L, Rand.GetLastSetSeed());
		return 1;
	}

	static int R_Seed(lua_State* L) {
		Rand.Seed((uint32)luaL_optinteger(L, 1, 0));
		return 0;
	}

	static int R_Rand(lua_State* L) {
		auto
			A1{ luaL_checkinteger(L,1) },
			A2{ luaL_optinteger(L,2,0) },
			R1{ std::min(A1,A2) },
			R2{ std::max(A1,A2) };
		lua_pushinteger(L, Rand.Get(R1, R2));
		return 1;
	}

	void Init_API_Random(){
	
		std::map<std::string, lua_CFunction>IAPI{
			{ "LastSeed", R_LastSetSeed },
			{ "Seed",R_Seed },
			{ "Rand",R_Rand }
		};
		InstallAPI("Rand", IAPI);
	}
}
