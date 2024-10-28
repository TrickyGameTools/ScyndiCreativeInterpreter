// License:
// 
// Scyndi's Creative Interpreter
// Real File
// 
// 
// 
// 	(c) Jeroen P. Broks, 2024
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
// Version: 24.10.28
// End License
// Lic:
// Scyndi's Creative Interpreter
// Real File
// 
// 
// 
// (c) Jeroen P. Broks, 2024
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

#include <Lunatic.hpp>
#include <SlyvRequestFile.hpp>
#include <SlyvDirry.hpp>
#include <SlyvString.hpp>
#include <SlyvStream.hpp>

#include "../SCI_Script.hpp"

using namespace Slyvina;
using namespace Units;
using namespace NSLunatic;

namespace Scyndi_CI {

	static int API_RF_RequestFile(lua_State* L) {
		auto ret {
			RequestFile(
				Lunatic_OptString(L, 1, "Please select a file"),
				Lunatic_OptString(L,2,Dirry("$Home$")),
				Lunatic_OptString(L,3,"All:*") + "\000",
				false)
			};
		Lunatic_PushString(L, ret);
		return 1;
	}

	static int API_RF_RequestSaveFile(lua_State* L) {
		auto ret {
			RequestFile(
				Lunatic_OptString(L, 1, "Please select a file"),
				Lunatic_OptString(L,2,Dirry("$Home$")),
				Lunatic_OptString(L,3,"All:*") + "\000",
				true)
			};
		
		Lunatic_PushString(L, ret);
		return 1;
	}

	static int API_RF_SaveText(lua_State* L) {
		auto
			FileName{ Lunatic_CheckString(L,1) },
			Text{ Lunatic_CheckString(L,2) },
			Dir{ ExtractDir(FileName) };
		if (!DirectoryExists(Dir)) MakeDir(Dir);
		SaveString(FileName, Text);
		return 0;
	}

	static int API_RF_LoadText(lua_State* L) {
		auto
			FileName{ Lunatic_CheckString(L,1) };
		if (!FileExists(FileName)) {
			Lunatic_PushString(L,"File \"" + FileName = "\" has not been found!");
			lua_pushboolean(L, false);
			return 2;
		}
		try {
			Lunatic_PushString(L, FLoadString(FileName));
			lua_pushboolean(L, true);
			return 2;
		} catch (std::runtime_error e) {
			Lunatic_PushString(L, e.what());
			lua_pushboolean(L, false);
			return 2;
		}

	}


	void Init_API_RealFile() {
		std::map<std::string, lua_CFunction>IAPI{
			{ "RequestFile", API_RF_RequestFile},
			{ "RequestSaveFile", API_RF_RequestSaveFile },
			{ "SaveText",API_RF_SaveText },
			{ "LoadText", API_RF_SaveText }
		};
		InstallAPI("RealFile", IAPI);
	}

}
