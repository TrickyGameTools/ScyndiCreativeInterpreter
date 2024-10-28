// License:
// 
// Scyndi's Creative Interpreter
// GINIE API
// 
// 
// 
// 	(c) Jeroen P. Broks, 2023, 2024
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
// GINIE API
// 
// 
// 
// (c) Jeroen P. Broks, 2023, 2024
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
// Version: 24.03.22
// EndLic

#include <SlyvGINIE.hpp>

#include "../SCI_Script.hpp"
#include "../SCI_Config.hpp"
#include "../SCI_Crash.hpp"

#define AutoTag auto Tag{ Upper(luaL_checkstring(L,1))}; if (!GINREG.count(Tag)) {luaL_error(L,"There is no GINIE tagged '%s",Tag.c_str()); return 0; } auto REC{GINREG[Tag]}

#define API_GINIE_DEBUG

using namespace Slyvina;
using namespace Units;
using namespace NSLunatic;

namespace Scyndi_CI {

	static std::map<std::string, GINIE> GINREG{};

	static int API_GINIE_Create(lua_State* L) {
		auto Tag{ Upper(luaL_checkstring(L,1)) };
		if (Tag == "") {
			static uint64 count{ 0 };
			Tag = TrSPrintF("***AUTO_%08x", count++);
		}
		GINREG[Tag] = ParseGINIE("");
		Lunatic_PushString(L, Tag);
		return 1;
	}

	static int API_GINIE_SetValue(lua_State* L) {
		AutoTag;
		auto
			Cat{ luaL_checkstring(L,2) },
			Key{ luaL_checkstring(L,3) };
		auto
			NewValue{ luaL_optinteger(L,5,0) > 0 };
		if (NewValue && REC->HasValue(Cat, Key)) return 0;
		switch (lua_type(L, 4)) {
			case LUA_TSTRING:
				REC->Value(Cat, Key, luaL_checkstring(L, 4));
				break;
			case LUA_TNUMBER:
				REC->Value(Cat, Key, luaL_checkinteger(L, 4));
				break;
			case LUA_TFUNCTION:
				luaL_error(L, "Functions cannot be set as a GINIE value");
				return 0;
			case LUA_TNONE:
				luaL_error(L, "Nothing given to be a value for GINIE value");
				return 0;
			default:
				luaL_error(L, "Type (Lua code %d) received for GINIE value, but I don't know how to handle that ", lua_type(L, 4));
				return 0;
		}
		return 0;
	}

	static int API_GINIE_NewValue(lua_State* L) {
		AutoTag;
		auto
			Cat{ luaL_checkstring(L,2) },
			Key{ luaL_checkstring(L,3) };
		//auto
		//	NewValue{ luaL_optinteger(L,5,0) > 0 };
		//if (NewValue && REC->HasValue(Cat, Key)) return 0;
		//cout << "New Value " << Cat << ":" << Key << " (Has value:" << REC->HasValue(Cat, Key) << ") "; // Debug Only!
		switch (lua_type(L, 4)) {
		case LUA_TSTRING:
			REC->NewValue(Cat, Key, luaL_checkstring(L, 4));
			break;
		case LUA_TNUMBER:
			REC->NewValue(Cat, Key, luaL_checkinteger(L, 4));
			break;
		case LUA_TFUNCTION:
			luaL_error(L, "Functions cannot be set as a GINIE value");
			return 0;
		case LUA_TNONE:
			luaL_error(L, "Nothing given to be a value for GINIE value");
			return 0;
		default:
			cout << "<GINIE." << Tag << ">.NewValue(\"" << Cat << "," << Key << "\n,<Lua#" << lua_type(L, 4) << ") << \x1b[31mERROR!!!\x1b[0m\n";
			luaL_error(L, "Type (Lua code %d) received for GINIE value, but I don't know how to handle that ", lua_type(L, 4));
			return 0;
		}
		//cout << REC->Value(Cat, Key) << "\n"; // Debug Only
		return 0;
	}

