// License:
// 
// Scyndi's Creative Interperter
// Debug Console
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
// Version: 24.10.28
// End License
// Lic:
// Scyndi's Creative Interperter
// Debug Console
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
#include <SlyvString.hpp>
#include <SlyvConInput.hpp>
#include <SlyvQCol.hpp>

#include <TQSG.hpp>
#include <TQSE.hpp>

#include "../SCI_Crash.hpp"
#include "../SCI_Script.hpp"
#include "../SCI_Config.hpp"
#include "../SCI_Graphics.hpp"
#include "../SCI_Kthura.hpp"
#include "API_DebugConsole.hpp"

using namespace Slyvina;
using namespace Units;
using namespace TQSG;
using namespace TQSE;

namespace Scyndi_CI {

	typedef void(*InternalCommand)(VecString);

	struct Debug_Command {
		std::string Flow{""};
		std::string CallFunction{""};
		InternalCommand IC{ nullptr };
	};
	std::map<std::string, Debug_Command> MCommands{};
	bool ConsoleRunning{ false };


#pragma region API
	static int DefCommand(lua_State* L) {
		auto
			Command{ Upper(luaL_checkstring(L,1)) },
			Flow{ Upper(luaL_checkstring(L,2)) },
			CallFunction{ Upper(luaL_checkstring(L,3)) };
		if (!DebugBuild()) return 0; // No debug! No console!
		if (MCommands.count(Command) && MCommands[Command].IC) {
			Crash("Debug internal command override");
			return 0;
		}
		MCommands[Command] = { Flow,CallFunction,nullptr };
		return 0;
	}

	static int EndConsole(lua_State* L) { ConsoleRunning = false; return 0; }
#pragma endregion

#pragma region "Internal Commands"
	void IC_Bye(VecString) { EndTheRun(); ConsoleRunning = false; }
	void IC_Back(VecString) { printf("System returned to the game. Please click the game window to continue\n"); ConsoleRunning = false; }
	void IC_Fuck(VecString) { QCol->Error("HEY! What kind of talk is that?"); }
	void IC_Echo(VecString L) { for (auto A : *L) printf("=> %s\n", A.c_str()); }

	void IC_Block(VecString) {
		using namespace std;
		if (!MapPicked()) { QCol->Error("Request not possible. No map picked"); }
		if (!LayerPicked()) { QCol->Error("Request not possible. No layer picked"); }
		QCol->Reset();
		cout << "Blockmap: " << PickedMap() << "; Layer:" << PickedLayer() << endl;
		auto L{ GetKthuraLayer() };		
		printf("Blockmap: %dx%d\n", L->BlockWidth(), L->BlockHeight());
		for (auto y = 0; y < L->BlockHeight(); ++y) {
			for (auto x = 0; x < L->BlockWidth(); ++x) {
				if (L->Block(x, y)) QCol->Yellow("X"); else QCol->Dark(".");
			} cout << endl;
		}

	}

	void IC_CountObjects(VecString) {
		if (!MapPicked()) { QCol->Error("Request not possible. No map picked"); }
		if (!LayerPicked()) { QCol->Error("Request not possible. No layer picked"); }
		std::map<String, size_t> Cnt;
		size_t total = 0;
		QCol->Doing("Layer", PickedLayer());
		for (auto o = GetKthuraLayer()->FirstObject(); o; o = o->Next()) {
			if (!Cnt.count(o->SKind())) Cnt[o->SKind()] = 0;
			Cnt[o->SKind()]++;
			total++;
		}
		for (auto result : Cnt) {
			QCol->Doing(result.first, TrSPrintF("%9d", result.second));
		}
		QCol->Doing("Total", TrSPrintF("%9d", total));
	}
#pragma endregion




