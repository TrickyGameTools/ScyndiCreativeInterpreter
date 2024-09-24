// Lic:
// Scyndi's Creative Interpreter - Builder
// Update itch.io updates with Butler
// 
// 
// 
// (c) Jeroen P. Broks, 2024
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



/*

	This code will only be used in release builds and can immediately update the itch.io downloads.
	It uses the tool Butler for this and in the global config it should therefore be configured where this tool can be found.

*/

#include <SlyvAsk.hpp>

#include "SCI_Butler.hpp"
#include "SCI_Build_Config.hpp"
#include "../SCI_Share/SCI_GlobalConfig.hpp"

using namespace Slyvina;
using namespace Units;


namespace Scyndi_CI {
	namespace Builder {

		static std::string Butler() {
			//const char* DefaultButler = "Butler.exe";
			const std::string DefaultButler{ TrSPrintF("%s/%s", ExtractDir(CLI_Args.myexe).c_str(), "Butler.exe") };
			auto ret{ GlobalConfig()->NewValue("Builder","Butler",DefaultButler) };
			while (!FileExists(ret)) {
				GlobalConfig()->Value("Builder", "Butler", "");
				ret = Ask(GlobalConfig(), "Builder", "Butler", TrSPrintF("I could not locate '%s'. Please tell me where to find it: ", ret.c_str()), DefaultButler);
			}
			return ret;
		}
		

		void Butler(SCI_Project* P) {
			// General
			if (!P->Yes("Butler", "Use", "Does this project have an itch.io I should automatically update")) return;
			auto devname{ P->Vraag("Butler","Dev","Itch.io developer's name: ") };
			auto prjname{ P->Vraag("Butler","Prj","Itch.io project name: ") };
			auto dlfname{ P->Vraag("Butler","DLF","Itch.io download name: ") };
			

			// Windows
			auto cmd{ Butler() };
			cmd += " push ";
			cmd += "\"" + P->ReleaseDirectory() + "\" ";
			cmd += devname + "/" + prjname + ":" + dlfname + "_win64";
			QCol->Doing("==>", cmd);
			QCol->Reset(); std::cout << "\n\n\n";
			auto excode{ system(cmd.c_str()) };
			if (excode == 1)
				QCol->Error(TrSPrintF("\7Butler did apparently encounter an error (%d) during the compile session", excode));
			else
				QCol->Green("Butler did not report any errors\n\n");			


			// Mac
			// I will only implement this if Apple's policy on free software development changes.


		}
	}
}