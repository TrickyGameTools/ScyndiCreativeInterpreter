// License:
// 
// Scyndi's Creative interpreter
// InterState Manager
// 
// 
// 
// 	(c) Jeroen P. Broks, 2023
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
// Version: 24.10.10 I
// End License

#include "../SCI_Script.hpp"

using namespace Slyvina;
using namespace Units;
using namespace NSLunatic;

namespace Scyndi_CI {

	static int IS_Call(lua_State* L) {
		auto
			_State{ Lunatic_CheckString(L,1) },
			_Func{ Lunatic_CheckString(L,2) },
			_Args{ Lunatic_OptString(L,3,"nil") };
		//std::cout << "Interstate call to State:" << _State << "; " << _Func << "(" << _Args << ")\n"; // debug
		Call(_State, _Func, _Args);
		return 0;
	}

	static int IS_LoadState(lua_State* L) {
		auto
			_State{ Lunatic_CheckString(L,1) },
			_File{ Lunatic_CheckString(L, 2) };
		State(_State, _File);
		return 0;
	}

	static int IS_LoadNewState(lua_State* L) {
		auto
			_State{ Lunatic_CheckString(L,1) },
			_File{ Lunatic_CheckString(L, 2) };
		if (!HasState(_State))
			State(_State, _File);
		return 0;

	}

	void Init_API_InterState() {
		std::map<std::string, lua_CFunction>IAPI{
			{"Call",IS_Call},
			{"Load",IS_LoadState},
			{"LoadNew",IS_LoadNewState}
		
		};
		InstallAPI("InterState", IAPI);
	}
}
