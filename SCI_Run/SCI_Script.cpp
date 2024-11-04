// License:
// 
// Scyndi's Creative Interpreter
// Script & State Manager
// 
// 
// 
// 	(c) Jeroen P. Broks, 2023, 2024
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
// Version: 24.11.04 I
// End License

#include <Lunatic.hpp>

#include <TQSG.hpp>
#include <TQSE.hpp>

#include <SlyvQCol.hpp>
#include <SlyvOpenURL.hpp>


#include "SCI_Crash.hpp"
#include "SCI_Script.hpp"
#include "SCI_JCR.hpp"
#include "SCI_Config.hpp"

#include "API/SCI_InstallAPIs.hpp"
#include "API/API_DebugConsole.hpp"

using namespace Slyvina;
using namespace Units;
using namespace JCR6;
using namespace NSLunatic;
using namespace TQSG;
using namespace TQSE;
using namespace std;

namespace Scyndi_CI {
	struct SCFlow { std::string Name; SLunatic State{ nullptr }; };
	SCFlow CFlow;
	bool EndRun{ false };
	int DontFlip{ 0 };
	static std::map < std::string, SLunatic > StateRegister{};

	static std::map<std::string, std::map<std::string, lua_CFunction>> API{};

#pragma region Paniek	

	static int PaniekLua(lua_State* L) {
		auto Uitleg{ NewVecString() };
		//cout << lua_gettop(L) << "\n";
		Uitleg->push_back(TrSPrintF("lua_gettop(State) = %d", lua_gettop(L)));
		for (int i = 1; i <= lua_gettop(L); i++) {
			auto line{ TrSPrintF("Arg #%d/%d:\t",i,lua_gettop(L)) };
			switch (lua_type(L, i)) {
			case LUA_TSTRING:
				line += TrSPrintF("String: \"%s\"", luaL_checkstring(L, i));
				break;
			case LUA_TNUMBER:
				line += TrSPrintF("Number: %f", luaL_checknumber(L, i));
				break;
			case LUA_TFUNCTION:
				line += "Function";
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
#pragma endregion

#pragma region "Core APIs"
	// These APIs must always be loaded, regardless if the script to shortcut them is loaded or not.
	// Scyndi will still be able to find them anyway (if you know what you are doing).
	static int SYS_Exit(lua_State* L) { EndRun = true; return 0; }
	static int DBG_OnOff(lua_State* L) {
		auto St{ std::string(luaL_checkstring(L,1)) };
		State(St)->Debug = luaL_checkinteger(L, 2);
		return 0;
	}
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
		auto state = luaL_checkstring(L, 1);
		//std::cout << "Push: " << state << "::" << k.Func << "!\n";
		if (HasState(state)) {
			State(state)->Trace.push_back(k);
			//std::cout << "Done: " << State(state)->Trace.size() << "\n";
		}
		return 0;
	}
	static int DBG_Line(lua_State* L) {
		auto St{ luaL_checkstring(L,1) };
		if (HasState(St)) {
			auto S{ State(St) };
			if (!S->Trace.size()) {
				//QCol->Error("Trace data empty!");
				//std::cout << "Trace " << luaL_checkstring(L, 1) << "::" << luaL_checkstring(L, 2) << "::" << luaL_checkinteger(L, 3) << "!\n"; // debug
				return 0;
			}
			//auto k{ &(S->Trace[S->Trace.size() - 1]) };
			//if (!k) {
			//	QCol->Error("Trace data caught is null");
			//	return 0;
			//}
			S->Trace[S->Trace.size() - 1].File = luaL_checkstring(L, 2);
			S->Trace[S->Trace.size() - 1].Line = luaL_checkinteger(L, 3);
		}
		return 0;
	}

	static int DBG_Pop(lua_State* L) {
		auto St{ luaL_checkstring(L, 1) };
		if (HasState(St)) {
			//std::cout << "Pop " << St << " " << State(St)->Trace.size();
			if (!State(St)->Trace.size()) {
				//QCol->Warn("Trace pop on empty trace");
			} else
				State(St)->Trace.pop_back();
			//std::cout << " -> " << State(St)->Trace.size()<<"\n";
		}
		return 0;
	}

	static int DBG_ShowTraceback(lua_State* L) {
		auto St{ luaL_checkstring(L, 1) };
		auto &sb{ State(St)->Trace };
		QCol->Doing("State", St);
		if (!State(St)->Trace.size()) {
			QCol->Doing("Traceback", "Emptry");
		} else {
			QCol->Doing("Traceback", sb.size());
			for (auto& sbi : sb) {
				QCol->LMagenta("===============\n");
				QCol->Doing("File", sbi.File);
				QCol->Doing("Function", sbi.Func);
				QCol->Doing("Line", sbi.Line);
			}
		}
	}

	static int SYS_Crash(lua_State* L) {
		auto
			St{ luaL_checkstring(L,1) },
			Er{ luaL_checkstring(L,2) };
		auto
			Uitleg{ NewVecString() };
		if (!HasState(St)) {
			Uitleg->push_back(TrSPrintF("It looks like state '%s' where the error occurred does not exist.", St));
			Uitleg->push_back("Nothing to worry about, this can be normal.");
			Uitleg->push_back("When the state is already unliked from the memory, so leftover destructors in the script");
			Uitleg->push_back("could need to be taken care of. It is possible the error happened in one of those destructors.");
			Uitleg->push_back("Unfortunately, no traceback data can be retreived as a result. Sorry!");
		} else {
			for (auto& k : State(St)->Trace) {
				Uitleg->push_back(TrSPrintF("%s:Line %05d: %s", k.File.c_str(), k.Line, k.Func.c_str()));
			}
		}
		Crash(TrSPrintF("Lua Error: %s", Er), Uitleg);
		return 0; // Should never be executed, but hey, this way I can at least play safe, eh?
	}

#define InitAPIChat(abc) std::cout << "\x1b[94mScyndi API DEBUG> \x1b[0m"<<abc<<"\n"
//#define InitAPIChat(abc)
	static int SYS_InitAPI(lua_State* L) {
		InitAPIChat("Reading params");
		auto
			St{ luaL_checkstring(L,1) },
			Ap{ luaL_checkstring(L,2) };
		InitAPIChat("-> " << St << "; " << Ap);
		std::string
			ApS{ Ap },
			APU{ Upper(Ap) },
			//Scrippie{ TrSPrintF("-- Init: %s --\n\n%s, UC_%s = {}, {}\n",Ap,Ap,Ap) };
			Scrippie{ (String)"-- Init: " + Ap + " --\n\n" + Ap + ", UC_" + Ap + "= {},{}\n" };
		InitAPIChat("API Fault Check");
		if (!API.count(APU)) {
			Crash("API '" + APU + "' does not exist!");
			return 0;
		}
		InitAPIChat("Get State: " << St);
		auto _S{ State(St) };
		InitAPIChat("-> " << boolstring(_S != nullptr));
		for (auto k : API[APU]) {
			InitAPIChat("Registering: " << ApS << "::" << k.first << "::" << k.second);
			_S->Register("SCI_MOD_" + ApS + "_" + k.first, k.second);
			//Scrippie += TrSPrintF("%s.%s = SCI_MOD_%s_%s\n", Ap, k.first.c_str(), Ap, k.first.c_str());
			//Scrippie += TrSPrintF("UC_%s.%s = SCI_MOD_%s_%s\n", Ap, Upper(k.first).c_str(), Ap, k.first.c_str());
			Scrippie += (String)Ap + "." + k.first + " = SCI_MOD_" + Ap + "_" + k.first + "\n";
			Scrippie += (String)"UC_" + Ap + "." + k.first + " = SCI_MOD_" + Ap + "_" + k.first + "\n";
		}
		InitAPIChat("Go!\n" << Scrippie << "\n\x1b[96m*End\x1b[0m");
		_S->QDoString(Scrippie);
		return 0;
	}

	static int SYS_Use(lua_State* L) {
		auto
			St{ Lunatic_CheckString(L,1) },
			Md{ Lunatic_CheckString(L,2) };
		auto _State{ State(St) };
		if (Resource()->EntryExists(Md + ".lbc")) {
			QCol->Doing("Using", Md + ".lbc");
			auto buf{ Resource()->B(Md + ".lbc") };
			_State->QDoByteCode(buf, Md);
		} else if (Resource()->EntryExists(Md + ".lua")) {
			QCol->Doing("Using", Md + ".lua");
			auto src{ Resource()->GetString(Md + ".lua") };
			QCol->Doing("Compiling", Md + ".lua");
			_State->QDoString(src);
		} else {
			Crash("No module named '" + Md + "' has been found");
		}
	}

	int SYS_GoToFlow(lua_State* L) {
		auto St{ Lunatic_CheckString(L,1) };
		//if (!Prefixed(St, "FLOW_")) St = "FLOW_" + St;
		if (!HasFlow(St)) {
			//for (auto sr : StateRegister) QCol->Doing("DEBUG.STATE", sr.first); // debug only!
			Crash(TrSPrintF("GoToFlow(\"%s\"): That flow doesn't exist!", St.c_str())); return 0;
		}
		GoToFlow(St);
		return 0;
	}

	int SYS_LoadFlow(lua_State* L) {
		auto
			St{ luaL_checkstring(L,1) },
			Fl{ luaL_checkstring(L,2) };
		Flow(St, Fl);
		return 0;
	}

	int SYS_LoadNewFlow(lua_State* L) {
		// The difference with above is that this function will only load a flow if it hasn't been loaded before.
		auto
			St{ luaL_checkstring(L,1) },
			Fl{ luaL_checkstring(L,2) };
		if (!HasFlow(St)) Flow(St, Fl);
		return 0;
	}


	int SYS_LoadState(lua_State* L) {
		auto
			St{ luaL_checkstring(L,1) },
			Fl{ luaL_checkstring(L,2) };
		State(St, Fl);
		return 0;
	}

	int SYS_LoadNewState(lua_State* L) {
		// The difference with above is that this function will only load a flow if it hasn't been loaded before.
		auto
			St{ luaL_checkstring(L,1) },
			Fl{ luaL_checkstring(L,2) };
		if (!HasState(St)) State(St, Fl);
		return 0;
	}

	int SYS_HasState(lua_State* L) {
		lua_pushboolean(L, HasState(luaL_checkstring(L, 1)));
		return 1;
	}

	int SYS_BuildState(lua_State* L) {
		auto s = GameID_GINIE()->Value("BUILD", "TYPE");
		if (!s.size()) s = "debug";
		Lunatic_PushString(L, s);
		return 1;
	}

	static int SYS_DontFlip(lua_State* L) {
		DontFlip += std::max(1, (int)luaL_optinteger(L, 1, 1));
		return 1;
	}

	static map<string, string> CSayData  { {"ALLOW", "DEBUGONLY"} };
	static int SYS_CSaySetConfig(lua_State* L) {
		auto
			key{ Upper(Lunatic_CheckString(L, 1)) },
			val{ Lunatic_CheckString(L, 2) };
		CSayData[key] = val;
		return 0;
	}

	static int SYS_Platform(lua_State* L) {
		Lunatic_PushString(L, Platform(luaL_optinteger(L, 1, 1)));
		return 1;
	}

	static int SYS_CSay(lua_State* L) {
		auto
			St{ Lunatic_CheckString(L,1) },
			Msg{ Lunatic_CheckString(L,2) },
			Al{ Upper(CSayData["ALLOW"]) };
		auto
			Allow{ false };
		if (Al == "ALWAYS")
			Allow = true;
		else if (Al == "NEVER")
			Allow = false;
		else if (Al == "DEBUG" || Al == "DEBUGONLY")
			//Allow = State(St)->Debug;
			Allow = Upper(IDVal("Build", "Type")) == "DEBUG";
		else if (Al == "RELEASE" || Al == "RELEASEONLY" || Al == "NODEBUG")
			//Allow = !State(St)->Debug;
			Allow = Upper(IDVal("Build", "Type")) == "RELEASE";
		else
			Crash("Unknown CSay condition (" + Al + ")");
		if (Allow) {
			QCol->Doing("CSay", Msg);
			auto SMsg{ StReplace(Msg,"\"","\\\"") };
			SMsg = StReplace(SMsg, "\t", "\\t");
			SMsg = StReplace(SMsg, "\n", "\\n");
			SMsg = StReplace(SMsg, "\r", "\\r");
			if (CSayData["CBSTATE"] != "" && CSayData["CBFUNC"] != "") Call(CSayData["CBSTATE"], CSayData["CBFUNC"], "\"" + SMsg + "\"");
		}
		return 0;
	}

	static int SYS_Call(lua_State* L) {
		auto
			State{ Lunatic_CheckString(L,1) },
			Func{ Lunatic_CheckString(L,2) },
			Param{ (string)"" };

		for (int i = 3; i <= lua_gettop(L); i++) {
			if (i > 3) Param += ", ";
			switch (lua_type(L, i)) {
			case LUA_TSTRING: {
				string A{ "\"" };
				auto B{ Lunatic_CheckString(L, i) };
				for (int j = 0; j < B.size(); ++j) A += TrSPrintF("\\x%02x", B[j]);
				A += "\"";
				Param += A;
				// This may seem a bit over the top, but trust me, 
				// the more direct approach led me to many many crashes in the Apollo engine, 
				// and I wanted to make sure that was NOT gonna happen again!
			} break;
			case LUA_TNUMBER:
				Param += TrSPrintF("%f", luaL_checknumber(L, i));
				break;
			case LUA_TFUNCTION:
				char emsg[1000];
				sprintf_s(emsg, "Sys.Call(\"%s\",\"%s\",...): Call to other states cannot transfer functions!", State.c_str(), Func.c_str());
				luaL_argerror(L, i, emsg);
				break;
			case LUA_TNIL:
				Param += "nil";
				break;
			default:
				Crash(TrSPrintF("Unknown argument for interstate call: %d (argument #%d)", lua_type(L, i), i));
				break;
			}
		}
		if (!Param.size()) Param = "nil";
		Call(State, Func, Param);
		return 0;
	}


	static int SYS_OpenURL(lua_State* L) {
		OpenURL(luaL_checkstring(L, 1));
		return 0;
	}

	static int SYS_Yes(lua_State* L) {
		lua_pushboolean(L, Yes(StReplace(luaL_checkstring(L, 1), "<nl>", "\n")));
		return 1;
	}

	static int SYS_Annoy(lua_State* L) {
		Uint32 flags{ SDL_MESSAGEBOX_INFORMATION };
		auto Type{ Upper(Lunatic_OptString(L,3,"INFO")) };
		if (Type == "ERROR") flags = SDL_MESSAGEBOX_ERROR;
		if (Type == "WARNING") flags = SDL_MESSAGEBOX_WARNING;
		SDL_ShowSimpleMessageBox(flags, luaL_optstring(L, 2, GameTitle().c_str()), luaL_checkstring(L, 1), nullptr);
		return 0;
	}

	static int SYS_Sleep(lua_State* L) {
		SDL_Delay(luaL_checkinteger(L, 1));
		return 0;
	}

	static int SYS_ErrMsg(lua_State* L) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, luaL_optstring(L, 2, GameTitle().c_str()), luaL_checkstring(L, 1), nullptr);
		return 0;
	}

