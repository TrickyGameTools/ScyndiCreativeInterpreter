
#include <TQSG.hpp>
#include <TQSG_BlopPlasma.hpp>


#include "../SCI_Script.hpp"

using namespace Slyvina;
using namespace Lunatic;
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