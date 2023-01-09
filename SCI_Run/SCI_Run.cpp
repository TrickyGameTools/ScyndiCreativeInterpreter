// Lic:
// Scyndi's Creative Interpreter - Runtime
// Start up
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
// Version: 23.01.09
// EndLic

#include <SlyvArgParse.hpp>
#include <SlyvStream.hpp>
#include <SlyvQCol.hpp>

#include "../SCI_Share/SCI_Header.hpp"
#include "SCI_Config.hpp"

using namespace Slyvina::Units;
namespace Scyndi_CI {
	bool CanStart(int c, char** cliargs) {
		FlagConfig Cfg;
		Args = ParseArg(c, cliargs, Cfg);
		QCol->Doing("Resource", JCR_MainFile());
		auto J{ JCR_MainFile() };
		if (!FileExists(J)) {
			QCol->Error("File '" + J + "' has not been found");
			return false;
		}
		return true;
	}
}

int main(int c, char** cli_args) {
	using namespace Scyndi_CI;	
	Header("Scyndi's Creative Interpreter");
	QCol->Doing("Run File", ChReplace(cli_args[0],'\\','/'));
	QCol->Doing("Run from", CurrentDir());
	QCol->Doing("Game Home", GameGlobalHome());
	if (CanStart(c, cli_args)) {

	} else { return 1; }
	return 0;
	
}