	static int API_GINIE_GetStringValue(lua_State* L) {
		AutoTag;
		auto
			Cat{ luaL_checkstring(L,2) },
			Key{ luaL_checkstring(L,3) };
		Lunatic_PushString(L, REC->Value(Cat, Key));
		return 1;
	}

	static int API_GINIE_GetIntegerValue(lua_State* L) {
		AutoTag;
		auto
			Cat{ luaL_checkstring(L,2) },
			Key{ luaL_checkstring(L,3) };
		lua_pushinteger(L, REC->IntValue(Cat, Key));
		return 1;
	}

	static int API_GINIE_GetBooleanValue(lua_State* L) {
		AutoTag;
		auto
			Cat{ luaL_checkstring(L,2) },
			Key{ luaL_checkstring(L,3) };
		lua_pushboolean(L, Upper(REC->Value(Cat, Key)) == "TRUE");
		return 1;
	}

	static int API_HasGINIE(lua_State* L) {
		lua_pushboolean(L, GINREG.count(Upper(luaL_checkstring(L, 1))));
		return 1;
	}

	static int API_Parse(lua_State* L) {
		AutoTag;
		auto Source = Lunatic_CheckString(L, 2);
		auto Merge = luaL_optinteger(L, 3, 0);
		REC->Parse(Source, Merge);
		return 0;
	}

	static int API_UnParse(lua_State* L) {
		AutoTag;
		auto Header = Lunatic_OptString(L, 2, "Unparsed by Scyndi's Creative Interpreter");
		Lunatic_PushString(L, REC->UnParse(Header));
		return 1;
	}

	static int API_GINIEKill(lua_State* L) {
		auto Tag{ Upper(luaL_checkstring(L,1)) };
		if (GINREG.count(Tag)) GINREG.erase(Tag);
		return 0;
	}

	static int API_GINIEClear(lua_State* L) {
		AutoTag;
		REC->Clear();
		return 0;
	}

	static int API_GINIELoad(lua_State* L) {
		auto
			File{ Lunatic_CheckString(L,1) },
			Tag{ Upper(Lunatic_CheckString(L,2)) },
			src{ Resource()->GetString(File) };
		auto
			REC = ParseGINIE(src);
		GINREG[Tag] = REC;
		return 0;
	}

	static int API_GINIELoadHome(lua_State* L) {
		auto
			PFile{ Lunatic_CheckString(L,1) },
			FFile{ SaveGameDir() + "/" + PFile },
			Tag{ Upper(Lunatic_CheckString(L,2)) },
			AutoSaveFile{ SaveGameDir() + "/" + Lunatic_OptString(L,3,"") },
			AutoSaveHead{ Lunatic_OptString(L,4,"") };			
		if (!FileExists(FFile)) { // { Crash("File not found: " + PFile, "Full File Name: " + FFile); return 0; }
#ifdef API_GINIE_DEBUG
			cout << "GINIE HOME FILE '"<<FFile<<"' does not exist! Creating \n";
#endif
			GINREG[Tag] = ParseGINIE("# Nothing\n#Move along!");
			if (AutoSaveFile.size()) GINREG[Tag]->AutoSave = AutoSaveFile;
			if (AutoSaveHead.size()) GINREG[Tag]->AutoSaveHeader = AutoSaveHead;
		} else {
			auto
				REC = LoadGINIE(FFile, AutoSaveFile, AutoSaveHead);
			GINREG[Tag] = REC;
		}
		return 0;
	}

	static int API_GINIEAdd(lua_State* L) {
		AutoTag;
		auto
			Cat{ luaL_checkstring(L,2) },
			Key{ luaL_checkstring(L,3) },
			Val{ luaL_checkstring(L,4) };
		auto
			Sort{ luaL_checkinteger(L,5) },
			Unique{ luaL_checkinteger(L,6) };

		REC->Add(Cat, Key, Val, Sort, Unique);
		return 0;
	}

