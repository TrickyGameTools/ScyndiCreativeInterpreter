// License:
// 
// Scyndi's Creative Interpreter - Builder
// Project Manager
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
// Version: 25.01.29
// End License

#include <SlyvQCol.hpp>
#include <SlyvAsk.hpp>
#include <SlyvTime.hpp>
#include <SlyvVolumes.hpp>
#include <SlyvMD5.hpp>
#include <SlyvDir.hpp>
#include <SlyvStream.hpp>
#include <SlyvDir.hpp>

#include "../SCI_Share/Version.hpp"

#include "SCI_Build_Config.hpp"
#include "SCI_Project.hpp"
#include "SCI_Script.hpp"
#include "SCI_Butler.hpp"

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
			return true;
		}

		bool SCI_Project::WindowSettings() {
			if (DebugFlag())
				ID->Value("BUILD", "TYPE", "debug");
			else {
				ID->Value("BUILD", "TYPE", "release");
				if (Yes("RELEASE", "WINDOWS_CONSOLE_RELEASE", "Release from console in Windows"))
					ID->Value("PLATFORM::WINDOWS", "CONSOLE", "RELEASE");
			}
			ID->Value("Engine", "Engine", "SCI");
			ID->Value("Engine", "Version", QVersion.Version());
			ID->Value("Window", "Caption", Ask(Data, "SCI::WINDOW", "Caption", "Window - Caption: ", Title()));
			auto Full{ Units::Yes(Data, "SCI::WINDOW","Full Screen","Do you want to game to run full screen?") };
			ID->Value("Window", "Full", boolstring(Full));
			if (!Full) {
				ID->Value("Window", "Width", Ask(Data, "SCI::Window", "WinWidth", "Window width (suffix with % to make it calculate on the desktop size)", "1200"));
				ID->Value("WIndow", "Height", Ask(Data, "SCI::Window", "WinHeight", "Window height (suffix with % to make it calculate on the desktop size)", "1000"));
			}
			auto Alt{ Units::Yes(Data,"SCI::Window","UseAlt","Use the Alt Height system") };
			ID->Value("Alt", "Use", boolstring(Alt));
			if (Alt) {
				ID->Value("Alt", "Width",Ask(Data, "SCI::Window", "AltWidth", "Alt width", "1200"));
				ID->Value("Alt", "Height",Ask(Data, "SCI::Window", "AltHeight", "Alt Height", "1000"));
				if (Units::Yes(Data, "SCI::Window", "WinAdeptAlt", "Automatically adept Window Size to keep Alt Screen Ratios correct")) ID->Value("Window", "WinAdeptAlt", "Y");
			}
			ID->Value("Mouse", "Hide", boolstring(Units::Yes(Data, "SCI::Mouse", "Hide", "Do you want to hide the system's mouse pointer")));
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

		std::string SCI_Project::ReleaseDirectory() {
			auto dir{ Ask(Data,"Release","Directory_"+Platform(),"Directory for release: ")};
			dir = ChReplace(dir, '\\', '/');
			if (!Suffixed(dir, "/")) dir += "/";
			return AVolPath(dir);
		}

		std::string SCI_Project::ReleaseDirectory(std::string pf) {
		    return ReleaseDirectory()+(Suffixed(ReleaseDirectory(),"/")?"":"/")+pf+"/";
		}

		std::string SCI_Project::PrefferedStorage() {
			return Ask(Data, "Release", "PrefStorage", "What storage method is preferred for releases? ", "zlib");
		}

		bool SCI_Project::DirIsSolid(std::string pkg, std::string d) {
			static std::map<std::string, bool> AlwaysSolid = { {".JPBF",true} };
			static std::map<std::string, bool> NeverSolid = { {".JFBF",true} };
			if (d == "") return false;
			for (auto as : AlwaysSolid) if (as.second && Upper(ExtractExt(d)) == as.first) return true;
			for (auto as : NeverSolid) if (as.second && Upper(ExtractExt(d)) == as.first) return false;
			return Units::Yes(Data, "SOLID::" + pkg, d, "Is the directory \"" + d + "\" to be packed as a solid block");
		}

		std::string SCI_Project::AssetsDir() {
			return AVolPath(Ask(Data, "Directory::"+Platform(), "SCI_Assets_Source", "Please tell me where the assets to include in this project are stored: "));
		}

		bool SCI_Project::AssetsMultiDir() {
			return Units::Yes(Data,"Directory","SCI_ASSETS_MULTIDIR","Is the assets dir set up for 'multi-dir'");
		}

		bool SCI_Project::UseMedals() {
			return Upper(Data->Value("MEDALS", "USE")) == "YES";
		}

		bool SCI_Project::Yes(std::string cat, std::string key, std::string question) { return Units::Yes(Data, cat, key, question); }

		std::string SCI_Project::Vraag(std::string cat, std::string key, std::string question, std::string defaultvalue) {
			return Ask(Data, cat, key, question, defaultvalue);
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
			//cout << "Debug flag -> " << DebugFlag() << endl; // Debug only!
			if (!DebugFlag()) {
				Export_Windows();
				//Export_Mac();
				Export_Linux_Basic();
				//Butler(this); // temporarily on dummy
			}
		}

		static void QCF(std::string ori, std::string tar) {
			if ((!FileExists(tar)) || (FileSize(tar) != FileSize(ori)) || (FileTimeStamp(tar) != FileSize(ori))) {
				QCol->Doing("Copying", ori, " ");
				QCol->Green(" -> ");
				QCol->Cyan(tar + "\n");
				FileCopy(ori, tar);
			}
		}

		void SCI_Project::JCRtoMe(std::string d, std::string an) {
		    auto JD{FileList(ReleaseDirectory("JCR6")) };
		    for (auto J:*JD) QCF(ReleaseDirectory("JCR6")+J,ReleaseDirectory(d)+(an==""?J:an));
		}


		void SCI_Project::Export_Windows() {
			QCol->Doing("Exporting to", "Windows");
			auto mydir{ ExtractDir(CLI_Args.myexe) }; // cout << mydir << endl;
			auto content{ FileList(mydir) };
			auto od{ReleaseDirectory("Win64")};
			if (!IsDir(od)) {
                QCol->Doing("Creating",od); MakeDir(od);
            }
			for (auto f : *content) {
				if (Lower(ExtractExt(f)) == "dll") {
					auto ori{ mydir + "/" + f };
					auto tar{ od + f };
					//cout << ori << " >> " << tar << endl;
					QCF(ori, tar);
				}
			}
			QCF(mydir + "/SCI_Run.exe", od + OutputName() + ".exe");
			QCF(mydir + "/SCI_Run.srf", od + OutputName() + ".srf");
			JCRtoMe("Win64");
			/*
			SaveString(
				ReleaseDirectory("Win64") + OutputName(),
				"#! /usr/bin/bash\n"
				"\n\n"
				"# This shell script SHOULD run the game in Linux providing that WINE has been installed\n"
				"# No official support, though. Make sure that this file has the 'x' attribute or it will NOT run at all\n\n"
				"wine \""+OutputName()+".exe\"\n\n"
			);
			*/
		}

        void SCI_Project::Export_Linux_Basic() {
            QCol->Doing("Exporting to", "Linux"," "); QCol->LMagenta("BASIC\n");
            // The Basic Linux install will contain, the executable, the srf file and the jcr data file, and nothing more.
            // Dependencies will have to be installed separately.
            auto od{ReleaseDirectory("Linux_Basic")};
            auto mydir{ ExtractDir(CLI_Args.myexe) };
            auto on{ ChReplace(Lower(OutputName()),' ','_')};
			if (!IsDir(od)) {
                QCol->Doing("Creating dir",od); MakeDir(od);
            }

           	QCF(mydir + "/SCI_Run", od + on);
			QCF(mydir + "/SCI_Run.srf", od + on+".srf");
			JCRtoMe("Linux_Basic");
        }

	}
}