	static int SYS_CurrentFlow(lua_State* L) {
		Lunatic_PushString(L, CurrentFlow());
		return 1;
	}

	static int SYS_ATAN2(lua_State* L) { 
		auto
			a{ (double)luaL_checknumber(L,1) },
			b{ (double)luaL_checknumber(L,2) };
		lua_pushnumber(L,atan2(a, b));
		return 1;
	}

	static int SYS_GameID(lua_State* L) {
		Lunatic_PushString(L, IDVal(luaL_checkstring(L, 1), luaL_checkstring(L, 2)));
		return 1;
	}

	static int SYS_Ticks(lua_State* L) {
		lua_pushinteger(L,SDL_GetTicks());
		return 1;
	}
#pragma endregion

	static void InitScript() {
		static bool done{ false };
		if (done) return;
		InstallAllAPIS();
		QCol->Doing("Init", "Script engine");
		_Lunatic::Panick = PaniekLua;
		_Lunatic::Register4All({
			{"SCI_Exit",SYS_Exit},
			{"SCI_Crash",SYS_Crash},
			{"SCI_Use",SYS_Use},
			{"SCI_InitAPI",SYS_InitAPI},
			{"SCI_GoToFlow",SYS_GoToFlow},
			{"SCI_LoadFlow",SYS_LoadFlow},
			{"SCI_CurrentFlow",SYS_CurrentFlow},
			{"SCI_LoadNewFlow",SYS_LoadNewFlow},
			{"SCI_LoadState",SYS_LoadState},
			{"SCI_LoadNewState",SYS_LoadNewState},
			{"SCI_BuildState",SYS_BuildState},
			{"SCI_DontFlip",SYS_DontFlip},
			{"SCI_CSay",SYS_CSay},
			{"SCI_CSaySetConfig",SYS_CSaySetConfig},
			{"SCI_OpenURL",SYS_OpenURL},
			{"SCI_Call",SYS_Call},
			{"SCI_HasState",SYS_HasState},
			{"SCI_Yes",SYS_Yes},
			{"SCI_Annoy",SYS_Annoy},
			{"SCI_ErrorBox",SYS_ErrMsg},
			{"SCI_Sleep",SYS_Sleep},
			{"SCI_ATAN2",SYS_ATAN2},
			{"SCI_Traceback",DBG_ShowTraceback},
			{"SCI_Platform",SYS_Platform},
			{"SCI_GameID",SYS_GameID},
			{"SCI_Ticks",SYS_Ticks},

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
	bool _statecheck{ true };
	Slyvina::NSLunatic::SLunatic State(std::string _State) {
		Trans2Upper(_State);
		if (!StateRegister.count(_State)) { 
			//throw std::runtime_error(TrSPrintF("State '%s' doesn't exist", _State.c_str())); 
			if (_statecheck) {
				Crash(TrSPrintF("State '%s' doesn't exist", _State.c_str()));
			} else {
				QCol->Warn("A call has been done to non-existent state: " + _State);
			}
			return nullptr;
		}
		return StateRegister[_State];
	}

	void State(std::string _State, Slyvina::NSLunatic::SLunatic _Lun) {
		Trans2Upper(_State);
		if (!_Lun) {
			if (StateRegister.count(_State)) StateRegister.erase(_State);
			return;
		}
		StateRegister[_State] = _Lun;
	}

	void State(std::string _State, Slyvina::JCR6::JT_Dir _Res, std::string _Entry) {
		Trans2Upper(_State);
		if (!_Res->EntryExists(_Entry)) throw std::runtime_error(TrSPrintF("There is no script named '%s'",_Entry.c_str()));
		auto Buf{ _Res->B(_Entry) };
		static auto Core{ SRF()->GetString("Script/ScyndiCore.lua") };
		//StateRegister[_State] = LunaticByByteCode(Buf,_Entry);
		QCol->Doing("Loading", _Entry, "\t"); QCol->Doing("to state", _State);
		StateRegister[_State] = LunaticBySource(Core); 
		std::string initscript{
			"-- InitScript: " + _State + " --\n\n"
			"SCI_StateName = '"+_State+"'\n\n"
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
			"function Use(_module)\n"
			"\tSCI_Use('"+_State+"',_module)\n"
			"end\n\n"
			"function CSay(...)\n"
			"\tfor _,m in ipairs{...} do\n"
			"\t\tSCI_CSay(\""+_State+"\",m)\n"
			"\tend\n"
			"end\n"
			"function CSayF(fmt,...)\n"
			"\tCSay(string.format(fmt,...))\n"
			"end\n"
			"Scyndi.SetUseFunction(Use)\n"
			"Scyndi.SetUseCaseSensitive(false)\n"
		};
		//std::cout << initscript << std::endl; // debug!
		StateRegister[_State]->QDoString(initscript);
		auto ext = Upper(ExtractExt(_Entry));
		if (ext == "LUA") {
			QCol->Doing("Compiling", _Entry);
			auto src{ _Res->GetString(_Entry) };
			StateRegister[_State]->QDoString(src);
		} else if (ext == "LBC") {
			QCol->Doing("Obtaining", _Entry);			
			auto bnk{ _Res->B(_Entry) };
			if (Last()->Error) {
				Crash("Load State JCR6 Error!", "Error: " + Last()->ErrorMessage + "\nMain:" + Last()->MainFile + "\nEntry:" + Last()->Entry); 
				return;
			}
			StateRegister[_State]->QDoByteCode(bnk->Direct(), bnk->Size(), _Entry);
		}

	}

	void State(std::string _Stage, std::string _Entry) { State(_Stage, Resource(), _Entry); }

	void StateCheck(bool s) {
		_statecheck = s;
	}

	bool HasState(std::string _State) {
		return StateRegister.count(Upper(_State));
	}

	std::string CurrentFlow() {
		return CFlow.Name;
	}

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
			//src += TrSPrintF("local suc,err = pcall(Scyndi.Globals.%s,%s)\n", Func.c_str(), Para.c_str());
			src += "local suc,err = pcall(Scyndi.Globals." + Func + "," + Para + ")";
		//src += "print(suc,err)\n"; // debug
		src += "if not suc then SCI_Crash('" + _State + "',err) end";
		// std::cout << src << "\n"; // debug
		S->QDoString(src);
	}

	void InstallAPI(std::string _API, std::map<string, lua_CFunction> _Funcs) {
		API[Upper(_API)] = _Funcs;
	}

	void ScriptError(std::string _State, std::string ErrorMessage) {
		auto
			Uitleg{ NewVecString() };
		for (auto& k : State(_State)->Trace) {
			Uitleg->push_back(TrSPrintF("%s:Line %05d: %s", k.File.c_str(), k.Line, k.Func.c_str()));
		}
		Crash(ErrorMessage, Uitleg);
	}

	void KillAllStates() {
		for (auto& Victim : StateRegister) {
			QCol->Doing("Killing state", Victim.first);
			Victim.second->Kill();
		}
		StateRegister.clear();
	}

	void EndTheRun() { EndRun = true; }

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
				Cls();
				Poll();
				if (DebugConsoleCalled()) break;
				Call(CFlow.Name, "MainFlow");
				if (DontFlip)
					DontFlip--;
				else
					Flip();
				break;
			case RunType::Callback: {
				Cls();
				Call(CFlow.Name, "CB_Draw");
				if (DontFlip)
					DontFlip--;
				else
					Flip();
				Poll();
				if (DebugConsoleCalled()) break;
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
		QCol->Yellow("Sequence ended!\n");
	}

}
