// Lic:
// Scyndi's Creative Interpreter - Builder
// Main
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
#include <SlyvArgParse.hpp>
#include <SlyvString.hpp>
#include <JCR6_JQL.hpp>
#include "../SCI_Share/SCI_Header.hpp"
#include "SCI_Build_Config.hpp"
#include "SCI_Project.hpp"

using namespace Scyndi_CI;
using namespace Scyndi_CI::Builder;
using namespace Slyvina::Units;

int main(int ac, char** args) {
	Header("Scyndi's Creative Interpreter - Builder");
	FlagConfig Cfg;
	AddFlag_Bool(Cfg, "debug", false);
	AddFlag_Bool(Cfg, "scyndidebug", false); // Use debug builds of Scyndi compilations
	AddFlag_Bool(Cfg, "scyndiforce", false);	
	CLI_Args = ParseArg(ac, args, Cfg);
	if (!CLI_Args.arguments.size()) {
		QCol->Magenta("Usage: ");
		QCol->Yellow(StripAll(args[0]));
		QCol->Green(" [<switches>] ");
		QCol->Cyan("<Project>\n\n");
		QCol->Magenta("Available switches:\n");
		QCol->White("\t-debug       "); QCol->Green("Create a debug build\n");
		QCol->White("\t-scyndidebug "); QCol->Green("Use the debug compilations of Scyndi scripts\n");
		QCol->White("\t-scyndiforce "); QCol->Green("Forces Scyndi to compile ALL required scripts regardless if they were modified or not\n");
		QCol->Reset();
		return 0;
	}
	Slyvina::JCR6::InitJQL();
	for (auto prj : CLI_Args.arguments) {
		auto LozeTroep{ SCI_Project(prj) };
	}
	QCol->OnlyVal("Processed", SCI_Project::Processed);
	QCol->OnlyVal("Success", SCI_Project::Success);
	QCol->OnlyVal("Failed", SCI_Project::Fail);
	QCol->Cyan("Ok\n");
	QCol->Reset();
	return SCI_Project::Fail;
}