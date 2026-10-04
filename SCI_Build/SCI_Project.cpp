// License:
//
// Scyndi's Creative Interpreter - Builder
// Project Manager
//
//
//
// 	(c) Jeroen P. Broks, 2023, 2024, 2025, 2026
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
// Version: 26.06.27
// End License

#include <SlyvQCol.hpp>
#include <SlyvAsk.hpp>
#include <SlyvTime.hpp>
#include <SlyvVolumes.hpp>
#include <SlyvMD5.hpp>
#include <SlyvDir.hpp>
#include <SlyvStream.hpp>
#include <SlyvDirry.hpp>
#include <SlyvConInput.hpp>

#include <JCR6_Write.hpp>

#include "../SCI_Share/Version.hpp"
#include "../SCI_Share/SCI_GlobalConfig.hpp"

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
				ID->Value("Window", "Height", Ask(Data, "SCI::Window", "WinHeight", "Window height (suffix with % to make it calculate on the desktop size)", "1000"));
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
			return AVolPath(Dirry(dir));
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
				#ifdef SlyvLinux
				Export_Linux_AppImage();
				Export_Linux_Debian();
				#endif // SlyvLinux
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
        	if (!Yes("Linux_Basic","Do","Do you want a Linux Basic build")) return;
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

        static String NewAppImageTool(GINIE d) {
        	auto IT{d->Value("AppImage","AltAppImageTool")};
        	while (IT=="" && (!FileExists(IT)))  {
				d->Value("AppImage","AltAppImageTool","");
				IT=Ask(d,"AppImage","AltAppImageTool","AppImageTool not found! Where can I find it? Full name with path please: ");
			}
			return IT;

        }

        union _ec {byte b[4];int i;};
        void SCI_Project::Export_Linux_AppImage() {
			if (!Yes("AppImage","Do","Do you want to export this project to Linux .AppImage")) return;
			QCol->Doing("Exporting to", "Linux"," "); QCol->LMagenta("AppImage\n");
			#ifndef SlyvLinux
			QCol->Error("Linux is required in order to create an .AppImage");
			return;
			#endif // SlyvLinux
			auto mydir{ ExtractDir(CLI_Args.myexe) }; // cout << mydir << endl;
			String Session{""};
			String RDP{Ask(Data,"AppImage","RAMDISKDIR","Which directory should I use to mount RAM disk used to create .AppImage?",Dirry("$Home$/SCI_RAMDISK"))};
			int RDS{AskInt(Data,"AppImage","MAXGB","Maximum size for the RAM disk in gigabytes: ",6)};
			String Icon{Ask(Data,"AppImage","Icon","Icon file:")};
			String Categories{Ask(Data,"AppImage","Categories","Which categories should I add to the manifest? ","Game;")};
			String Desktop{"[Desktop Entry]\n"};
			String DTName{Ask(Data,"AppImage","DT.NAME","Desktop -> Name:")};
			String DTExec{Ask(Data,"AppImage","DT.EXEC","Desktop -> Exec:",DTName)};
			Desktop+="Name="+DTName+"\n";
			Desktop+="Exec="+DTExec+"\n";
			Desktop+="Icon=GameIcon\n";
			Desktop+="Type=Application\n";
			Desktop+="Categories="+Categories+"\n";
			if (!IsDir(RDP)) {
				QCol->Doing("Creating",RDP);
				MakeDir(RDP);
			}
			do {
				static uint32 c{0};
				Session=md5(CurrentDate()+" "+CurrentTime())+TrSPrintF("_%d",++c);
			} while (IsDir(RDP+"/"+Session));
			String SESD{RDP+"/"+Session};
			QCol->Doing("Session",Session);
			String MOUNT{TrSPrintF("sudo mount -t tmpfs -o size=%dG -o mode=1777 tmpfs ",RDS)+SESD};
			QCol->Doing("Mount",SESD);
			MakeDir(SESD);
			QCol->Grey("$ "+MOUNT+"\n");
			_ec e; e.i=system(MOUNT.c_str());
			if (e.i) {
				QCol->Error(TrSPrintF("Mount failed: %d.%d.%d.%d (%i)",e.b[0],e.b[1],e.b[2],e.b[3],e.i));
				exit(e.b[1]); // This seems to be the true exit code (bug in Linux?)
				return;
			}
			String AppDir{SESD+"/"+DTExec+".AppDir"};
			QCol->Doing("Creating",AppDir);
			MakeDir(AppDir);
			QCol->Doing("Saving","Desktop");
			SaveString(AppDir+"/"+DTExec+".desktop",Desktop);
			//QCol->Doing("Icon",Icon);
			QCF(Icon,AppDir+"/GameIcon.png");
			static String libraries[6] {
				"lib/x86_64-linux-gnu/libc.so.6",
				"lib/x86_64-linux-gnu/libgcc_s.so.1",
				"lib/x86_64-linux-gnu/libstdc++.so.6",
				"usr/local/lib/libSDL2-2.0.so.0",
				"usr/local/lib/libSDL2_image-2.0.so.0",
				"usr/local/lib/libSDL2_mixer-2.0.so.0"
			};
			for(auto& lib:libraries) {
				QCol->Doing("Library",lib);
				auto D{ExtractDir(lib)};
				auto TD{AppDir+"/"+D};
				if (IsDir(TD)) {QCol->Doing("Creating",TD); MakeDir(TD); }
				FileCopy("/"+lib,TD+"/"+StripDir(lib));
			}
			MakeDir(AppDir+"/usr/bin");
			QCF(mydir + "/SCI_Run",     AppDir+"/usr/bin/SCI_Run");
			QCF(mydir + "/SCI_Run.srf", AppDir+"/usr/bin/SCI_Run.srf");
			String MEX{"chmod +x "+AppDir+"/usr/bin/SCI_Run"}; system(MEX.c_str());
			auto JD{FileList(ReleaseDirectory("JCR6")) };
		    for (auto J:*JD) QCF(ReleaseDirectory("JCR6")+J,AppDir+"/usr/bin/"+J);

		    String AppRun{"#!/usr/bin/sh\n"};
		    AppRun+="export LD_LIBRARY_PATH=\"$APPDIR/usr/local/lib:$LD_LIBRARY_PATH\"\n";
		    AppRun+="$APPDIR/usr/bin/SCI_Run ";
		    AppRun+="\"$APPDIR/usr/bin/"+Data->Value("Project","OutputName")+".jcr\"\n";
		    AppRun+="echo Exit code $?\n\n# Script Generated with SCI_Build\n#(c) Jeroen P. Broks\n\n";
		    SaveString(AppDir+"/AppRun",AppRun);
		    String ARX{"chmod +x "+AppDir+"/AppRun"}; system(ARX.c_str());
		    String AppImageTool{mydir+"/appimagetool-x86_64.AppImage" };
		    AppImageTool=FileExists(AppImageTool)?AppImageTool:NewAppImageTool(Data);
		    String AITC{"'"+AppImageTool+"' "+AppDir};
		    QCol->Doing("Creating","AppImage");
		    QCol->Grey("$ "+AITC+"\n");
		    String OldD{CurrentDir()};
		    //ChangeDir(ExtractDir(AppDir));
		    String RelDir{ReleaseDirectory("Linux_AppImage")};
		    if (!IsDir(RelDir)) { QCol->Doing("Creating",RelDir); MakeDir(RelDir);}
		    ChangeDir(RelDir);
		    e.i=system(AITC.c_str());
		    ChangeDir(OldD);
			if (e.i) {
				QCol->Error(TrSPrintF("AppImage Creation failed: %d.%d.%d.%d (%i)",e.b[0],e.b[1],e.b[2],e.b[3],e.i));
				exit(e.b[1]); // This seems to be the true exit code (bug in Linux?)
				return;
			}
			QCol->Doing("Unmounting",SESD);
			MOUNT="sudo umount "+SESD;
			system(MOUNT.c_str());
			MOUNT="rm -R "+SESD;
			QCol->Doing("Destroying",SESD);
			system(MOUNT.c_str());
       }

       void SCI_Project::Export_Linux_Debian() {
			// control
			std::string
				deb_control {""},
				deb_package{""},
				deb_icon{""},
				deb_license{"Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/\n"};

			// control - package
			do {
				deb_package=ChReplace(Lower(Ask(Data,"Debian","Package","Debian Package Name:")),' ','-');
				auto ok{true};
				for(size_t i=0;i<deb_package.size();i++) {
					auto ch{deb_package[i]};
					ok=ok && (ch=='-' || ch=='+' || (ch>='a' && ch<='z') || ch>='0' && ch<='9');
				}
				if (!ok) {
						QCol->Error("Invalid Debian Package Name!");
						Data->Value("Debian","Package","");
				}
			} while(Data->Value("Debian","Package")=="");
			deb_control+="Package: "+deb_package+"\n";
			// Version
			auto deb_version{TrSPrintF("%d.%d.%d",CurrentYear()-2000,CurrentMonth(),CurrentDay())};
			if (Data->Value("Debian","LastVersion")==deb_version) {
				Data->Value("Debian","TimesToday",Data->IntValue("Debian","TimesToday")+1);
				deb_version+=TrSPrintF("-%d",Data->IntValue("Debian","TimesToday"));
			} else {
				Data->Value("Debian","LastVersion",deb_version);
				Data->Value("Debian","TimesToday",0);
			}
			deb_control+="Section: games\nPriority: optional\n";
			deb_control+="Version: "+deb_version+"\n";
			// Architecture
			deb_control+="Architecture: amd64\n";
			// Maintainer
			deb_control+="Maintainer: ";
			deb_control+=Ask(Data,"Debian","Maintainer_Name","Debian Maintainer Name:",Data->Value("AA_META","02_CREATEDBY"));
			deb_control+=" <"+Ask(Data,"Debian","Maintainer_Mail","Debian Maintainer e-mail:")+">\n";
			// Dependencies (SDL2)
			deb_control+="Depends: libsdl2-2.0-0 (>= 2.0.0), libsdl2-image-2.0-0 (>= 2.0.0), libsdl2-mixer-2.0-0 (>= 2.0.0)\n";
			// Description
			deb_control+="Description: "+Ask(Data,"Debian","Description_Short","Debian Short Description: ")+"\n";
			deb_control+=" "+StReplace( Ask(Data,"Debian","Description_Long","Debian Long Description: (<nl>=New Line) "),"<nl>","\n ")+"\n";

			// Copyright
			std::vector<std::string> licsasked{}; licsasked.push_back("GPL-3.0+");
			deb_license+="Upstream-Name: "+Ask(Data,"Debian","License.Upstream-Name","License Upstream Name: ",Data->Value("Debian","Package"))+"\n";
			deb_license+="Upstream-Contact: "+Data->Value("Debian","Maintainer_Name")+" <"+Data->Value("Debian","Maintainer_Mail")+">\n";
			{
				auto src{Ask(Data,"Debian","License.Source","Repository source code:","NONE")};
				if (Upper(src)!="NONE") deb_license+="Source: "+src+"\n";
			}
			deb_license+="\nFiles: usr/bin/"+deb_package+"\n";
			deb_license+=TrSPrintF("Copyright: 2023-%d",CurrentYear());
			deb_license+="\nLicense: GPL-3.0+\n";
			deb_license+="\nFiles: usr/share/ScyndisCreativeInterpreter/"+deb_package+".jcr\n";
			{
				auto iyear{ToInt(Ask(Data,"Debian","License.Copyright.InitialYear","Project's initial copyright year: "))},cyear{CurrentYear()};
				deb_license+="Copyright: ";
				deb_license+=iyear==cyear?TrSPrintF("%d ",iyear):TrSPrintF("%d-%d ",iyear,cyear);
				deb_license+=Ask(Data,"Debian","License.Copyright.Owner","Project's copyright owner:",Data->Value("Debian","Maintainer_Name"))+"\n";
				auto lic{Ask(Data,"Debian","License.JCR6","Project's license: ","GPL-3.0+")};
				deb_license+="License: "+lic+"\n";

				// Only add license to list if it isn't already in there.
				auto havelic{false};
				for(auto& L:licsasked) {havelic=havelic||Upper(L)==Upper(lic);}
				if (!havelic) licsasked.push_back(lic);
			}

			for(auto&lic:licsasked) {
				auto lfile{Ask(GlobalConfig(),"Build.Licenses",lic,"Which file should I copy for the \""+lic+"\" license? ") };
				deb_license+="\nLicense: "+lic+"\n";
				if (FileExists(lfile)) {
					auto ll{LoadLines(lfile)};
					for (auto &l:*ll) { deb_license+=" "+l+"\n"; }
				} else {
					QCol->Error("\x07File "+lfile+" (for "+lic+") has not been found!");
					deb_license+=" -- No details about this license available --\n";
				}
			}

			// Desktop
			std::string deb_desktop{""};
			deb_desktop+="[Desktop Entry]\n";
			deb_desktop+="Name="+Ask(Data,"Debian","Desktop_Name","Desktop Application Name:",Data->Value("AA_META","01_TITLE"))+"\n";
			deb_desktop+="Exec=/usr/bin/"+deb_package+"\n";
			deb_desktop+="Icon="+deb_package+"\n";
			deb_desktop+="Type=Application\n";
			deb_desktop+="Categories=Game;\n";
			deb_icon = Ask(Data,"Debian","Icon","Debian Icon (PNG): ");

			// Meta-Info
			std::string deb_meta{"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<component type=\"desktop-application\">\n"};
			deb_meta+="\t<id>"+deb_package+"</id>\n";
			deb_meta+="\t<name>"+Data->Value("Debian","Desktop_Name")+"</name>\n";
			deb_meta+="\t<summary>"+Data->Value("Debian","Description_Short")+"</summary>\n";
			deb_meta+="\t<description>\n\t<p>"+StReplace(Data->Value("Debian","Description_Long"),"<nl>","</p>\n\t<p>")+"</p></description>\n";
			{
				auto url{Ask(Data,"Debian","MetaInfo.Homepage","Project homepage URL: ","NONE")};
				if (Upper(url)!="NONE") deb_meta+="\t<url type=\"homepage\">"+url+"</url>\n";
			}
			//deb_meta+="\t<icon type=\"local\">usr/share/icons/hicolor/256x256/apps/"+deb_package+".png</icon>\n";
			deb_meta+="\t<icon type=\"stock\">"+deb_package+"</icon>\n";
			deb_meta+="\t<metadata_license>CC0-1.0</metadata_license>\n";
			deb_meta+="\t<project_license>LicenseRef-"+Data->Value("Debian","License.JCR6")+"</project_license>\n";
			deb_meta+="\t<content_rating type=\"oars-1.1\">";
			{
				static const size_t catcnt=10;
				static char cats[catcnt][30]{"violence-cartoon", "violence-realistic", "violence-bloodshed", "sex-nudity", "sex-themes", "language-profanity", "language-discrimination", "drugs-alcohol", "drugs-narcotics", "gambling" };
				for(size_t i=0;i<catcnt;i++) {
					char *cat{cats[i]};
					while(Data->Value("Debian","Rating."+std::string(cat))=="") {
						static const size_t ratcnt=4;
						static char rating[ratcnt][10] = {"none","mild","moderate","intense"};
						QCol->Yellow(); printf("No rating has been set for category '%s'. Please set one:\n",cat);
						for(size_t j=0;j<ratcnt;j++) {
							QCol->Cyan(); printf("%2d: ",j);
							QCol->LGreen(); printf("%s\n",rating[j]);
						}
						int crating{-1};
						do {
							QCol->Yellow("Please give a rating: ");
							QCol->LCyan();
							crating=ToInt(ReadLine());
						} while(crating<0||crating>=ratcnt);
						Data->Value("Debian",(std::string)"Rating."+cat,rating[crating]);
					}
					if (Lower(Data->Value("Debian","Rating."+std::string(cat)))!="none") {
						deb_meta+="\t\t<content_attribute id=\"";
						deb_meta+=cat;
						deb_meta+="\">"+Lower(Data->Value("Debian",(String)"Rating."+cat))+"</content_attribute>\n";
					}
				}
			}
			deb_meta+="\t</content_rating>\n";
			deb_meta+="</component>";

			// Create Debian Source folder
			std::string deb_debsrc{ReleaseDirectory("Linux_DebianSource")};
			QCol->Doing("Creating","Debian Source");
			if (!DirectoryExists(deb_debsrc)) {
				QCol->Doing("- Creating",deb_debsrc);
				MkDir(deb_debsrc);
			} else {
				// prevent conflicts with older builds
				QCol->Doing("- Cleaning",deb_debsrc);
				std::string rmcmd{"rm -R \""+deb_debsrc+"\"*"};
				system(rmcmd.c_str());
			}
			// Text files
			MkDir(deb_debsrc+"usr");
			QCol->Doing("- Saving","Control");
			MkDir(deb_debsrc+"DEBIAN");
			SaveString(deb_debsrc+"DEBIAN/control",deb_control);
			QCol->Doing("- Saving","Desktop");
			MkDir(deb_debsrc+"usr/share");
			MkDir(deb_debsrc+"usr/share/applications");
			SaveString(deb_debsrc+"usr/share/applications/"+deb_package+".desktop",deb_desktop);
			QCol->Doing("- Saving","Copyright");
			MkDir(deb_debsrc+"usr/share/doc/"+deb_package);
			SaveString(deb_debsrc+"usr/share/doc/"+deb_package+"/copyright",deb_license);
			QCol->Doing("- Saving","Meta Info");
			MkDir(deb_debsrc+"usr/share/metainfo");
			SaveString(deb_debsrc+"usr/share/metainfo/"+deb_package+".metainfo.xml",deb_meta);

			// Let's copy the binaries
			auto mydir{ ExtractDir(CLI_Args.myexe) };  // Dir where SCI_build with all stuff is located.
			MkDir(deb_debsrc+"usr/bin");
			QCol->Doing("- Creating","ELF");
			std::string ELF{"cat \""+mydir+"/SCI_Run\" \""+mydir+"/SCI_Run.srf\" > \""+deb_debsrc+"/usr/bin/"+deb_package+"\""};
			system(ELF.c_str());

			MkDir(deb_debsrc+"usr/share/icons");
			MkDir(deb_debsrc+"usr/share/icons/hicolor");
			MkDir(deb_debsrc+"usr/share/icons/hicolor/256x256");
			MkDir(deb_debsrc+"usr/share/icons/hicolor/256x256/apps");
			QCF(deb_icon,deb_debsrc+"usr/share/icons/hicolor/256x256/apps/"+deb_package+".png");
			//MkDir(deb_debsrc+"usr/share/"+deb_package);
			MkDir(deb_debsrc+"usr/share/ScyndisCreativeInterpreter");
			//SaveString(deb_debsrc+"usr/share/ScyndisCreativeInterpreter/Update.txt","Last Updated: "+Now()+"\n");
			//QCF(deb_debsrc+"usr/share/"+deb_package+"/"+deb_package+".jcr");
			auto JD{FileList(ReleaseDirectory("JCR6")) };
			auto JL{JCR6::CreateJCR6(deb_debsrc+"usr/share/ScyndisCreativeInterpreter/"+deb_package+".jcr")};
			QCol->Doing("- Resources",JD->size());
		    for (auto J:*JD) {
				//QCF(ReleaseDirectory("JCR6")+J,deb_package+"/usr/share/"+deb_package+"/"+J);
				QCF(ReleaseDirectory("JCR6")+J,deb_debsrc+"/usr/share/ScyndisCreativeInterpreter/"+J);
				JL->Import("/usr/share/ScyndisCreativeInterpreter/"+J);
		    }
		    JL->Close();

		    // Let dpkg-dep do its magic
		    std::string deb_debtar{ReleaseDirectory("Linux_DebianPackage")};
		    	QCol->Doing("Creating","Debian Package");
			if (!DirectoryExists(deb_debtar)) {
				QCol->Doing("- Creating",deb_debtar);
				MkDir(deb_debtar);
			} else {
				// prevent conflicts with older builds
				QCol->Doing("- Cleaning",deb_debtar);
				std::string rmcmd{"rm -R \""+deb_debtar+"\"*"};
				system(rmcmd.c_str());
			}
		    std::string dpkg{"dpkg --build \""+deb_debsrc+"\" \""+deb_debtar+deb_package+".deb\""};
			QCol->Doing("- Executing",dpkg);
			auto ec{system(dpkg.c_str())};
			if (ec>0) QCol->Error(TrSPrintF("DPKG returned: %d/%x",ec,ec));
       }

	}
}
