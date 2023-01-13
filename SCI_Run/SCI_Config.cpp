// Lic:
// Scyndi's Creative Interpreter
// Configuration
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

#include <TQSE.hpp>

#include <SlyvArgParse.hpp>
#include <SlyvString.hpp>
#include <SlyvQCol.hpp>
#include <SlyvGINIE.hpp>

#include <JCR6_Core.hpp>

#include "../SCI_Share/Version.hpp"
#include "../SCI_Share/SCI_GlobalConfig.hpp"

#include "SCI_Config.hpp"
#include "SCI_JCR.hpp"

using namespace Slyvina;
using namespace Units;
using namespace JCR6;
using namespace TQSE;

namespace Scyndi_CI {
	ParsedArg Args{};

	static GINIE srf_id{ nullptr };
	static GINIE res_id{ nullptr };
	
	std::string JCR_MainFile() {
		if (Args.arguments.size()) return ChReplace(Args.arguments[0],'\\','/');
		return ChReplace(StripExt(Args.myexe) + ".jcr", '\\', '/');
	}

	std::string GameGlobalHome() {
		auto ret = GlobalConfig()->Value("Runtime", "Home");
		if (!DirectoryExists(ret)) {
			QCol->Doing("Creating", ret);
			MakeDir(ret);
		}
		return ret;
	}

#pragma region SRF

	std::string JCR_SRF() {	return ChReplace(StripExt(Args.myexe) + ".srf",'\\','/'); }

	static void LoadSRFGINIE() {
		if (!srf_id) {
			auto src = SRF()->GetString("ID/Identify.ini");
			if (Last()->Error) { QCol->Error("JCR Error in getting SRF identification: " + Last()->ErrorMessage); exit(1); }
			QCol->Doing("Parsing", "SRF Identification");
			srf_id = ParseGINIE(src); 

		}		
	}

	std::string SRF_Date() {
		LoadSRFGINIE();
		return srf_id->Value("SRF", "Build");
	}

	
#pragma endregion

#pragma region "Game ID"
	static void LoadGameID() {
		if (!res_id) {
			QCol->Doing("Loading", "Game identification data");
			auto s = Resource()->GetString("ID/Identify.ini");
			if (Last()->Error) {
				QCol->Error("JCR6 error: " + Last()->ErrorMessage);
				QCol->Doing("==> Main", Last()->MainFile);
				QCol->Doing("==> Entry", Last()->Entry);
				exit(1234);
			}
			if (!s.size()) {
				QCol->Error("For some reason there's no ID data!");
				exit(4321);
			}
			res_id = ParseGINIE(s);
		}
	}

	std::string GameTitle() {
		LoadGameID();
		return res_id->Value("ID", "Title");
	}

	std::string GameCopyright() {
		LoadGameID();
		return res_id->Value("ID", "Copyright");
	}

	GINIE GameID() {
		return GINIE();
	}

	bool VersionMatch(bool ask) {
		LoadGameID();
		try {
			auto ret{ false };
			auto s{ Split(res_id->Value("Engine","Version"),'.') };
			auto
				gt{ stol((*s)[0]) },
				mj{ stol((*s)[1]) },
				mn{ stol((*s)[2]) };
			if (gt != QVersion.giant || mj != QVersion.major || mn != QVersion.minor) {
				if (!ask) return false;
				if (gt > QVersion.giant) { throw runtime_error(TrSPrintF("This game requires at least version %d.0.0 in order to run", gt)); }
				if (gt == QVersion.giant && (mj > QVersion.major || (mj == QVersion.minor && mn > QVersion.minor))) {
					return Yes(TrSPrintF("This game requires version %d.%d.%d of Scyndi's Creative Interpreter, which is a slightly later version than you are using now. Are you sure you wish to continue?"));
				}				
				return Yes("This version of Scyndi's Creative Interpreter appears to be of a later version than the game wants.\n\nThis means it's possible this game uses deprecated or even removed portions of the engine.\n\nAre you sure you wish to continue?");
			}
			return true;
		} catch (runtime_error rte) {
			QCol->Error(rte.what());
			exit(50);
		}
		return false;
	}

	RunType GetRunType() {
		static RunType Ret{ RunType::Unknown };
		if (Ret == RunType::Unknown) {
			auto SRT = Upper(res_id->Value("Run", "Type"));
			if (SRT == "CALLBACK") Ret = RunType::Callback;
			if (SRT == "STRAIGHT") Ret = RunType::Straight;
			if (SRT == "" || SRT == "FLOW") Ret = RunType::Flow;
			throw runtime_error("Unknown RunType");
		}
		return Ret;
	}

	std::string GameAuthor() {
		LoadGameID();
		return res_id->Value("ID", "Author");
	}

	GINIE GameID_GINIE() {
		LoadGameID();
		return res_id;
	}
#pragma endregion

}