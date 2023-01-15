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
// Version: 23.01.15
// EndLic

#include <SlyvQCol.hpp>

#include "../SCI_Config.hpp"
#include "../SCI_Script.hpp"
#include "../SCI_Crash.hpp"

using namespace Slyvina;
using namespace Lunatic;
using namespace Units;

namespace Scyndi_CI {

	static int SG_SaveString(lua_State* L) {
		auto
			File{ SaveGameDir() + "/" + Lunatic_CheckString(L,1) },
			Content{ Lunatic_CheckString(L,2) },
			Dir{ ExtractDir(File) };
		if (!DirectoryExists(Dir)) {
			QCol->Doing("Creating", Dir);
		}
		SaveString(File, Content);
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
		}
		return 1;
	}

	void Init_API_SGFile(){}
}