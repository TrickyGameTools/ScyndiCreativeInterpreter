// Lic:
// Scyndi's Creative Interpreter
// Bank API
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
// Version: 24.09.24
// EndLic
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
		if (Address >= Bnk->Size()) luaL_error(L, "Back[%s].Peek(%d):Out of bounds (%d)", Tag.c_str(), Address, Bnk->Size());
		switch (Bits) {
		case 8:lua_pushinteger(L, Bnk->PeekByte(Address)); break;
		case 16:lua_pushinteger(L, Bnk->PeekUInt16(Address)); break;
		case 32:lua_pushinteger(L, Bnk->PeekUInt32(Address)); break;
		case 64:lua_pushinteger(L, Bnk->PeekUInt64(Address)); break;
		case -8:lua_pushinteger(L, Bnk->PeekChar(Address)); break;
		case -16:lua_pushinteger(L, Bnk->PeekInt16(Address)); break;
		case -32:lua_pushinteger(L, Bnk->PeekInt32(Address)); break;
		case -64:lua_pushinteger(L, Bnk->PeekInt64(Address)); break;
		default: luaL_error(L, "Unknown bitrate for peek (%d)", Bits); return 0;
		}
		return 1;
	}

	static int API_KillBank(lua_State* L) {
		auto Tag{ Upper(luaL_checkstring(L,1)) };
		if (BankRegister.count(Tag)) BankRegister.erase(Tag);
		return 0;
	}

	static int API_Clean(lua_State* L) {
		auto Tag{ Upper(luaL_checkstring(L,1)) };
		if (!BankRegister.count(Tag)) luaL_error(L, "There is no bank tagged '%s'", Tag.c_str());
		auto Bnk{ BankRegister[Tag] };
		for (size_t p = 0; p < Bnk->Size(); p++) Bnk->Poke(p, (char)0);
		return 0;
	}

	Bank SGBank(std::string Tag) { return BankRegister[Upper(Tag)]; } // Needed for the savegame system
	void SGBank(std::string Tag, Bank Bnk) { BankRegister[Upper(Tag)] = Bnk; }
 
	void Init_API_Bank() {
		std::map<std::string, lua_CFunction>IAPI{
			{ "Create", API_CreateBank},
			{ "Kill",API_KillBank },
			{ "Poke",API_PokeBank },
			{ "Peek",API_PeekBank },
			{ "Clean",API_Clean }
		};
		InstallAPI("Bank", IAPI);
	}
}