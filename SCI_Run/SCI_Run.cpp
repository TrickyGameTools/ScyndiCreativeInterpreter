// Lic:
// Scyndi's Creative Interpreter - Runtime
// Start up
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
// Version: 24.03.16
// EndLic

#undef SCI_NO_ERROR_CATCHING

#include <SDL.h>

#include <TQSE.hpp>

#include <SlyvArgParse.hpp>
#include <SlyvStream.hpp>
#include <SlyvQCol.hpp>
#include <SlyvTime.hpp>
#include <SlyvBank.hpp>
#include <SlyvWindows.hpp>

#include "../SCI_Share/SCI_Header.hpp"
#include "SCI_Config.hpp"
#include "SCI_Graphics.hpp"
#include "SCI_Load.hpp"
#include "SCI_Script.hpp"
#include "SCI_Crash.hpp"




using namespace Slyvina;
using namespace Units;
using namespace TQSE;
using namespace Scyndi_CI;

namespace Scyndi_CI {
	bool CanStart(int c, char** cliargs) {
		FlagConfig Cfg;
		Args = ParseArg(c, cliargs, Cfg);
		QCol->Doing("SRF build", SRF_Date());
		QCol->Doing("Resource", JCR_MainFile());
		auto J{ JCR_MainFile() };
		if (!FileExists(J)) {
			QCol->Error("File '" + J + "' has not been found");
			return false;
		}
		auto
			Tit{ GameTitle() },
			Author{ GameAuthor() },
			ByAuthor{ "By: " + Author },
			Copy{ GameCopyright() };
		

		auto
			TP{ std::max((uint64)0,(uint64)40 - (Tit.size() / 2)) },
			TA{ std::max((uint64)0,(uint64)40 - (ByAuthor.size() / 2)) },
			TC{ std::max((uint64)0,(uint64)40 - (Copy.size() / 2)) };

		QCol->White("\n\n\n" + Repeat(" ", TP) + Tit+"\n");
		QCol->Yellow(Repeat(" ",TA) + "By: "); QCol->LCyan(Author + "\n");
		QCol->LMagenta(Repeat(" ", TC) + Copy + "\n\n\n\n");

		if (GameID_GINIE()->Value("ENGINE", "ENGINE") != "SCI") {
			QCol->Error("This game is NOT made for Scyndi's Creative Interpreter");
			return false;
		}
		if (!VersionMatch()) return false;

		return true;
	}
}

void SCI_BankPanic(std::string err) { Crash("Banking error:"+err); }

int main(int c, char** cli_args) {
	using namespace Scyndi_CI;	
	Header("Scyndi's Creative Interpreter");
	QCol->Doing("Run File", ChReplace(cli_args[0],'\\','/'));
	QCol->Doing("Run from", CurrentDir());
	QCol->Doing("Game Home", GameGlobalHome());
	_Bank::DefineDefaultBankPanic(SCI_BankPanic);
	if (CanStart(c, cli_args)) {
#ifndef SCI_NO_ERROR_CATCHING
		try {
#endif
			StartGraphics();
			auto StartTime{ TimeStamp() };
			LoadAllStartStuff();			
			Flow("MAIN", MainScript());
			do { Poll(); } while (abs(TimeStamp() - StartTime) < 3);
			GoToFlow("MAIN");
#ifdef SlyvWindows
			QCol->Doing("Releasing", "Console");
			if (NoConsole()) ReleaseConsole();
#endif
			RunGame();
#ifndef SCI_NO_ERROR_CATCHING
		} catch (runtime_error rte) {
			if (!Crashed()) {
				Crash(std::string(rte.what()), "", false);
			} else {
				QCol->Error(rte.what());
			}
		}
#endif
	} else { return 1; }
	StateCheck(false); // Force State Checks off.
	KillAllStates();
	QCol->Doing("Ending", "Scyndi's Creative Interpreter");
	QCol->Reset();
	return 0;	
}