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

#include <TQSG.hpp>
#include <TQSE.hpp>

#include <SlyvQCol.hpp>

#include "SCI_Crash.hpp"
#include "SCI_Script.hpp"
#include "SCI_JCR.hpp"
#include "SCI_Config.hpp"

using namespace Slyvina;
using namespace Units;
using namespace Lunatic;
using namespace TQSG;
using namespace TQSE;

namespace Scyndi_CI {
	struct SCFlow { std::string Name; SLunatic State{ nullptr }; };
	SCFlow CFlow;
	bool EndRun{ false };
	static std::map < std::string, SLunatic > StateRegister{};
	
	static std::map<std::string, std::map<std::string, lua_CFunction>> API{};

	

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

	static int SYS_Exit(lua_State* L) { EndRun = true; return 0; }
	static int DBG_OnOff(lua_State* L) {
		auto St{ std::string(luaL_checkstring(L,1)) };
		State(St)->Debug = luaL_checkinteger(L, 2);
		return 0; }
	static int DBG_Push(lua_State* L) {
		_LunDbg k;
		/*
		auto
			St{ luaL_checkstring(L,1) },
			Sc{ luaL_checkstring(L,2) },
			Fn{ luaL_checkstring(L,3) };
		auto
			Ln{ luaL_checkinteger(L,4) };
		k.File = Sc;
		k.Func = Fn;
		k.Line = Ln;
		State(St)->Trace.push_back(k);
		*/
		k.Func = luaL_checkstring(L, 2);
		State(luaL_checkstring(L, 1))->Trace.push_back(k);
		return 0;
	}
	static int DBG_Line(lua_State* L) {
		auto St{ luaL_checkstring(L,1) };
		auto S{ State(St) };
		auto k{ &(S->Trace[S->Trace.size() - 1]) };
		k->File = luaL_checkstring(L, 2);
		k->Line = luaL_checkinteger(L, 3);
		return 0;
	}

	static int DBG_Pop(lua_State* L) { State(luaL_checkstring(L, 1))->Trace.pop_back(); }

	static int SYS_Crash(lua_State* L) {
		auto
			St{ luaL_checkstring(L,1) },
			Er{ luaL_checkstring(L,2) };
		auto
			Uitleg{ NewVecString() };
		for (auto& k : State(St)->Trace) {
			Uitleg->push_back(TrSPrintF("%s:Line %05d: %s", k.File.c_str(), k.Line, k.Func.c_str()));
		}
		Crash(TrSPrintF("Lua Error: %s", Er), Uitleg);
		return 0; // Should never be executed, but hey, this way I can at least play safe, eh?
	}
	

	static void InitScript() {
		static bool done{ false };
		if (done) return;
		QCol->Doing("Init", "Script engine");
		_Lunatic::Panick = PaniekLua;
		_Lunatic::Register4All({
			{"SCI_Exit",SYS_Exit},
			{"SCI_Crash",SYS_Crash},
			{"__DEBUG_ONOFF",DBG_OnOff},
			{"__DEBUG_LINE",DBG_Line},
			{"__DEBUG_PUSH",DBG_Push},
			{"__DEBUG_POP",DBG_Pop}		
			});
	}

