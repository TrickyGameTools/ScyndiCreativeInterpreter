// Lic:
// Scyndi's Creative Interpreter
// Script & State Manager
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
// Version: 23.01.13
// EndLic

#include <Lunatic.hpp>

#include <SlyvQCol.hpp>

#include "SCI_Crash.hpp"
#include "SCI_Script.hpp"
#include "SCI_JCR.hpp"

using namespace Slyvina;
using namespace Units;
using namespace Lunatic;

namespace Scyndi_CI {
    static std::map < std::string, SLunatic > StateRegister{};

    static int PaniekLua(lua_State* L) {
        auto Uitleg{ NewVecString() };
        //cout << lua_gettop(L) << "\n";
        Uitleg->push_back(TrSPrintF("lua_gettop(State) = %d", lua_gettop(L)));
        for (int i = 1; i <= lua_gettop(L); i++) {
            auto line{ TrSPrintF("Arg #%d/%d:\t",i,lua_gettop(L)) };
            switch (lua_type(L, i)) {
            case LUA_TSTRING:
                line += TrSPrintF("String: \"%s\"",luaL_checkstring(L, i)); 
                break;
            case LUA_TNUMBER:
                line += TrSPrintF("Number: %f", luaL_checknumber(L, i));
                break;
            case LUA_TFUNCTION:
                line+= "Function";
                break;
            default:
                line += TrSPrintF("Unknown: %d", lua_type(L, i));
                break;
            }
            Uitleg->push_back(line);
        }
        Crash("Lua went in a state of panic", Uitleg);
        return 0;
    }

    static void InitScript() {
        static bool done{ false };
        if (done) return;
        QCol->Doing("Init", "Script engine");
        Slyvina::Lunatic::_Lunatic::Panick = PaniekLua;
    }

    std::string MainScript() {
        InitScript();
        static std::string ret{ "" };
        if (!ret.size()) {
            auto E = Resource()->Entries();
            for (auto f : *E) {
                auto mf = Upper(f->Name());
                auto tf = ExtractDir(mf);
                if (tf == "MAIN.LBC" || tf == "MAIN.LUA") ret = f->Name();
            }
        }
        if (!ret.size()) throw std::runtime_error("Neither MAIN.LBC nor MAIN.LUA found in any directory of the game's resource");
        return ret;
    }
    bool GotState(std::string _State) {
        Trans2Upper(_State);
        return StateRegister.count(_State);
    }
    Slyvina::Lunatic::SLunatic State(std::string _State) {
        Trans2Upper(_State);
        if (!StateRegister.count(_State)) { throw std::runtime_error(TrSPrintF("State '%s' doesn't exist", _State.c_str())); }
        return StateRegister[_State];
    }

    void State(std::string _State, Slyvina::Lunatic::SLunatic _Lun) {
        Trans2Upper(_State);
        if (!_Lun) {
            if (StateRegister.count(_State)) StateRegister.erase(_State);
            return;
        }
        StateRegister[_State] = _Lun;
    }

    void State(std::string _State, Slyvina::JCR6::JT_Dir _Res, std::string _Entry) {
        if (!_Res->EntryExists(_Entry)) throw std::runtime_error(TrSPrintF("There is no script named '%s'"));
        auto Buf{ _Res->B(_Entry) };
        StateRegister[_State] = LunaticByByteCode(Buf,_Entry);
    }

    void State(std::string _Stage, std::string _Entry) { State(_Stage, Resource(), _Entry); }
}