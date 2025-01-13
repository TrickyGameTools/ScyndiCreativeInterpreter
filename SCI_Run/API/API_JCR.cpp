// License:
// 
// Scyndi's Creative Interpreter
// JCR6 handling
// 
// 
// 
// 	(c) Jeroen P. Broks, 2023, 2025
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

#include "../SCI_Script.hpp"


using namespace Slyvina;
using namespace JCR6;
using namespace Units;
using namespace NSLunatic;


namespace Scyndi_CI {

	class RegJCR {
	public:
		JT_Dir Res;
		std::shared_ptr < std::vector<JT_Entry>> Entries;

		RegJCR() {} // FUCK YOU!

		RegJCR(JT_Dir _Res) {
			Res = _Res;
			Entries = Res->Entries();
		}
	};

	static std::map<std::string, RegJCR> JCR_Register{}; //{ {"*MAIN", RegJCR(Resource()) } };

	static int API_GetJCR(lua_State* L) {
		auto
			Tag{ Upper(luaL_checkstring(L,1)) },
			File{ Lunatic_CheckString(L,2) };
		if (Prefixed(Tag, "*")) { luaL_error(L, "Tag '%s' is reserved!", luaL_checkstring(L, 1)); return 0; }
		auto J{ JCR6_Dir(File) }; // Please note that this handles the REAL file system. 
		if (Last()->Error) {
			luaL_error(L, "Error '%s' while reading: %s", Last()->ErrorMessage.c_str(), File.c_str());
			return 0;
		}
		JCR_Register[Tag] = RegJCR(J);
		return 0;
	}

	static int API_JCRSize(lua_State* L) {
		auto Tag{ Upper(luaL_optstring(L,1,"*MAIN")) };
		if (!JCR_Register.count(Tag)) { luaL_error(L, "There is no JCR6 resource loaded on tag '%s'", Tag.c_str()); return 0; }
		lua_pushinteger(L, JCR_Register[Tag].Entries->size());
		return 1;
	}

	static int API_JCREntry(lua_State* L) {
		auto Tag{ Upper(luaL_checkstring(L,1)) };
		auto idx{ (uint32)luaL_checkinteger(L,2) };
		if (!JCR_Register.count(Tag)) { luaL_error(L, "There is no JCR6 resource loaded on tag '%s'", Tag.c_str()); return 0; }
		if (idx < 0 || idx >= JCR_Register[Tag].Entries->size()) { luaL_error(L, "JCR6 '%s' index %d out of bounds", Tag.c_str(), idx); return 0; }
		auto e{ (*JCR_Register[Tag].Entries)[idx] };
		lua_pushstring(L, e->Name().c_str());
		lua_pushinteger(L, e->RealSize());
		lua_pushinteger(L, e->CompressedSize());
		lua_pushstring(L, e->_ConfigString["__Storage"].c_str());
		return 4;
	}

	static int API_JCREntryExists(lua_State* L) {
		auto
			Tag{ Upper(luaL_checkstring(L,1)) },
			File{ Lunatic_CheckString(L,2) };
		if (!JCR_Register.count(Tag)) { luaL_error(L, "There is no JCR6 resource loaded on tag '%s'", Tag.c_str()); return 0; }
		lua_pushboolean(L, JCR_Register[Tag].Res->EntryExists(File));
		return 1;
	}

	static int API_JCRGetString(lua_State* L) {
		auto
			Tag{ Upper(luaL_checkstring(L,1)) },
			File{ Lunatic_CheckString(L,2) };
		if (!JCR_Register.count(Tag)) { luaL_error(L, "There is no JCR6 resource loaded on tag '%s'", Tag.c_str()); return 0; }
		if (!JCR_Register[Tag].Res->EntryExists(File)) { luaL_error(L, "Resource (%s) does not have a file named: %s", Tag.c_str(), File.c_str()); return 0; }
		auto content{ JCR_Register[Tag].Res->GetString(File) };
		if (Last()->Error) {
			luaL_error(L, "Error '%s' (%s::%s)", Last()->ErrorMessage.c_str(), Last()->MainFile.c_str(), Last()->Entry.c_str());
			return 0;
		}
		lua_pushstring(L, content.c_str());
		return 1;
	}


	std::vector<std::string> FoundFiles{};
	size_t FoundFilesIndex{ 0 };
	static int API_JCRFindFirst(lua_State* L) {
		auto
			Tag{ Upper(luaL_checkstring(L,1)) },
			Dir{ Upper(ChReplace(luaL_checkstring(L,2),'\\','/')) }; if (!Suffixed(Dir, "/")) Dir += "/";
		if (!JCR_Register.count(Tag)) { luaL_error(L, "There is no JCR6 resource loaded on tag '%s'", Tag.c_str()); return 0; }
		FoundFiles.clear();
		FoundFilesIndex = 1;
		auto entries{ JCR_Register[Tag].Res->Entries() };
		for (auto e : *entries) {
			auto UName{ Upper(e->Name()) };
			//std::cout << "FindFirst in " << Tag << " Directory: " << Dir << " --> " << e->Name() << "(Allow: "<< (Prefixed(UName, Dir)) <<")\n"; // DEBUG!
			if (Prefixed(UName, Dir)) FoundFiles.push_back(e->Name());
		}
		if (FoundFiles.size()) {
			Lunatic_PushString(L, FoundFiles[0]);
			return 1;
		}
		return 0;
	}

	static int API_JCRFindNext(lua_State* L) {
		if (FoundFilesIndex < FoundFiles.size()) {
			Lunatic_PushString(L, FoundFiles[FoundFilesIndex++]);
			return 1;
		}
		return 0;
	}


	void Init_API_JCR() {
		JCR_Register["*MAIN"] = RegJCR(Resource());
		std::map<std::string, lua_CFunction> IAPI {
			{ "GetJCR", API_GetJCR},
			{ "JCRSize",API_JCRSize },
			{ "JCREntry",API_JCREntry },
			{ "JCREntryExists",API_JCREntryExists },
			{ "JCRGetString",API_JCRGetString },
			{ "JCRFindFirst",API_JCRFindFirst },
			{ "JCRFindNext", API_JCRFindNext }
		};
		InstallAPI("JCR6", IAPI);
	}
}