	std::string MainScript() {
		InitScript();
		static std::string ret{ "" };
		if (!ret.size()) {
			auto E = Resource()->Entries();
			for (auto f : *E) {
				auto mf = Upper(f->Name());
				auto tf = StripDir(mf);
				//std::cout << "MainScript Search: " << mf << " -> " << tf << std::endl; // debug
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
		Trans2Upper(_State);
		if (!_Res->EntryExists(_Entry)) throw std::runtime_error(TrSPrintF("There is no script named '%s'"));
		auto Buf{ _Res->B(_Entry) };
		static auto Core{ SRF()->GetString("Script/ScyndiCore.lua") };
		//StateRegister[_State] = LunaticByByteCode(Buf,_Entry);
		QCol->Doing("Loading", _Entry, "\t"); QCol->Doing("to state", _State);
		StateRegister[_State] = LunaticBySource(Core); 
		std::string initscript{
			"-- InitScript: " + _State + " --\n\n"
			"Exit=SCI_Exit\n\n"
			"Scyndi.InitDebugFunctions(\n"
			"function() -- SetDebug\n"
			"\t__DEBUG_ONOFF('" + _State + "')\n"
			"end,\n"
			"'" + _State + "',\n"
			"function(funcName) -- Push\n"
			"\t__DEBUG_PUSH('" + _State + "',funcName)\n"
			"end,\n"
			"function() -- Pop\n"
			"\t__DEBUG_POP('" + _State + "')\n"
			"end,\n"
			"function (st,src,ln) -- Line\n"
			"\t__DEBUG_LINE('" + _State + "',src,ln)\n"
			"end,\n"
			"true\n"
			")\n\n\n"
			"function Crash(err)\n"
			"\tSCI_Crash('" + _State + "',err)\n"
			"end\n\n"
		};
		//std::cout << initscript << std::endl; // debug!
		StateRegister[_State]->QDoString(initscript);
		auto ext = Upper(ExtractExt(_Entry));
		if (ext == "LUA") {
			QCol->Doing("Compiling", _Entry);
			auto src{ _Res->GetString(_Entry) };
			StateRegister[_State]->QDoString(src);
		} else if (ext == "LBC") {
			auto bnk{ _Res->B(_Entry) };
			StateRegister[_State]->QDoByteCode(bnk->Direct(), bnk->Size(), _Entry);
		}

	}

	void State(std::string _Stage, std::string _Entry) { State(_Stage, Resource(), _Entry); }

	void GoToFlow(std::string State) {
		CFlow.Name = "FLOW_" + State;
		CFlow.State = Flow(State);
	}

	void Call(std::string _State, std::string Func, std::string Para) {
		std::string src{"-- Call to State: "+_State+"--\n\n"};
		if (!Para.size()) Para = "nil";
		auto S{ State(_State) };
		if (S->Flag("NotScyndi"))
			src += TrSPrintF("local suc,err = pcall(%s,%s)\n", Func.c_str(), Para.c_str());
		else
			src += TrSPrintF("local suc,err = pcall(Scyndi.Globals.%s,%s)\n", Func.c_str(), Para.c_str());
		//src += "print(suc,err)\n"; // debug
		src += "if not suc then SCI_Crash('" + _State + "',err) end";
		// std::cout << src << "\n"; // debug
		S->QDoString(src);
	}

	void InstallAPI(std::string _API, std::map<string, lua_CFunction> _Funcs) {
		API[Upper(_API)] = _Funcs;
	}

	void RunGame() {
		auto RT{ GetRunType() };

		do {
			switch (RT) {
			case RunType::Unknown:
				Crash(TrSPrintF("Unknown Run-Type (U%d)", (int)RT), "This is an internal error in Scyndi's Creative Interpreter\nPlease go to https://github.com/TrickyGameTools/ScyndiCreativeInterpreter/issues\nand write an issue about this matter!");
				return;
			case RunType::Straight:
				Call("Flow_Main", "Main");
				return;
			case RunType::Flow:
				Call(CFlow.Name, "MainFlow");
				break;
			case RunType::Callback: {
				Cls();
				Call(CFlow.Name, "CB_Draw");
				Flip();
				Poll();
				Call(CFlow.Name, "CB_Update");
				static auto keys{ KeyArray() };
				for (auto k : keys) {
					if (KeyDown(k)) Call(CFlow.Name, "CB_KeyDown", to_string((int)k));
					if (KeyHit(k)) Call(CFlow.Name, "CB_KeyHit", to_string((int)k));
				}
				for (int mb = 0; mb < GetMouseButtons(); mb++) {
					if (MouseDown(mb)) Call(CFlow.Name, "CB_MouseDown", to_string(mb));
					if (MouseHit(mb)) Call(CFlow.Name, "CB_MouseHit", to_string(mb));
					if (MouseReleased(mb)) Call(CFlow.Name, "CB_MouseReleased", to_string(mb));
				}
			} break;
			default:
				Crash(TrSPrintF("Unknown Run-Type (D%d)", (int)RT), "This is an internal error in Scyndi's Creative Interpreter\nPlease go to https://github.com/TrickyGameTools/ScyndiCreativeInterpreter/issues\nand write an issue about this matter!");
				return;
			}
		} while (!EndRun);
	}

}