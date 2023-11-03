
#include <SlyvQCol.hpp>

#include <Medals.hpp>

#include "../SCI_Script.hpp"
#include "../SCI_Crash.hpp"
#include "../SCI_JCR.hpp"


using namespace Slyvina;
using namespace Lunatic;
using namespace Medals;

namespace Scyndi_CI {

	static bool Loaded;
	static std::string MGame{};

	static void RequireActive() {
		if (!Loaded) Crash("This action requires MEDALS.INI to be loaded. There appears to be none.");
		if (!Active()) Crash("This action requires an SCI version in which Medals was compiled. This version clearly has not.");
	}
		
	static bool GetMedalDataForSCI() {
		auto Entries{ Resource()->Entries() };
		for (auto E : *Entries) {
			if (StripDir(Upper(E->Name())) == "MEDALS.INI") {
				QCol->Doing("Loading", E->Name());
				LoadFromJCR6(Resource(), E->Name());
				return true;
			}
		}
		return false;
	}
	

	static int API_Active(lua_State* L) {
		lua_pushboolean(L, Active());
		return 1;
	}

	static int API_MedalValue(lua_State* L) {
		RequireActive();
		lua_pushinteger(L, MedalValue(MGame.c_str(), luaL_checkstring(L, 1)));
		return 1;
	}

	static int API_FullScore(lua_State* L) {
		RequireActive();
		lua_pushinteger(L, FullScore());
		return 1;
	}

	static int API_Award(lua_State* L) {
		RequireActive();
		Award(MGame.c_str(), luaL_checkstring(L, 1));
		return 0;
	}

	static int API_SetGame(lua_State* L) {
		MGame = luaL_checkstring(L, 1);
		return 0;
	}
	static int API_GetGame(lua_State* L) {
		Lunatic_PushString(L, MGame);
		return 1;
	}



	void Init_API_Medals() {
		if (!Loaded) {
			Loaded = GetMedalDataForSCI();
		}
		std::map<std::string, lua_CFunction>IAPI{
			{ "Active", API_Active },
			{ "MedalValue",API_MedalValue },
			{ "FullScore",API_FullScore },
			{ "Award", API_Award },
			{ "GetGame",API_GetGame },
			{ "SetGame",API_SetGame }
		};
		InstallAPI("Medals", IAPI);
	}
}