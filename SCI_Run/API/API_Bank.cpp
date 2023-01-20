#include <SlyvBank.hpp>

#include "../SCI_Script.hpp"

using namespace Slyvina;
using namespace Lunatic;
using namespace Units;


namespace Scyndi_CI {
	static std::map<std::string, Bank> BankRegister{};

	static int API_CreateBank(lua_State* L) {
		static uint64 count{ 0 };
		auto Tag{ Upper(Lunatic_CheckString(L,1)) };
		auto Bnk{ CreateBank(luaL_checkinteger(L,2)) };
		if (Tag == "") {
			Tag = TrSPrintF("***AUTO_%08x", count++);
		} else if (Prefixed(Tag, "*")) {
			luaL_error(L, "Cannot create a bank with a reserved tag! '%s'", Tag.c_str());
			return 0;
		}
		BankRegister[Tag] = Bnk;
		Lunatic_PushString(L, Tag);
		return 1;
	}

	static int API_PokeBank(lua_State* L) {
		auto Tag{ Upper(Lunatic_CheckString(L,1)) };
		auto
			Address{ luaL_checkinteger(L,2) },
			Value{ luaL_checkinteger(L,3) },
			Bits{ luaL_optinteger(L,4,8) };
		if (!BankRegister.count(Tag)) luaL_error(L, "There is no bank tagged '%s'", Tag.c_str());
		auto Bnk{ BankRegister[Tag] };
		switch (Bits) {
		case 8:	Bnk->Poke(Address, (byte)Value); break;
		case 16: Bnk->Poke(Address, (uint16)Value); break;
		case 32: Bnk->Poke(Address, (uint32)Value); break;
		case 64: Bnk->Poke(Address, (uint64)Value); break;
		case -8: Bnk->Poke(Address, (char)Value); break;
		case -16: Bnk->Poke(Address, (int16)Value); break;
		case -32: Bnk->Poke(Address, (int32)Value); break;
		case -64: Bnk->Poke(Address, (uint64)Value); break;
		default: luaL_error(L, "Unknown bitrate for poke (%d)", Bits); break;
		}
		return 0;
	}

	static int API_PeekBank(lua_State* L) {
		auto Tag{ Upper(luaL_checkstring(L,1)) };
		auto
			Address{ luaL_checkinteger(L,2) },
			Bits{ luaL_optinteger(L,3,8) };
		if (!BankRegister.count(Tag)) luaL_error(L, "There is no bank tagged '%s'", Tag.c_str());
		auto Bnk{ BankRegister[Tag] };
		switch (Bits) {
		case 8:lua_pushinteger(L, Bnk->PeekByte(Address)); break;
		case 16:lua_pushinteger(L, Bnk->PeekUInt16(Address)); break;
		case 32:lua_pushinteger(L, Bnk->PeekUInt32(Address)); break;
		case 64:lua_pushinteger(L, Bnk->PeekUInt64(Address)); break;
		case -8:lua_pushinteger(L, Bnk->PeekChar(Address)); break;
		case -16:lua_pushinteger(L, Bnk->PeekInt16(Address)); break;
		case -32:lua_pushinteger(L, Bnk->PeekInt32(Address)); break;
		case -64:lua_pushinteger(L, Bnk->PeekInt32(Address)); break;
		default: luaL_error(L, "Unknown bitrate for peek (%d)", Bits); return 0;
		}
		return 1;
	}

	static int API_KillBank(lua_State* L) {
		auto Tag{ Upper(luaL_checkstring(L,1)) };
		if (BankRegister.count(Tag)) BankRegister.erase(Tag);
		return 0;
	}

	Bank SGGet(std::string Tag) { return BankRegister[Upper(Tag)]; } // Needed for the savegame system


	void Init_API_Events() {
		std::map<std::string, lua_CFunction>IAPI{
			{"Create",API_CreateBank},
			{"Kill",API_KillBank},
			{"Poke",API_PokeBank},
			{"Peek",API_PeekBank}
		};
		InstallAPI("Bank", IAPI);
	}
}