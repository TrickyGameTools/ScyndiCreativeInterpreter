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
		Aud(Tag, File);
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

	void Init_API_Audio() {
		std::map<std::string, lua_CFunction>IAPI{
			{ "KillAudio", API_KillAudio},
			{ "LoadAudio",API_LoadAudio },
			{ "HasAudio",API_HasAudio },
			{ "PlayAudio",API_PlayAudio }
		};
		InstallAPI("Audio", IAPI);
	}
}