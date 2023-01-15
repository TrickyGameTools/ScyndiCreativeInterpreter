// Lic:
// Scyndi's Creative Interpreter
// InterVar C++ code
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
// Version: 23.01.15
// EndLic
#include <Lunatic.hpp>

#include <Slyvina.hpp>
#include <SlyvTime.hpp>
#include <SlyvString.hpp>
#include <SlyvGINIE.hpp>
#include <SlyvSTOI.hpp>

#include "../SCI_Crash.hpp"
#include "../SCI_Script.hpp"

using namespace Slyvina;
using namespace Units;
using namespace Lunatic;

namespace Scyndi_CI {
	
	struct _GvType {
		std::map<std::string, int64> _Int{};
		std::map<std::string, bool> _Bool{};
		std::map<std::string, std::string> _Str{};
	};
	static std::map<std::string, _GvType> GvRegister;

	std::string GvUnParse(std::string Tag) {
		Trans2Upper(Tag);
		if (!GvRegister.count(Tag)) { Crash("No InterVar named '" + Tag + "' found"); return ""; }
		auto Ret{ TrSPrintF("# InterVar Rollout: %s\n# Generated: %s\n\n",Tag.c_str(),CurrentDate().c_str()) };
		Ret += "\n[Boolean]\n"; for (auto& IT : GvRegister[Tag]._Bool) Ret += IT.first + "=" + boolstring(IT.second) + "\n";
		Ret += "\n[Integer]\n"; for (auto& IT : GvRegister[Tag]._Int) Ret += TrSPrintF("%s=%d\n", IT.first.c_str(), IT.second);
		Ret += "\n[String]\n"; for (auto& IT : GvRegister[Tag]._Str) Ret += IT.first + "=" + IT.second + "\n";
		Ret += "\n";
		return Ret;
	}

	void GvParse(std::string Tag, std::string src, bool Merge = false) {
		auto G{ ParseUGINIE(src) };
		auto
			B{ G->Values("Boolean") },
			I{ G->Values("Integer") },
			S{ G->Values("String") };
		Trans2Upper(Tag);
		if ((!Merge) && GvRegister.count(Tag)) {
			GvRegister[Tag]._Bool.clear();
			GvRegister[Tag]._Int.clear();
			GvRegister[Tag]._Str.clear();
		}
		for (auto& IT : *B) GvRegister[Tag]._Bool[IT] = G->Value("Boolean", IT) == "TRUE";
		for (auto& IT : *I) GvRegister[Tag]._Int[IT] = G->IntValue("Integer", IT);
		for (auto& IT : *S) GvRegister[Tag]._Str[IT] = G->Value("String", IT);
	}

	static int API_GvUnParse(lua_State* L) {
		lua_pushstring(L, GvUnParse(luaL_checkstring(L, 1)).c_str());
		return 1;
	}

	static int API_GvParse(lua_State* L) {
		GvParse(luaL_checkstring(L, 1), luaL_checkstring(L, 2), luaL_optinteger(L, 3, 0));
		return 0;
	}

	static int API_GvKill(lua_State* L) {
		auto Tag{ Upper(luaL_checkstring(L,1)) };
		if (Prefixed(Tag, "*")) { Crash("Tag " + Tag + " contains an InterVar register that may not be destroyed"); return 0; }
		if (GvRegister.count(Tag)) GvRegister.erase(Tag);
		return 0;
	}

	static int API_GvDefine(lua_State* L) {
		auto
			Tag{ Upper(luaL_checkstring(L,1)) },
			VarN{ Upper(luaL_checkstring(L,3)) };
		if (Prefixed(Tag, "*") && (!GvRegister.count(Tag))) { Crash("InterVar '" + Tag + "' cannot be created like this"); return 0; }
		switch(luaL_checkinteger(L,2)){
		case 1:
			GvRegister[Tag]._Bool[VarN] = luaL_checkinteger(L, 4);
			break;
		case 2:
			GvRegister[Tag]._Int[VarN] = luaL_checkinteger(L, 4);
			break;
		case 3:
			GvRegister[Tag]._Str[VarN] = luaL_checkstring(L, 5);
			break;
		default:
			Crash(TrSPrintF("Unknown interval type definition! (%d)", luaL_checkinteger(L, 2)));
			return 0;
		}
		return 0;
	}

	int API_GvCheck(lua_State* L) {
		auto
			Tag{ Upper(luaL_checkstring(L,1)) },
			VarN{ Upper(luaL_checkstring(L,3)) };
		if (Prefixed(Tag, "*") && (!GvRegister.count(Tag))) { Crash("InterVar '" + Tag + "' cannot be created like this"); return 0; }
		switch (luaL_checkinteger(L, 2)) {
		case 1:
			lua_pushboolean(L, GvRegister[Tag]._Bool[VarN]);
			return 1;
		case 2:
			lua_pushinteger(L, GvRegister[Tag]._Int[VarN]);
			return 1;
		case 3:
			Lunatic_PushString(L, GvRegister[Tag]._Str[VarN]);
			return 1;
		default:
			Crash(TrSPrintF("Unknown interval type checking! (%d)", luaL_checkinteger(L, 2)));
			return 0;
		}
	}


	void Init_API_Vars() {
		GvParse("*GENERAL", "# Nothing"); // Just makes sure the record exists!
		std::map<std::string, lua_CFunction>IAPI{
			{"UnParse",API_GvUnParse},
			{"Parse",API_GvParse},
			{"Kill",API_GvKill},
			{"Define",API_GvDefine},
			{"Check",API_GvCheck}
		};
		InstallAPI("InterVar", IAPI);
	}
}