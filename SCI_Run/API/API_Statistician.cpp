// Lic:
// Scyndi's Creative Interpreter
// Statistician Support
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
// Version: 23.09.23
// EndLic

#include <Lunatic.hpp>
#include <Statistician.hpp>
#include <SlyvString.hpp>

#include "../SCI_Script.hpp"
#include "../SCI_Crash.hpp"

#define BaseTag \
	auto \
		PartyTag{ Upper(Lunatic_CheckString(L,1)) },\
		CharTag{ Lunatic_CheckString(L,2) };\
	if (!PartyReg.count(PartyTag)) Crash(TrSPrintF("There is no party tagged '%s' in the statician database"));\
	auto CharData{ (*PartyReg[PartyTag])[CharTag] };

#define GetStatTag \
	BaseTag;\
	auto StatTag{ Lunatic_CheckString(L,3) };

using namespace std;
using namespace Slyvina;
using namespace Units;
using namespace Statistician;
using namespace Lunatic;

namespace Scyndi_CI {

	static map<string, Party> PartyReg{};

	static int API_Stat_HasParty(lua_State* L) {
		lua_pushboolean(L, PartyReg.count(Upper(luaL_checkstring(L, 1))));
		return 1;
	}

	static int API_Stat_CreateParty(lua_State* L) {
		static uint32 cnt{0};
		auto PartyTag{ Upper(luaL_optstring(L,1,"")) };
		if (PartyTag == "") {
			do { PartyTag = TrSPrintF("*** PARTY %08X ***", cnt++); } while (PartyReg.count(PartyTag));
		}
		PartyReg[PartyTag] = CreateParty();
		Lunatic_PushString(L, PartyTag);
		return 1;
	}

	static int API_Stat_KillParty(lua_State* L) {
		auto PartyTag{ Upper(luaL_checkstring(L,1)) };
		if (PartyReg.count(PartyTag)) PartyReg.erase(PartyTag);
		return 0;
	}

	static int API_StatGetData(lua_State* L) {
		BaseTag;
		auto DataTag{ luaL_checkstring(L,3) };
		auto Ret{ (*PartyReg[PartyTag])[CharTag]->Data[DataTag] };
		Lunatic_PushString(L, Ret);
		return 1;
	}

	static int API_StatSetData(lua_State* L) {
		BaseTag;
		auto DataTag{ luaL_checkstring(L,3) };
		(*PartyReg[PartyTag])[CharTag]->Data[DataTag] = luaL_checkstring(L, 4);
		return 0;
	}

	static int API_StatGetBaseStat(lua_State* L) {
		GetStatTag;
		lua_pushinteger(L, CharData->BStat[StatTag]);
		return 1;
	}

	static int API_StatSetBaseStat(lua_State* L) {
		GetStatTag;
		CharData->BStat[StatTag] = luaL_checkinteger(L, 4);
		return 0;
	}

	static int API_StatGetStat(lua_State* L) {
		GetStatTag;
		lua_pushinteger(L, CharData->Stats(StatTag));
	}

	static int API_StatGetBuf(lua_State* L) {
		GetStatTag;
		auto St = CharData->Statistic(StatTag);
		lua_pushinteger(L, (*St)[luaL_checkstring(L, 4)]);
		return 1;
	}

	static int API_StatSetBuf(lua_State* L) {
		GetStatTag;
		auto St = CharData->Statistic(StatTag);
		(*St)[luaL_checkstring(L, 4)] = luaL_checkinteger(L, 5);
		return 0;
	}

	static int API_StatCreateChar(lua_State* L) {
		auto
			PartyTag{ Upper(luaL_checkstring(L,1)) },
			CharTag{ Lunatic_CheckString(L,2) },
			Name{ Lunatic_CheckString(L,3) };
		auto
			AutoAdd{ luaL_checkinteger(L,4) };
		if (!PartyReg.count(PartyTag)) Crash(TrSPrintF("There is no party tagged '%s' in the statician database")); 
		PartyReg[PartyTag]->NewChar(AutoAdd, CharTag, Name);
		return 0;
	}

