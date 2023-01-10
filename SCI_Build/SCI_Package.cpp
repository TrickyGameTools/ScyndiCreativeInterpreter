// Lic:
// Scyndi's Creative Interpreter - Builder
// Package
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
// Version: 23.01.10
// EndLic

#define Act(A) if (!A) return false

#include <JCR6_Core.hpp>

#include <SlyvAsk.hpp>
#include <SlyvDir.hpp>

#include "SCI_Package.hpp"
#include "SCI_Build_Config.hpp"
#include "SCI_Package.hpp"

using namespace Slyvina;
using namespace Units;
using namespace JCR6;

namespace Scyndi_CI {
	namespace Builder {

		std::map<std::string, std::string> _Package::ReservedDirs {
			{"ID","identification data"},
			{"SCRIPT","game scripts"},
			{"LIBS","script libraries"}
		};

		std::string _Package::Package(std::string dir) {
			if (_debug) return "*DEBUG";
			if (dir == "*SCRIPT*") return "*MAIN";
			return Ask(PrjData, "DIR2PACKAGE", dir, TrSPrintF("Which package is '%s' part of? ", dir.c_str()), "*MAIN");			
		}

		bool _Package::PackDir(std::string srcdir, Slyvina::VecString D, std::string Package, std::string Author, std::string Notes) {
			srcdir = ChReplace(srcdir, '\\', '/');
			QCol->Doing("Directory:", srcdir);
			if (_debug) {
				OutputJQL += "Author:"; OutputJQL += Author + "\n";
				OutputJQL += "Notes:"; OutputJQL += Notes + "\n";
				for (auto file : *D) {
					auto PWD{ ExtractDir(file) };
					auto Allow{ true };
					if (ReservedDirs.count(PWD)) Allow = Yes(PrjData, "SCI_RESERVEDDIRS", PWD, TrSPrintF("The directory '%s' has been reserved for %s!\nThis can lead to undesirable behavior!\nAre you sure you still want to include files for this directory",PWD.c_str(),ReservedDirs[PWD].c_str()));
					if (Allow) {
						auto ffile{ srcdir + "/" + file };
						if (_JT_Dir::Recognize(ffile) == "NONE") {
							QCol->Doing("Raw Link", file);
							OutputJQL += "Raw:" + ffile + ">" + file + "\n";
						} else {
							QCol->Doing("Patch Link", file);
							OutputJQL += "Patch:" + ffile + ">" + file + "\n";
						}
					}
				}
				OutputJQL + "\n\n";
			}
			return true;
		}

		bool _Package::AddString(std::string package, std::string entry, std::string content, std::string author, std::string notes) {
			if (_debug || package == "*DEBUG") {
				OutputJQL += "Author:"; OutputJQL += author + "\n";
				OutputJQL += "Notes:"; OutputJQL += notes + "\n";
				OutputJQL += "Text:"; OutputJQL += entry + "\n";
				OutputJQL += content + "\n@END@\n\n";
			}
			return true;
		}

		bool _Package::Pack() {
			QCol->Doing("Gather from", _Parent->AssetsDir());
			if (_Parent->AssetsMultiDir()) {
				QCol->Doing("Multi-Dir", "Yes");
				auto ad{ _Parent->AssetsDir() };
				auto md{ FileList(ad,DirWant::Directories) };
				for (auto d : *md) {
					std::string MDTag{ TrSPrintF("Assets_MultiDir::%s",d.c_str()) };
					if (Yes(PrjData, MDTag, "Use", TrSPrintF("May I use directory '%s'", d.c_str()))) {
						auto
							author{ Ask(PrjData,MDTag,"Author",TrSPrintF("Author of directory '%s':",d.c_str()),d.c_str()) },
							notes{ Ask(PrjData,MDTag,"Notes",TrSPrintF("Notes for directory '%s':",d.c_str())) },
							package{ Ask(PrjData,MDTag,"Package",TrSPrintF("Directory '%s' belongs to package:",d.c_str()),"*MAIN") };
						auto stuff = GetTree(ad + "/" + d);
						PackDir(ad + "/" + d, stuff, package, author, notes);
					}
				}
			} else {
				QCol->Doing("Multi-Dir", "No");
				auto d = GetTree(_Parent->AssetsDir());
				PackDir(_Parent->AssetsDir(), d, "*MAIN", _Parent->Author(), _Parent->Copyright() + "\n" + _Parent->License());
			}
			return true;
		}

		_Package::_Package(SCI_Project *P,GINIE G) {
			_Parent = P;
			PrjData = G;
			if (DebugFlag()) {
				QCol->Doing("Build mode", "Debug");
				_debug = true;
				OutputJQL = "JQL\n\n";
			}
			QCol->Doing("Build mode", "Release");
		}

		_Package::~_Package() {
			if (_debug) {
				auto ofile{ _Parent->DebugJQLFile() };
				auto odir{ ExtractDir(ofile) };
				if (odir.size() && (!DirectoryExists(odir))) {
					QCol->Doing("Creating dir",odir);
				}
				QCol->Doing("Saving",ofile);
				SaveString(ofile, OutputJQL);
				return;
			}
			QCol->Error("Finalizing not yet implemented for release builds");
		}

		bool HandlePackage(SCI_Project* P, GINIE G) {
			QCol->Doing("Packaging", P->OutputName());
			auto HP{ Package(new _Package(P,G)) };
			QCol->Doing("Identifying", P->File());
			Act(HP->AddString("*MAIN", "ID/Identify.ini", G->UnParse("Identify data for Scyndi's Creative Interpreter"), P->Author(), P->Copyright()));
			Act(HP->Pack());
			return true;
		}
	}
}