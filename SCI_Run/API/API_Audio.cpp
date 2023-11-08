// Lic:
// Scyndi's Creative Interpreter
// Audio API
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
// Version: 23.11.03
// EndLic
#include <Lunatic.hpp>

#include "../SCI_Crash.hpp"
#include "../SCI_Script.hpp"
#include "../SCI_Audio.hpp"

using namespace Slyvina;
using namespace Units;
using namespace Lunatic;

namespace Scyndi_CI {

	static int API_LoadAudio(lua_State* L) {
		size_t Untagged{ 0 };
		auto
			File{ Lunatic_CheckString(L,1) },
			Tag{ Lunatic_OptString(L,2,"") };
		if (!Tag.size()) {
			Tag = TrSPrintF("***TAG:%08x", Untagged++);
		} else if (Prefixed(Tag, "*")) {
			Crash("Tags for LoadImage may not be prefixed with *");
			return 0;
		}
		Aud(Tag, Resource(), File);
		Lunatic_PushString(L, Tag);
		return 1;
	}

	static int API_KillAudio(lua_State* L) {
		KillAud(luaL_checkstring(L, 1));
		return 0;
	}

	static int API_HasAudio(lua_State* L) {
		lua_pushboolean(L, HasAud(luaL_checkstring(L, 1)));
		return 1;
	}

	static int API_PlayAudio(lua_State* L) {
		auto
			Tag{ luaL_checkstring(L,1) };
		auto
			Channel{ luaL_optinteger(L,2,-1) },
			Loops{ luaL_optinteger(L,3,0) };
		auto
			ret{ 0 };
		if (Channel < 0)
			ret = Aud(Tag)->Play(Loops);
		else {
			ret = Channel;
			Aud(Tag)->ChPlay(Channel, Loops);
		}
		lua_pushinteger(L, 1);
		return 1;
	}
	static int API_StopChannel(lua_State* L) {
		Mix_HaltChannel(luaL_checkinteger(L, 1));
		return 0;
	}

	static int API_Playing(lua_State* L) {
		lua_pushboolean(L, Mix_Playing(luaL_checkinteger(L, 1)));
		return 1;
	}

	void Init_API_Audio() {
		std::map<std::string, lua_CFunction>IAPI{
			{ "KillAudio", API_KillAudio},
			{ "LoadAudio",API_LoadAudio },
			{ "HasAudio",API_HasAudio },
			{ "PlayAudio",API_PlayAudio },
			{ "StopChannel",API_StopChannel },
			{ "Playing",API_Playing }
		};
		InstallAPI("Audio", IAPI);
	}
}