// License:
// 
// Scyndi's Creative Interpreter
// API header creator
// 
// 
// 
// 	(c) Jeroen P. Broks, 2023(C#), 2024(C#), 2026
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
// Version: 26.03.08
// End License



//using System.Text;
//using TrickyUnits;

#include <SlyvDir.hpp>
#include <SlyvQCol.hpp>
#include <SlyvDirry.hpp>
#include <SlyvString.hpp>
#include <SlyvStream.hpp>

using namespace Slyvina;
using namespace Slyvina::Units;

// This is only a quick program which will make sure a proper header file containing all APIs is present.
int main() {
try {
	//QCol->Doing("Running from", Directory.GetCurrentDirectory());
	const std::string APIDir = Dirry("$Home$/dScyndi/Projects/Applications/Slyvina/Apps/SCI/SCI_Run/API");
	auto APIS = FileList(APIDir); if (APIS == nullptr) throw std::runtime_error("I could not get the contents of \""+APIDir+"\"!");
	std::string Headers = "";
	std::string InitFunc = "\tinline void InstallAllAPIS() {\n";

	for (auto& APISource : *APIS) {
		if (Lower(ExtractExt(APISource)) == "cpp") {
			QCol->Doing("Adding", APISource);
			auto APIName = StripAll(APISource);
			Headers +="\tvoid Init_"+APIName+"();\n";
			InitFunc+="\t\tInit_"+APIName+"();\n";
		} else {
			QCol->Doing("Skipping", APISource);
		}
	}


	InitFunc+"\t\n";
	//const std::string AO = "{", AS = "}";
	std::string Output = "namespace Scyndi_CI{\n\n"+Headers+"\n\n"+InitFunc+"\n\t}\n\n}\n";
	QCol->Green(Output+"\n");
	//QuickStream.SaveString($"{APIDir}/SCI_InstallAPIs.hpp", Output);
	SaveString(APIDir+"/SCI_InstallAPIs.hpp",Output);


} catch (std::runtime_error E) {
	QCol->Error(E.what());
	QCol->Reset();
	return 1;
}
QCol->Reset();



return 0;
}
