// License:
// 
// Scyndi's Creative Interpreter
// Blop Plasma API
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
// Version: 24.10.10
// End License
// Lic:
// Scyndi's Creative Interpreter
// Blop Plasma API
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

#include <TQSG.hpp>
#include <TQSG_BlopPlasma.hpp>


#include "../SCI_Script.hpp"

using namespace Slyvina;
using namespace NSLunatic;
using namespace TQSG;


namespace Scyndi_CI {
	static int API_BPSet(lua_State* L) {
		NewBlopPlasma(
			luaL_optinteger(L, 1, 60),
			luaL_optinteger(L, 2, ScreenWidth()),
			luaL_optinteger(L, 3, ScreenHeight())
		);
		return 0;
	}

	static int AP_BPDraw(lua_State* L) {
		auto l{ lua_gettop(L) };
		if (l <= 1)
			DrawBlopPlasma(luaL_optinteger(L, 1, 60), false);
		else
			DrawBlopPlasma(
				luaL_checknumber(L, 1),
				luaL_checknumber(L, 2),
				luaL_checknumber(L, 3),
				luaL_optinteger(L, 4, 60)
			);
		return 0;
	}

	void Init_API_BlopPlasma() {
		InitBlopPlasma(SRF(), "GFX/Blop.png");
		std::map<std::string, lua_CFunction>IAPI{
			{ "Settings",API_BPSet },
			{ "Draw",AP_BPDraw }
		};
		InstallAPI("BlopPlasma", IAPI);
	}
}
