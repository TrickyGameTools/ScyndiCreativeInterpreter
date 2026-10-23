// License:
// 
// Scyndi's Creative Interpreter
// Real File
// 
// 
// 
// 	(c) Jeroen P. Broks, 2024, 2025, 2026
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
// Version: 26.10.23
// End License


#include <Slyvina.hpp>
#include <Lunatic.hpp>
#include <SlyvRequestFile.hpp>
#include <SlyvDir.hpp>
#include <SlyvDirry.hpp>
#include <SlyvQCol.hpp>
#include <SlyvString.hpp>
#include <SlyvStream.hpp>

#include "../SCI_Script.hpp"


#ifdef SlyvLinux
#include <cstdlib>
#define NoFileRequest
#define zenity
#endif // SlyvLinux

#ifdef zenity
#include <stdio.h>
#endif

#ifdef NoFileRequest
#include <TQSE.hpp>
using namespace Slyvina::TQSE;
#endif // NoFileRequest


using namespace Slyvina;
using namespace Units;
using namespace NSLunatic;

namespace Scyndi_CI {

	static VecString ePath() {
		auto ret{NewVecString()};
		#ifdef SlyvWindows
		auto p{Split(std::getenv("PATH"),';')};
		#else
		auto p{Split(std::getenv("PATH"),':')};
		#endif
		for(auto&pi:*p) ret->push_back(pi);
		return ret;
	}

	static String InPath(String F) {
		auto pth{ePath()};
		for(auto &pf:*pth) if (FileExists(pf+"/"+F)) return pf+"/"+F;
		return "";
	}

	static int API_RF_RequestFile(lua_State* L) {
		#ifdef zenity
        auto exe_zenity{InPath("zenity")};
        if (exe_zenity=="") {
            QCol->Error("This action requires the dependency 'zenity' which has not been found");
            return 0;
        }
        std::string com_zenity{exe_zenity+" --file-selection --title=\""+Lunatic_OptString(L, 1, "Please select a file")+"\""};

        FILE* pipe = popen(com_zenity.c_str(), "r");
        if (!pipe) {
            QCol->Error("Error opening pipe: "+exe_zenity);
            return 0;
        }
        std::string result;
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        int status = pclose(pipe);
        Lunatic_PushString(L,Trim(result));
        return 1;
	    #elif defined(NoFileRequest)
	    Notify("Unfortunately file requesting is not yet possible in "+Platform()+"\n\nThis has been planned for future versions, though!");
	    lua_pushstring(L,"");
        return 1;
	    #else
		auto ret {
			RequestFile(
				Lunatic_OptString(L, 1, "Please select a file"),
				Lunatic_OptString(L,2,Dirry("$Home$")),
				Lunatic_OptString(L,3,"All:*") + "\000",
				false)
			};
		Lunatic_PushString(L, ret);
        return 1;
        #endif
	}

    static int API_RF_Pipe(lua_State*L){
        // TODO: VS is a bit on the fuck when it comes o pipe. A temp file can help here, but I really hope it doesn't have to come to that.
        auto cm{luaL_checkstring(L,1)};
        FILE* pipe = popen(cm, "r");
        if (!pipe) {
            QCol->Error("Error opening pipe: "+(std::string)cm);
            return 0;
        }
        std::string result;
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        int status = pclose(pipe);
        Lunatic_PushString(L,Trim(result));
        lua_pushinteger(L,status);
        return 2;
    }

	static int API_RF_RequestSaveFile(lua_State* L) {
        #ifdef NoFileRequest
	    Notify("Unfortunately file requesting is not yet possible in "+Platform()+"\n\nThis has been planned for future versions, though!");
	    lua_pushstring(L,"");
	    #else
		auto ret {
			RequestFile(
				Lunatic_OptString(L, 1, "Please select a file"),
				Lunatic_OptString(L,2,Dirry("$Home$")),
				Lunatic_OptString(L,3,"All:*") + "\000",
				true)
			};

		Lunatic_PushString(L, ret);
		#endif
		return 1;
	}

	static int API_RF_SaveText(lua_State* L) {
		auto
            FileName{ Dirry(Lunatic_CheckString(L,1)) },
			Text{ Lunatic_CheckString(L,2) },
			Dir{ ExtractDir(FileName) };
		if (!DirectoryExists(Dir)) MakeDir(Dir);
		SaveString(FileName, Text);
		return 0;
	}

	static int API_RF_LoadText(lua_State* L) {
		auto
            FileName{ Dirry(Lunatic_CheckString(L,1)) };
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

    static int API_RF_GetDir(lua_State *L) {
        auto idx{luaL_optinteger(L,1,-1)};
        static std::map<int,DirWant> wantmap{ {0,DirWant::Files},{1,DirWant::Directories},{3,DirWant::FilesAndDirectories},{4,DirWant::Tree}};
        static VecString Lijst{nullptr};
        if(idx<0) {
            auto want{luaL_checkinteger(L,2)};
            auto pth{Lunatic_CheckString(L,3)};
            auto allowhidden{Lunatic_CheckBoolean(L,4)};
            Lijst=FileList(pth,wantmap[want],allowhidden);
            lua_pushinteger(L,Lijst->size());
            return 1;
        }
        if (!Lijst) { luaL_error(L,"ReadFile.GetDir: No dir read yet!"); return 0; }
        if (idx>=Lijst->size()) { luaL_error(L,"RealFile.GetDir: Index %d too high (%d)",idx,Lijst->size()); return 0; }
        Lunatic_PushString(L,(*Lijst)[idx]);
        return 1;
    }

    static int API_RF_ChkFile(lua_State*L) {
        auto f{Dirry(Lunatic_CheckString(L,1))};
        if (IsFile(f)) lua_pushstring(L,"file");
        else if (IsDir(f)) lua_pushstring(L,"dir");
        else lua_pushnil(L);
        return 1;
    }

    static int API_RF_MkDir(lua_State*L) {
        lua_pushboolean(L,MkDir(Dirry(luaL_checkstring(L,1))));
        return 1;
    }

	void Init_API_RealFile() {
		std::map<std::string, lua_CFunction>IAPI{
			{ "RequestFile", API_RF_RequestFile},
			{ "RequestSaveFile", API_RF_RequestSaveFile },
			{ "SaveText",API_RF_SaveText },
            { "LoadText", API_RF_LoadText },
            { "GetDir", API_RF_GetDir },
            { "Pipe",API_RF_Pipe },
            { "Check",API_RF_ChkFile },
            { "MkDir",API_RF_MkDir }
		};
		InstallAPI("RealFile", IAPI);
	}

}
