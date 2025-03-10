// License:
// 
// Scyndi's Creative Interpreter - Builder
// Script Manager
// 
// 
// 
// 	(c) Jeroen P. Broks, 2023, 2024, 2025
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
// Version: 25.03.04
// End License


#include <SlyvStream.hpp>
#include <SlyvAsk.hpp>
#include "SCI_Build_Config.hpp"
#include "SCI_Script.hpp"
using namespace Slyvina;

using namespace Slyvina::Units;

namespace Scyndi_CI {
	namespace Builder {
		std::string Scyndi() {
			//const char* DefaultScyndi = "Scyndi.exe";
			const std::string DefaultScyndi{ TrSPrintF("%s/%s",ExtractDir(CLI_Args.myexe).c_str(),"Scyndi.exe") };
			auto ret{ GlobalConfig()->NewValue("Builder","Scyndi",DefaultScyndi) };
			while (!FileExists(ret)) {
				GlobalConfig()->Value("Builder", "Scyndi", "");
				ret = Ask(GlobalConfig(), "Builder", "Scyndi", TrSPrintF("I could not locate '%s'. Please tell me where to find it: ", ret.c_str()), DefaultScyndi);
			}
			return ret;
		}

#ifdef SlyvLinux
union CEX { int i; char c[3]; };
#endif // SlyvLinux

		bool CompileScripts(SCI_Project* Prj, GINIE PrjDat) {
			QCol->Doing("Compiling", "Scripts");
			PrjDat->NewValue("AA_META", "01_TITLE", Prj->Title());
			PrjDat->NewValue("AA_META", "02_CREATEDBY", Prj->Author());
			PrjDat->NewValue("AA_META", "03_COPYRIGHT", Prj->Copyright());
			PrjDat->NewValue("AA_META", "04_LICENSE", Prj->License());
			//QCol->Dark(Platform() + "\n"); // debug
			AskList(PrjDat, "Directory::" + Platform(), "SourceFiles", "Name your directories to search for script source files:");
			AskList(PrjDat, "Directory::" + Platform(), "Libraries", "Name your directories to search for libraries:");
			PrjDat->AddNew("Directory::" + Platform(), "Libraries", ExtractDir(ChReplace(CLI_Args.myexe, '\\', '/')) + "/IntLibs");
			auto cmd{ Scyndi() }; cmd += " ";
			if (CLI_Args.bool_flags["scyndidebug"]) cmd += " -dbg ";
			if (CLI_Args.bool_flags["scyndiforce"]) cmd += " -force ";
			cmd += Prj->File();
			QCol->Doing("==>", cmd);
			QCol->Reset(); std::cout << "\n\n\n";
			#ifdef SlyvLinux
			// Avoid the Linux 256fold bug. Issue in Linux itself, nothing I can do about it but "cheat".
			CEX _CEX;
			_CEX.i=system(cmd.c_str());
			int excode{_CEX.c[1]};
			#else
			auto excode{ system(cmd.c_str()) };
			#endif // SlyvLinux
			if (excode == 1)
				QCol->Error("Scyndi did apparently encounter an error during the compile session");
			else if (excode < 0)
				QCol->Error("It appears Scyndi crashed during the compilation session");
			else if (excode)
				QCol->Error(TrSPrintF("Scyndi did apparently encounter %d errors during the compile session", excode));
			else
				QCol->Green("Scyndi did not report any errors\n\n");
			return excode==0;
		}
	}
}
