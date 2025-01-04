// License:
// 
// Scyndi's Creative Interpreter - Builder
// Package
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
// Version: 25.01.04
// End License

#define Act(A) if (!A) return false

#include <JCR6_Core.hpp>
#include <JCR6_Write.hpp>

#include <SlyvAsk.hpp>
#include <SlyvDir.hpp>
#include <SlyvDirry.hpp>
#include <SlyvVecSearch.hpp>

#include "SCI_Package.hpp"
#include "SCI_Build_Config.hpp"
#include "SCI_Package.hpp"

using namespace Slyvina;
using namespace Units;
using namespace JCR6;
using namespace std;

namespace Scyndi_CI {
	namespace Builder {

		struct PackResult {
			bool Success{false};
			VecString Dependencies{NewVecString()};
		};

#pragma region ScriptRecord
		class ScriptRecord {
		public:
			String
				FullFileName,
				FileName,
				ResDir;
			String ResFileName(bool wantbytecode) { 
				auto FN{ StripExt(FileName) }; if (CLI_Args.bool_flags["scyndidebug"]) FN = StripExt(FN); 
				if (wantbytecode) FN += ".lbc"; else FN += ".lua";
				return ResDir + "/" + FN; 
			}
			ScriptRecord(String FFN, String FN, String RD) { FullFileName = ChReplace(FFN, '\\', '/'); FileName = ChReplace(FN, '\\', '/'); ResDir = ChReplace(RD, '\\', '/'); }
		};
		
		typedef std::shared_ptr<std::vector<std::shared_ptr<ScriptRecord>>> SRList;
		inline SRList NewSRList() { return std::make_shared<std::vector<std::shared_ptr<ScriptRecord>>>(); }
		void SRPush(SRList L, String FFN, String FN, String RD) { L->push_back(std::make_shared<ScriptRecord>(FFN, FN, RD)); }
		bool SRFind(SRList L, String FFN) {
			FFN = ChReplace(FFN, '\\', '/');
			for (auto a : *L) if (a->FullFileName == FFN) return true;
			return false;
		}
		void SRPushUnique(SRList L, String FFN, String FN, String RD) { if (!SRFind(L,FFN)) L->push_back(make_shared<ScriptRecord>(FFN, FN, RD)); }
#pragma endregion
		


		std::map<std::string, std::string> _Package::ReservedDirs {
			{"ID","identification data"},
			{"SCRIPT","game scripts"},
			{"LIBS","script libraries"}
		};

		JT_Create _Package::GetPackage(std::string pkg) {
			Trans2Upper(pkg);
			if (pkg == "*MAIN") return MainPackage;
			if (!Packages.count(pkg)) {
				if (_Parent->Yes("Release_Package_MergeWithMain", pkg, "Merge package " + pkg + " with main package")) return MainPackage;
				auto jname{ _Parent->ReleaseDirectory() + _Parent->OutputName() + "." + pkg + ".jcr" };
				QCol->Doing("Creating", jname," ");
				QCol->Doing("For Package", pkg);
				Packages[pkg] = CreateJCR6(jname,_Parent->PrefferedStorage());
				if (Yes("Release_Package_Optional", pkg, "Is this package optional and can the game function without it")) {
					MainPackage->Import(jname);
				} else {
					MainPackage->Require(jname);
				}
			}
			return JT_Create();
		}

		Slyvina::JCR6::JT_CreateBlock _Package::GetBlock(std::string pkg, std::string dir) {
			Trans2Upper(pkg);
			Trans2Upper(dir);
			auto tag{ pkg + "::" + dir };
			if (!Blocks.count(tag)) {
				QCol->Doing("Creating block", tag);
				auto jpkg{ GetPackage(pkg) };
				Blocks[tag] = jpkg->AddBlock(_Parent->PrefferedStorage());
			}
			return Blocks[tag];
		}


		std::string _Package::Package(std::string dir) {
			if (_debug) return "*DEBUG";
			if (dir == "*SCRIPT*") return "*MAIN";
			return Ask(PrjData, "DIR2PACKAGE", dir, TrSPrintF("Which package is '%s' part of? ", dir.c_str()), "*MAIN");			
		}