	static int API_StatKilLChar(lua_State* L) {
		auto
			PartyTag{ Upper(luaL_checkstring(L,1)) },
			CharTag{ Lunatic_CheckString(L,2) },
			Name{ Lunatic_CheckString(L,3) };
		if (!PartyReg.count(PartyTag)) Crash(TrSPrintF("There is no party tagged '%s' in the statician database"));
		PartyReg[PartyTag]->Kill(CharTag);
		return 0;
	}

	static int API_StatSetParty(lua_State* L) {
		auto
			PartyTag{ Upper(luaL_checkstring(L,1)) };
		auto
			Index{ luaL_checkinteger(L,2) };
		auto
			ChTag{ luaL_checkstring(L,3) };
		if (!PartyReg.count(PartyTag)) Crash(TrSPrintF("There is no party tagged '%s' in the statician database"));
		(*PartyReg[PartyTag])[Index] = ChTag;
		return 0;
	}

	static int API_StatGetParty(lua_State* L) {
		auto
			PartyTag{ Upper(luaL_checkstring(L,1)) };
		auto
			Index{ luaL_checkinteger(L,2) };
		if (!PartyReg.count(PartyTag)) Crash(TrSPrintF("There is no party tagged '%s' in the statician database"));
		Lunatic_PushString(L, (*PartyReg[PartyTag])[Index]);
		return 1;
	}

	static int API_StatGetPartySize(lua_State* L) {
			auto
				PartyTag{ Upper(luaL_checkstring(L,1)) };
			if (!PartyReg.count(PartyTag)) Crash(TrSPrintF("There is no party tagged '%s' in the statician database"));
			lua_pushinteger(L, PartyReg[PartyTag]->Size());
			return 1;
	}

	static int API_StatPointsSet(lua_State* L) {
		BaseTag;
		auto Pnt{ CharData->GetPoints(luaL_checkstring(L,3)) };
		switch (luaL_checkinteger(L, 4)) {
		case 0: lua_pushinteger(L, Pnt->Have()); return 1;
		case 1: lua_pushinteger(L, Pnt->Max()); return 1;
		case 2: lua_pushinteger(L, Pnt->Min()); return 1;
		case 3: Lunatic_PushString(L, Pnt->MaxCopy); return 1;
		case 4: Lunatic_PushString(L, Pnt->MinCopy); return 1;
		default: Crash(TrSPrintF("Stat Points Set Unknown Tag %d", luaL_checkinteger(L, 4))); return 0;
		}
	}

	static int API_StatPointsGet(lua_State* L) {
		BaseTag;
		auto Pnt{ CharData->GetPoints(luaL_checkstring(L,3)) };
		switch (luaL_checkinteger(L, 4)) {
		case 0: Pnt->Have(luaL_checkinteger(L,5)); return 0;
		case 1: Pnt->Max(luaL_checkinteger(L, 5)); return 0;
		case 2: Pnt->Min(luaL_checkinteger(L, 5)); return 0;
		case 3: Pnt->MaxCopy = luaL_checkstring(L, 5); return 0;
		case 4: Pnt->MinCopy = luaL_checkstring(L, 5); return 0;
		default: Crash(TrSPrintF("Stat Points Get Unknown Tag %d", luaL_checkinteger(L, 4))); return 0;
		}
	}


	void Init_API_Statistician() {
		std::map<std::string, lua_CFunction>IAPI{
			{"HasParty", API_Stat_HasParty},
			{ "CreateParty",API_Stat_CreateParty },
			{ "KillParty",API_Stat_KillParty },
			{ "SetCharData", API_StatSetData },
			{ "GetCharData",API_StatSetData },
			{ "SetBaseStat",API_StatSetBaseStat },
			{ "GetBaseStat",API_StatGetBaseStat },
			{ "GetTotalStat",API_StatGetStat },
			{ "GetStatBuf",API_StatGetBuf },
			{ "SetStatBuf",API_StatSetBuf },
			{ "StatNewChar",API_StatCreateChar },
			{ "StatKillChar",API_StatKilLChar },
			{ "GetPartySize",API_StatGetPartySize },
			{ "GetParty",API_StatGetParty },
			{ "SetParty",API_StatSetParty },
			{ "StatPointsSet",API_StatPointsSet },
			{ "StatPointsGet",API_StatPointsGet }
		};
		InstallAPI("Statistician", IAPI);
	}

}