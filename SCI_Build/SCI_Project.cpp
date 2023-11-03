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
// Version: 23.11.03
// EndLic

#include <SlyvQCol.hpp>
#include <SlyvAsk.hpp>
#include <SlyvTime.hpp>
#include <SlyvVolumes.hpp>
#include <SlyvMD5.hpp>

#include "../SCI_Share/Version.hpp"

#include "SCI_Build_Config.hpp"
#include "SCI_Project.hpp"
#include "SCI_Script.hpp"

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

		bool SCI_Project::WindowSettings() {
			if (DebugFlag()) 
				ID->Value("BUILD", "TYPE", "debug"); 
			else
				ID->Value("BUILD", "TYPE", "release");
			ID->Value("Engine", "Engine", "SCI");
			ID->Value("Engine", "Version", QVersion.Version());
			ID->Value("Window", "Caption", Ask(Data, "SCI::WINDOW", "Caption", "Window - Caption: ", Title()));
			auto Full{ Yes(Data, "SCI::WINDOW","Full Screen","Do you want to game to run full screen?") };
			ID->Value("Window", "Full", boolstring(Full));
			if (!Full) {
				ID->Value("Window", "Width", Ask(Data, "SCI::Window", "WinWidth", "Window width (suffix with % to make it calculate on the desktop size)", "1200"));
				ID->Value("WIndow", "Height", Ask(Data, "SCI::Window", "WinHeight", "Window height (suffix with % to make it calculate on the desktop size)", "1000"));
			}
			auto Alt{ Yes(Data,"SCI::Window","UseAlt","Use the Alt Height system") };
			ID->Value("Alt", "Use", boolstring(Alt));
			if (Alt) {
				ID->Value("Alt", "Width",Ask(Data, "SCI::Window", "AltWidth", "Alt width", "1200"));
				ID->Value("Alt", "Height",Ask(Data, "SCI::Window", "AltHeight", "Alt Height", "1000"));				
			}
			ID->Value("Mouse", "Hide", boolstring(Yes(Data, "SCI::Mouse", "Hide", "Do you want to hide the system's mouse pointer")));
			ID->Value("Save", "Dir", Ask(Data,"SCI::Save", "Dir", "Savegame dir:"));
			if (!Data->HasValue("SCI::Save", "ID")) {
				Data->Value("SCI::Save", "ID", md5(TrSPrintF("Scyndi %d %s", TimeStamp(), Data->Value("SCI::Save", "Dir").c_str())) + "-" + md5(CurrentDate()));
			}
			ID->Value("Save", "ID", Data->Value("SCI::Save", "ID"));
			return true;
		}

		std::string SCI_Project::Title() { return Ask(Data, "Project", "Title","What is the title of the project?"); }
		std::string SCI_Project::Author() { return Ask(Data, "Project", "Author", "What is the name of the author?"); }
		std::string SCI_Project::Copyright() { return Ask(Data, "Project", "Copyright", "Copyright: ", TrSPrintF("(c) %s, %s", Author().c_str(), Right(CurrentDate(), 4).c_str())); }
		std::string SCI_Project::License() { return Ask(Data, "Project", "License", "License", "General Public License 3"); }

		std::string SCI_Project::File() { return Project; }

		std::string SCI_Project::OutputName() {
			if (!Data->HasValue("Project", "Outputname")) {
				QCol->LMagenta("Output Name\n");
				QCol->White("This name will be used to name the .exe file, and the main resource file.\n\n");				
			}
			return Ask(Data, "Project", "OutputName", "Please enter the output name: ", Title());			
		}

		std::string SCI_Project::DebugJQLFile() {
			auto dir = Ask(Data, "Project", "DebugJQLDir", "For the debug JQL file, I need a directory. Please name it: ", ChReplace(ExtractDir(File()),'\\','/'));
			dir = ChReplace(dir,'\\', '/');
			if (!Suffixed(dir, "/")) dir += "/";
			return AVolPath(dir + OutputName() + ".jql");
		}

		std::string SCI_Project::AssetsDir() {
			return AVolPath(Ask(Data, "Directory", "SCI_Assets_Source", "Please tell me where the assets to include in this project are stored: "));
		}

		bool SCI_Project::AssetsMultiDir() {
			return Yes(Data,"Directory","SCI_ASSETS_MULTIDIR","Is the assets dir set up for 'multi-dir'");
		}

		bool SCI_Project::UseMedals() {
			return Upper(Data->Value("MEDALS", "USE")) == "YES";
		}

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
			ID->Value("ID", "Title", Title());
			ID->Value("ID", "Author", Author());
			ID->Value("ID", "Copyright", Copyright());
			WindowSettings();
			if (!CompileScripts(this, Data)) { Fail++; return; }
			if (!HandlePackage(this, Data)) { Fail++; return; }
		}

	}
}