	void Init_API_DebugConsole(){
		std::map<std::string, lua_CFunction>IAPI{
			{"DefDebugConsole",DefCommand},
			{"EndConsole",EndConsole}
		};
		MCommands["BYE"] = { "DEBUG_GLOBAL","",IC_Bye };
		MCommands["EXIT"] = { "DEBUG_GLOBAL","",IC_Bye };
		MCommands["BACK"] = { "DEBUG_GLOBAL","",IC_Back };
		MCommands["RETURN"] = { "DEBUG_GLOBAL","",IC_Back };
		MCommands["BLOCKMAP"] = { "DEBUG_GLOBAL","",IC_Block };
		MCommands["COUNTOBJECTS"] = { "DEBUG_GLOBAL","",IC_CountObjects };
		// Test commands
		MCommands["FUCK"] = { "DEBUG_GLOBAL","",IC_Fuck };
		MCommands["SHIT"] = { "DEBUG_GLOBAL","",IC_Fuck };
		MCommands["ECHO"] = { "DEBUG_GLOBAL","",IC_Echo };

		InstallAPI("DebugConsole", IAPI);
	}


	bool Scyndi_CI::DebugConsoleCalled() {
		static TImage CheckConsole{nullptr};
		if (DebugBuild() && KeyDown(SDLK_LSHIFT) && KeyDown(SDLK_RSHIFT) && KeyDown(SDLK_KP_MINUS)) {
			Flush();
			if (!CheckConsole) {
				Img("*CHECKCONSOLE", SRF(), "GFX/CheckConsole.png");
				CheckConsole = Img("*CHECKCONSOLE");
				//CheckConsole->Hot(0, CheckConsole->Height() / 2);
			}
			Cls();
			SetColor(255, 255, 255, 255);
			CheckConsole->StretchDraw(0, ScreenHeight() / 2,ScreenWidth(),(int)floor(((double)ScreenWidth()/CheckConsole->Width())*CheckConsole->Height()));
			Flip();
			Flush(); // I really need to be sure of this one!
			ConsoleRunning = true;
			do {
				QCol->Yellow("SCI DEBUG> ");
				QCol->LCyan("");
				auto RawCommand = Trim(ReadLine());
				QCol->Reset();
				if (RawCommand.size()) {
					String Cmd{};
					VecString Params;
					auto P = FindFirst(RawCommand, ' ');
					if (P < 0) {
						Cmd = Upper(RawCommand);
						Params = NewVecString();
					} else {
						Params = NewVecString();
						Cmd = Upper(RawCommand.substr(0, P));
						auto RawParams{ RawCommand.substr(P + 1) };
						String Word;
						bool quote{ false };
						for (size_t i = 0; i < RawParams.size(); ++i) {
							switch (RawParams[i]) {
							case ' ':
								if (!quote) {
									if (Word.size()) Params->push_back(Word);
									Word = "";
								} else Word += " ";
								break;
							case '"':
								if (Word.size()) Params->push_back(Word);
								Word = "";
								quote = !quote;
								break;
							default:
								Word += RawParams[i];
							}
						}
						if (Word.size()) Params->push_back(Word);
					}
					if (!MCommands.count(Cmd)) {
						QCol->Error("Command \"" + Cmd + "\" has not been understood");
					} else if (MCommands[Cmd].Flow == "DEBUG_GLOBAL" || MCommands[Cmd].Flow == CurrentFlow()) {
						if (MCommands[Cmd].IC) 
							MCommands[Cmd].IC(Params);
						else {
							std::string P{ "" };
							for (auto PI : *Params) {
								if (P.size()) P += ", ";
								P += TrSPrintF("\"%s\"", PI.c_str());
							}
							Call(MCommands[Cmd].Flow, MCommands[Cmd].CallFunction, P);
						} 
					} else {
						QCol->Error("Command \"" + Cmd + "\" is not for this flow, but for "+ MCommands[Cmd].Flow+" in stead.");
					}
				}
			} while (ConsoleRunning);
			return true;
		}
		return false;
	}
}