		bool _Package::EligableScriptBundle(std::string bundle) {
			Trans2Upper(bundle);
			if (CLI_Args.bool_flags["scyndidebug"]) return Suffixed(bundle, ".DEBUG.STB");
			return Suffixed(bundle,".STB") && (!Suffixed(bundle,".DEBUG.STB"));
		}

		bool _Package::PackDir(std::string srcdir, Slyvina::VecString D, std::string Package, std::string Author, std::string Notes) {
			srcdir = ChReplace(srcdir, '\\', '/');
			QCol->Doing("Directory", srcdir);
			if (_debug) {
				OutputJQL += "Author:"; OutputJQL += Author + "\n";
				OutputJQL += "Notes:"; OutputJQL += Notes + "\n";
				for (auto file : *D) {
					auto PWD{ ExtractDir(file) };
					auto Allow{ true };
					if (ReservedDirs.count(PWD)) Allow = Yes(PrjData, "SCI_RESERVEDDIRS", PWD, TrSPrintF("The directory '%s' has been reserved for %s!\nThis can lead to undesirable behavior!\nAre you sure you still want to include files for this directory",PWD.c_str(),ReservedDirs[PWD].c_str()));
					if (Allow) {
						auto ffile{ srcdir + "/" + file };
						//std::cout << "JCR6 Recognize: " << ffile << " >> " << _JT_Dir::Recognize(ffile) << "\n"; // debug only
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
			} else {
				OutputJQL += "Author:"; OutputJQL += Author + "\n";
				OutputJQL += "Notes:"; OutputJQL += Notes + "\n";
				for (auto file : *D) {
					auto PWD{ ExtractDir(file) };
					auto Allow{ true };
					if (ReservedDirs.count(PWD)) Allow = Yes(PrjData, "SCI_RESERVEDDIRS", PWD, TrSPrintF("The directory '%s' has been reserved for %s!\nThis can lead to undesirable behavior!\nAre you sure you still want to include files for this directory", PWD.c_str(), ReservedDirs[PWD].c_str()));
					if (Allow) {
						auto ffile{ srcdir + "/" + file };
						if (_JT_Dir::Recognize(ffile) == "NONE") {
							QCol->Doing("Adding", ffile, " ");
							if (_Parent->DirIsSolid(Package, PWD)) {
								GetBlock(Package, PWD)->AddFile(ffile, file, Author, Notes);
								QCol->Magenta("Added to block: " + Package + "::" + PWD+"\n");
							} else {
								string Storage{ "Store" };
								if (FileSize(ffile) > 4000) Storage = _Parent->PrefferedStorage();
								GetPackage(Package)->AddFile(ffile, file, Storage, Author, Notes);
								auto de{ GetPackage(Package)->Entries[Upper(file)] };
								if (de->Storage() == "Store") 
									QCol->LMagenta("Stored\n");
								else { 
									QCol->LMagenta("Packed "); 
									QCol->LGreen(de->Storage());
									QCol->LCyan(TrSPrintF(" %5.1f%%\n", ((double)de->CompressedSize() / (double)de->RealSize()) * 100));
								}
							}
						} else {
							auto D{ JCR6_Dir(ffile) };
							auto EntryList{ D->Entries() };
							for (auto entry : *EntryList) {
								auto _Author{ entry->Author() };
								auto _Notes{ entry->Notes() };
								if (_Author == "" && _Parent->Yes("JCR6MERGE_AUTHOR_TRANSFER::" + Package, file, "No author data found for " + entry->Name() + ".Just use package data")) _Author = Author;
								if (_Notes == "" && _Parent->Yes("JCR6MERGE_NOTES_TRANSFER::" + Package, file, "No notes data found for " + entry->Name() + ". Just use package data")) _Notes = Notes;
								QCol->Doing("Adding", "", "");
								QCol->Magenta(ffile + "/");
								QCol->Cyan(entry->Name()+" ");
								if (_Parent->DirIsSolid(Package, file)) {
									GetBlock(Package, file)->AddBank(D->B(entry->Name()), file + "/" + entry->Name(), _Author, _Notes);
									QCol->Magenta("Added to block: " + Package + "::" + PWD + "\n");
								} else {
									string
										Storage{ "Store" },
										TarName{ file + "/" + entry->Name() };
									if (entry->RealSize() > 4000) Storage = _Parent->PrefferedStorage();
									GetPackage(Package)->AddBank(D->B(entry->Name()), TarName, Storage, _Author, _Notes);
									auto de{ GetPackage(Package)->Entries[Upper(TarName)] };
									if (de->Storage() == "Store")
										QCol->LMagenta("Stored\n");
									else {
										QCol->LMagenta("Packed ");
										QCol->LGreen(de->Storage());
										QCol->LCyan(TrSPrintF(" %5.1f%%\n", ((double)de->CompressedSize() / (double)de->RealSize()) * 100));
									}
								}

							}
						}
					}
				}
				//QCol->Error("Packing for release not yet implemented");
				return false;
			}
			return true;
		}

		PackResult _Package::PackScript(std::string bundle, bool bytecode, std::string entry, std::string package, std::string directory) {
			PackResult ret;
			//cout << " GGRR!\n"; // debug
			if (_debug) {
				QCol->Doing("Script Link", bundle);
				OutputJQL += "From:" + bundle + "\n";				
				if (bytecode) {
					//OutputJQL += "Steal:Bytecode.lbc>" + directory + "/" + entry + ".lbc\n";
					OutputJQL += "Steal:Bytecode.lbc>" + entry + "\n";
				} else {
					//OutputJQL += "Steal:Translation.lua>" + directory + "/" + entry + ".lua\n";
					OutputJQL += "Steal:Translation.lua>" + entry + "\n";
				}
			} else {
				QCol->Doing("Script Import", bundle);
				auto JS{ JCR6_Dir(bundle) };
				auto PWD{ ExtractDir(entry) };
				if (_Parent->DirIsSolid(package, PWD)) {
					if (bytecode) {
						GetBlock(package, PWD)->AddBank(JS->B("ByteCode.lbc"), entry,_Parent->Author());
					} else {
						GetBlock(package, PWD)->AddBank(JS->B("translation.lua"), entry, _Parent->Author());
					}
				} else {
					if (bytecode) {
						if (JS->Entry("Bytecode.lbc")->RealSize() > 4000)
							GetPackage(package)->AddBank(JS->B("ByteCode.lbc"), entry, _Parent->PrefferedStorage(), _Parent->Author());
						else
							GetPackage(package)->AddBank(JS->B("ByteCode.lbc"), entry, "Store", _Parent->Author());
					} else {
						if (JS->Entry("Translation")->RealSize() > 4000)
							GetPackage(package)->AddBank(JS->B("translation.lua"), entry, _Parent->PrefferedStorage(), _Parent->Author());
						else
							GetPackage(package)->AddBank(JS->B("translation.lua"), entry, "Store", _Parent->Author());
					}
				}
				//QCol->Error("Release not yet implemented for script"); 
			}
			return ret;
		}

		

		bool _Package::PackScript() {
			QCol->LMagenta("Script inclusion\n");
			auto wantbytecode = Yes(PrjData, "Script", "ByteCode", "Import the bytcode of the scripts");
			auto wantpackage = Ask(PrjData, "Script", "Package", "Put script in package: ", "*MAIN");
			auto wantlibs = NewVecString();
			auto listscripts = PrjData->List("Directory::"+Platform(), "SourceFiles");
			auto listlibs = PrjData->List("Directory::"+Platform(), "Libraries");
			/*
			auto
				DepDone{ NewVecString() },
				DepNeed{ NewVecString() },
				DepNextNeed{ NewVecString() };
			*/
			auto
				DepDone{ NewSRList() },
				DepNeed{ NewSRList() },
				DepNextNeed{ NewSRList() };

			VecString List;
			auto
				SourceList{ PrjData->List("Directory::"+Platform(),"SourceFiles")},
				LibList{ PrjData->List("Directory::"+Platform(),"Libraries")};
			QCol->Doing("Src dirs", SourceList->size());
			QCol->Doing("Lib dirs", LibList->size());
			for (auto sl : *SourceList) {
				QCol->Doing("Scanning", ChReplace(sl, '\\', '/'));
				auto d = GetTree(sl);
				for (auto bndl : *d) {
					//cout << "Is " << bndl << " eligable for linking? >> " << EligableScriptBundle(bndl) << endl; // debug
					if (EligableScriptBundle(bndl)) SRPush(DepNeed, sl + "/" + bndl, bndl, "Script"); // DepNeed->push_back(sl + "/" + bndl);
				}
				uint64 TimeOut{ 0xffffff };
				while (DepNeed->size()) {
					TimeOut--;
					if (TimeOut % 10000 == 0) QCol->Doing("Time Warning", TimeOut);
					if (!TimeOut) QCol->Error("Timeout");
					for (auto bndl : *DepNeed) {
						//auto bndl{ sl + "/" + _bndl };
						auto J = JCR6_Dir(bndl->FullFileName);
						if (Last()->Error) { 
							QCol->Error(TrSPrintF("JCR ERROR: %s", Last()->ErrorMessage.c_str())); 
							QCol->Doing("JCR6 Main:", Last()->MainFile);
							QCol->Doing("JCR6 Entry:", Last()->Entry);
							return false; 
						}
						if (!J->EntryExists("Configuration.ini")) {
							QCol->Error(TrSPrintF("Bundle '%s' does not contain configuration", bndl->FullFileName.c_str()));
							return false;
						}
						QCol->Doing("Analysing", bndl->FullFileName);
						auto bcfg{ ParseGINIE(J->GetString("Configuration.ini")) };
						auto list{ bcfg->List("DEPENDENCIES","LIST") };

						for (auto ldep : *list) {
							string dep{ "" };
							if (Prefixed(Upper(ldep), "LIBS/")) {
								//QCol->Error("Dependencies NOT YET supported!"); return false;
								//* 
								dep = ldep.substr(5);
								auto found{ false };
								QCol->Doing("=> Dep", ldep);
								for (auto libd : *LibList) {
									auto fdep{ libd + "/" + dep };
									if (CLI_Args.bool_flags["scyndidebug"]) fdep += ".Debug"; fdep += ".stb";
									//cout << "Checking: " << fdep << endl;
									if ((!SRFind(DepDone, fdep)) && (!SRFind(DepNeed, fdep))) {
										if (FileExists(fdep)) {
											//VecPushUnique(DepNextNeed, fdep);
											//cout << "Push " << fdep << "," << dep << ", Libs\n";
											auto tdep{ dep }; if (CLI_Args.bool_flags["scyndidebug"]) tdep += ".Debug"; tdep += ".stb";
											SRPushUnique(DepNextNeed, fdep, tdep, "Libs");
											found = true;
										}
									} else found = true;
								}
								if (!found) { QCol->Error("Dependency not found in any of the library directories"); return false; }
								//*/
							}
						}
						string ent{ bndl->ResFileName(wantbytecode) };
						string dir{ bndl->ResDir }; // No longer needed, but kept for saving myself some work
						PackScript(bndl->FullFileName, wantbytecode, ent, wantpackage , dir);
					}
					DepNeed = DepNextNeed;
					DepNextNeed = NewSRList(); //NewVecString();
				}
			}
			return true;
		}

		bool _Package::AddString(std::string package, std::string entry, std::string content, std::string author, std::string notes) {
			if (_debug || package == "*DEBUG") {
				OutputJQL += "Author:"; OutputJQL += author + "\n";
				OutputJQL += "Notes:"; OutputJQL += notes + "\n";
				OutputJQL += "Text:"; OutputJQL += entry + "\n";
				OutputJQL += content + "\n@END@\n\n";
			} else {
				string storage{ "Store" };
				if (content.size() > 4000) storage = _Parent->PrefferedStorage();
				GetPackage(package)->AddString(content, entry, storage, author, notes);
			}

			return true;
		}

		bool _Package::AddMedals(std::string package) {
			if (_debug || package == "*DEBUG") {
				if (_Parent->UseMedals()) {
					std::string MedalP{ "E:/projects/applications/site/Medals/Medals.ini" };
					auto Medal{ Dirry("$AppSupport$/Medals.JBC") };
					if (FileExists(MedalP)) {
						QCol->Doing("Importing", MedalP);
						OutputJQL += "Author:Jeroen P. Broks\nNotes:To be kept within THIS project alone!\nRaw:" + MedalP + ">ID/Medals.ini\n\n";
					} else {
						if (!FileExists(Medal)) {
							QCol->Error(Medal + " could not be found!");
							return false;
						}
						QCol->Doing("Stealing from", Medal);
						OutputJQL += "From:" + Medal + "\n";
						OutputJQL += "Steal:Score>ID/Medals.ini\n\n";
					}
				}
			} else {
				std::string MedalP{ "E:/projects/applications/site/Medals/Medals.ini" };
				auto Medal{ Dirry("$AppSupport$/Medals.JBC") };
				if (FileExists(MedalP)) {
					string storage{ "Store" };
					if (FileSize(MedalP) > 4000) storage = "zlib";
					QCol->Doing("Importing", MedalP);
					MainPackage->AddFile(MedalP, "ID/Medals.ini", storage, "Jeroen P. Broks", "To be kept within THIS project alone!");
				} else {
					if (!FileExists(Medal)) {
						QCol->Error(Medal + " could not be found!");
						return false;
					}
					QCol->Doing("Stealing from", Medal);
					auto J{ JCR6_Dir(Medal) };
					auto mcontent{ J->GetString("Score") };
					AddString("*MAIN", "ID/Medals", mcontent, "Jeroen P. Broks", "To be kept within THIS project alone!");
				}
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

		bool _Package::Aliases() {
			auto AL{ PrjData->Values("Alias") };
			QCol->Doing("Aliases", (int)AL->size());
			for (auto ALE : *AL) {
				if (DebugFlag()) {
					QCol->Doing("Alias", ALE);
					OutputJQL += "Alias:" + ALE + ">" + PrjData->Value("Alias", ALE)+"\n";
				} else {
					uint32 c{ 0 };
					for (auto PKG : Packages) {
						//QCol->Red("DEBUG: " + ALE + "::" + PKG.first + "\n");
						if (PKG.second->Entries.count(Upper(ALE))) {
							QCol->Doing("Alias", ALE, "");
							QCol->Yellow(" to ");
							QCol->Cyan(PrjData->Value("Alias", ALE));
							QCol->LMagenta("   " + PKG.first + "\n");
							PKG.second->Alias(ALE, PrjData->Value("Alias", ALE));
							c++;
						}
					}
					switch (c) {
					case 1: break; // Nothing wrong
					case 0: 
						QCol->Error("Original not found for alias: " + ALE);
						return false;
					default:
						QCol->Error("Multiple occurances found for alias: " + ALE + ", but only one will actually be used in the game!");
					}
				}
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
			} else {
				QCol->Doing("Build mode", "Release");
				auto ofile{ P->ReleaseDirectory() + P->OutputName() + ".jcr" };
				QCol->Doing("Creating", ofile);
				MainPackage = CreateJCR6(ofile);				
				Packages["*MAIN"] = MainPackage;
			}
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
			for (auto b : Blocks) {
				QCol->Doing("Closing block", b.first);
				b.second->CloseAllEntries();
				b.second->Close();
			}
			for (auto p : Packages) {
				QCol->Doing("Closing package", p.first);
				p.second->CloseAllEntries();
				p.second->Close();
			}
			//QCol->Error("Finalizing not yet implemented for release builds");
		}

		bool HandlePackage(SCI_Project* P, GINIE G) {
			QCol->Doing("Packaging", P->OutputName());
			auto HP{ Package(new _Package(P,G)) };
			QCol->Doing("Identifying", P->File());
			//Act(HP->AddString("*MAIN", "ID/Identify.ini", G->UnParse("Identify data for Scyndi's Creative Interpreter"), P->Author(), P->Copyright()));
			Act(HP->AddString("*MAIN", "ID/Identify.ini", P->IDUnparsed(),P->Author(), P->Copyright()));
			Act(HP->AddMedals("*MAIN"));
			Act(HP->Pack());
			Act(HP->PackScript());
			Act(HP->Aliases());
			return true;
		}
	}
}
