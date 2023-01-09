// Lic:
// Scyndi's Creative Interpreter - Builder
// Project Manager
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

#include <SlyvQCol.hpp>
#include <SlyvAsk.hpp>
#include <SlyvTime.hpp>

#include "SCI_Project.hpp"

using namespace Slyvina;
using namespace Units;


namespace Scyndi_CI {
	namespace Builder {
		uint64 SCI_Project::Processed{ 0 };
		uint64 SCI_Project::Success{ 0 };
		uint64 SCI_Project::Fail{ 0 };

		bool SCI_Project::CreateIfNeeded() {
			auto PPath(ExtractDir(Project));
			if (FileExists(Project)) return true;
			if (!QuickYes(TrSPrintF("The project '%s' does not appear to exist! Create it", Project.c_str()))) return false;
			if (PPath.size()) {
				if ((!DirectoryExists(PPath)) && (!QuickYes(TrSPrintF("May I create the directory '%s'", PPath.c_str())))) return false;
				QCol->Doing("Creating dir", PPath);
				MakeDir(PPath);
			}
			QCol->Doing("Creating Project", Project);
			SaveString(Project,"[Project]\nCreated=" + CurrentDate() + "; " + CurrentTime() + "\n");		
		}

		std::string SCI_Project::Title() { return Ask(Data, "Project", "Title","What is the title of the project?"); }
		std::string SCI_Project::Author() { return Ask(Data, "Project", "Author", "What is the name of the author?"); }
		std::string SCI_Project::Copyright() { return Ask(Data, "Project", "Copyright", "Copyright: ", TrSPrintF("(c) %s, %s", Author().c_str(), Right(CurrentDate(), 4).c_str())); }

		Scyndi_CI::Builder::SCI_Project::SCI_Project(std::string _Project) {
			Project = _Project;
			QCol->Doing("Project", _Project);
			Processed++;
			if (!CreateIfNeeded()) { Fail++; return; }
			Data = LoadGINIE(Project, Project);
			Data->AutoSaveHeader += Title() + "\n" + Author() + "\n" + "\n" + Copyright();
			QCol->Doing("Title", Title());
			QCol->Doing("Author",Author());
			QCol->Doing("Copyright", Copyright());
		}

	}
}