	static int API_GINIEListSize(lua_State* L) {
		AutoTag;
		auto
			Cat{ luaL_checkstring(L,2) },
			Key{ luaL_checkstring(L,3) };
		// cout << "API:GINIELISTSIZE(" << Tag << ":" << Cat << "," << Key << ") -> " << REC->ListCount(Cat, Key) << "\n";
		lua_pushinteger(L, REC->ListCount(Cat, Key));
		return 1;
	}

	static int API_GINIEListEntry(lua_State* L) {
		AutoTag;
		auto
			Cat{ luaL_checkstring(L,2) },
			Key{ luaL_checkstring(L,3) };
		auto
			IDX{ luaL_checkinteger(L,4) };
		if (IDX < 0 || IDX >= REC->ListCount(Cat, Key)) {
			Crash(TrSPrintF("%s->ListEntry(\"%s\",\"%s\",%d): Index out of bounds", Tag.c_str(), Cat, Key, IDX));
			return 0;
		}
		Lunatic_PushString(L, (*REC->List(Cat, Key))[IDX]);
		return 1;
	}


	static VecString LastCats{nullptr};
	static int API_GINIE_GrabCats(lua_State* L) {
		AutoTag;
		LastCats = REC->Categories();
		lua_pushinteger(L, LastCats->size());
		return 1;
	}

	static int API_GINIE_Cat(lua_State* L) {
		AutoTag;
		auto idx{ luaL_checkinteger(L,2) };
		if (!LastCats) { luaL_error(L, "No cats have been set up first."); return 0; }
		if (idx < 0 || idx >= LastCats->size()) { luaL_error(L, "Cat out of bounds (%d/%d)", idx, LastCats->size()); return 0; }
		Lunatic_PushString(L, (*LastCats)[idx]);
		return 1;
	}


	static VecString LastVals{nullptr};
	static int API_GINIE_GrabVals(lua_State* L) {
		AutoTag;
		auto Cat{ luaL_checkstring(L,2) };
		LastVals = REC->Values(Cat);
		lua_pushinteger(L, LastVals->size());
		return 1;
	}

	static int API_GINIE_Val(lua_State* L) {
		AutoTag;
		auto idx{ luaL_checkinteger(L,2) };
		if (!LastVals) { luaL_error(L, "No vales have been set up first."); return 0; }
		if (idx < 0 || idx >= LastVals->size()) { luaL_error(L, "Cat out of bounds (%d/%d)", idx, LastVals->size()); return 0; }
		Lunatic_PushString(L, (*LastVals)[idx]);
		return 1;

	}
	



	void Init_API_GINIE() {
		std::map<std::string, lua_CFunction>IAPI{
			{ "Create", API_GINIE_Create },
			{ "Has",API_HasGINIE },
			{ "GetStringValue",API_GINIE_GetStringValue },
			{ "GetIntegerValue",API_GINIE_GetIntegerValue },
			{ "GetBooleanValue",API_GINIE_GetBooleanValue },
			{ "SetValue",API_GINIE_SetValue },
			{ "NewValue",API_GINIE_NewValue },
			{ "UnParse",API_UnParse },
			{ "Parse",API_Parse },
			{ "Kill",API_GINIEKill },
			{ "Load",API_GINIELoad },
			{ "LoadHome",API_GINIELoadHome },
			{ "Add",API_GINIEAdd },
			{ "ListSize",API_GINIEListSize },
			{ "ListEntry",API_GINIEListEntry },
			{ "CatsGrab",API_GINIE_GrabCats },
			{ "Cats",API_GINIE_Cat }, 
			{ "ValsGrab", API_GINIE_GrabVals },
			{ "Vals",API_GINIE_Val }
		};
		InstallAPI("GINIE", IAPI);
	}

}
