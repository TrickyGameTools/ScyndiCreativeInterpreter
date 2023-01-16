// Lic:
// Scyndi's Creative Interpreter
// SGFile management API
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
// Version: 23.01.16
// EndLic

#include <SlyvDir.hpp>
#include <SlyvQCol.hpp>

#include "../SCI_Config.hpp"
#include "../SCI_Script.hpp"
#include "../SCI_Crash.hpp"

using namespace Slyvina;
using namespace Lunatic;
using namespace Units;

namespace Scyndi_CI {

	static VecString SG_Tree;
	inline void MakeTree() {
		if (!SG_Tree) SG_Tree = GetTree(SaveGameDir());
	}
	

	static int SG_SaveString(lua_State* L) {
		auto
			File{ SaveGameDir() + "/" + Lunatic_CheckString(L,1) },
			Content{ Lunatic_CheckString(L,2) },
			Dir{ ExtractDir(File) };
		if (!DirectoryExists(Dir)) {
			QCol->Doing("Creating", Dir);
		}
		SaveString(File, Content);
		SG_Tree = nullptr;
		return 0;
	}

	static int SG_LoadString(lua_State* L) {
		auto
			PFile{ Lunatic_CheckString(L,1) },
			FFile{ SaveGameDir() + "/" + PFile };
		if (!FileExists(FFile)) { Crash("File not found: " + PFile, "Full File Name: " + FFile); return 0; }
		Lunatic_PushString(L, FLoadString(FFile));
		return 1;
	}

	static int SG_DeleteFile(lua_State* L) {
		auto
			PFile{ Lunatic_CheckString(L,1) },
			FFile{ SaveGameDir() + "/" + PFile };
		if (!FileExists(FFile)) {
			QCol->Warn("Cannot delete " + FFile + " as it doesn't exist!");
			lua_pushboolean(L, false);
		} else {
			lua_pushboolean(L, FileDelete(FFile));
			SG_Tree = nullptr;
		}
		return 1;
	}

	static int SG_DirCount(lua_State* L) {
		MakeTree();
		lua_pushinteger(L, SG_Tree->size());
		return 1;
	}

	static int SG_DirEntry(lua_State* L) {
		MakeTree();
		auto idx{ luaL_checkinteger(L,1) };
		if (!(idx > 0 && idx < SG_Tree->size())) {
			luaL_error(L, "Directory index out of bounds (%d/%d)", idx, SG_Tree->size());
			return 0;
		}
		lua_pushstring(L, (*SG_Tree)[idx].c_str());
		return 1;
	}

	static int SG_FileExists(lua_State* L) {
		lua_pushboolean(L, FileExists(SaveGameDir() + "/" + luaL_checkstring(L, 1)));
		return 1;
	}

	static int SG_ResetDir(lua_State* L) {
		SG_Tree = nullptr;
		return 0;
	}
	

	void Init_API_SGFile() {
		std::map<std::string, lua_CFunction>IAPI{
			{"DirCount", SG_DirCount},
			{"DirEntry", SG_DirEntry},
			{"Delete", SG_DeleteFile},
			{"Kill", SG_DeleteFile},
			{"LoadString", SG_LoadString},
			{"SaveString", SG_SaveString},
			{"FileExists",SG_FileExists},
			{"ResetDir",SG_ResetDir},
			{"FileExists",SG_FileExists}
		};
		InstallAPI("SGFile", IAPI);
	}
}