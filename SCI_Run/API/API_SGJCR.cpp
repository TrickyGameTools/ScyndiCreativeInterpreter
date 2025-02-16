// License:
// 
// Scyndi's Creative Interpreter
// JCR based savegames
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

#include <SlyvString.hpp>
#include <SlyvStream.hpp>
#include <SlyvRandom.hpp>
#include <SlyvTime.hpp>
#include <SlyvQCol.hpp>

#include <Statistician.hpp>
#include <Statistician_SaveJCR6.hpp>
#include <Statistician_LoadJCR6.hpp>

#include <JCR6_Write.hpp>

#include "../../SCI_Share/Version.hpp"

#include "../SCI_Script.hpp"
#include "../SCI_Config.hpp"

#include "API_Bank.hpp"
#include "API_SGJCR_Enum.hpp"


using namespace std;
using namespace Slyvina;
using namespace Units;
using namespace Statistician;
using namespace NSLunatic;
using namespace JCR6;

#define CheckTag if (!_SGRec::Register.count(Tag)) { luaL_error(L,"No savegame record tagged '%s' exists!",Tag.c_str()); return 0;}
#define AutoTag auto Tag{Upper(luaL_checkstring(L,1))}; CheckTag
#define AutoRec AutoTag auto Rec{ _SGRec::Register[Tag] }

namespace Scyndi_CI {

	Party JSG_GetParty(string T, bool create = false); // This is merely a header for API_Statistician.cpp | One definition only used here, a .hpp header could do harm than good.
	void JSG_SetParty(string t, Party P);

	class _SGRec; typedef shared_ptr<_SGRec> SGRec;
	class _SGRec {
	public:
		static map<string, SGRec> Register;
		map<string, Bank> Banks{};
		map<string, string> TextEntries{};
		map<string, string> PartyEntries{};
		map<SGJCR_Types, bool> DoType{};
		string JCRFile{ "" };
		//JT_Dir Res{ nullptr };
		static SGRec Create(string Tag) {
			Trans2Upper(Tag);
			Register[Tag] = make_shared<_SGRec>();
			Register[Tag]->DoType[SGJCR_Types::ID] = true;
			return Register[Tag];
		}
	};
	map<string, SGRec> _SGRec::Register{ {"*MAIN",make_shared<_SGRec>()} };

	static int API_SGJCRCreate(lua_State* L) {
		static uint64 count{ 0 };
		auto Tag{ Upper(luaL_optstring(L,1,"")) };
		if (Tag == "") {
			Tag = TrSPrintF("***AUTO_%08x", count++);
		} else if (Prefixed(Tag, "*")) {
			luaL_error(L, "Invalid JCR-Savegame tag '%s'", Tag.c_str());
			return 0;
		}
		_SGRec::Create(Tag);
		Lunatic_PushString(L, Tag);
		return 1;
	}

	static int API_SGJCRAssign(lua_State* L) {
		auto
			Tag{ Upper(luaL_checkstring(L,1)) },
			File{ Lunatic_CheckString(L,2) };
		CheckTag;
		_SGRec::Register[Tag]->JCRFile = File;
		return 0;
	}

	// The reason behind this function is that in my experience, deleting files is not always flawless on Windows systems.
	// This API function will at least see to it that a SaveGame file that should have been deleted will no longer be functional
	// in any way. And it being completely replaced with random bytes, will also make data recovery next to impossible.
	static int API_SGJCRMutilate(lua_State* L) {
		AutoTag;
		auto File{ SaveGameDir() + "/" + _SGRec::Register[Tag]->JCRFile };
		if (!File.size()) return 0;
		if (FileExists(File)) {
			auto S{ FileSize(File) };
			auto BO{ WriteFile(File) };
			for (size_t i = 0; i < S; i++) BO->Write((Byte)Rand.Get(0, 255));
			BO->Close();
		} else {
			cout << "\x1b[31mNOTE>\x1b[37m File \"" << File << "\" does not exist, so request to mutilate ignored!\n";
		}
		return 0;
	}

	static int API_SGJCRGetText(lua_State* L) {
		AutoRec;
		auto EntryN{ Upper(Lunatic_CheckString(L,2)) };
		Lunatic_PushString(L, Rec->TextEntries[EntryN]);
		return 1;
	}

	static int API_SGJCRSetText(lua_State* L) {
		//printf("SGJCRSetText request received from Lua State at $%x", (int)L);
		AutoRec;
		auto EntryN{ Upper(Lunatic_CheckString(L,2)) }, Content{ Lunatic_CheckString(L,3) };
		Rec->TextEntries[EntryN] = Content;
		Rec->DoType[SGJCR_Types::TxtData] = true;
		return 0;
	}

	static int API_SGJCRSetBank(lua_State* L) {
		AutoRec;
		auto
			EntryN{ Upper(luaL_checkstring(L,2)) },
			BankTag{ Lunatic_CheckString(L,3) };
		auto
			Bnk{ SGBank(BankTag) };
		if (!Bnk) { luaL_error(L, "Bank %s appears to be null", BankTag.c_str()); return 0; }
		Rec->DoType[SGJCR_Types::BinData] = true;
		Rec->Banks[EntryN] = Bnk;
		return 0;
	}

	static int API_SGJCRGetBank(lua_State* L) {
		AutoRec;
		auto
			EntryN{ Upper(luaL_checkstring(L,2)) },
			BankTag{ Lunatic_CheckString(L,3) };
		auto
			Bnk{ Rec->Banks[EntryN] };
		if (!Bnk) { luaL_error(L, "Bank %s does not appear to be present in the savegame", BankTag.c_str()); return 0; }
		SGBank(BankTag, Bnk);
		return 0;
	}

	static int API_SGJCRDoType(lua_State* L) {
		AutoRec;
		auto Type{ Upper(Lunatic_CheckString(L,2)) };
		if (!Nm2En.count(Type)) { luaL_error(L,"Unknown savegame datatype '%s'", Type.c_str()); return 0; }
		auto EType{ Nm2En[Type] };
		//printf("Do Tag: %s -> %d\n", Type.c_str(), (int)EType); // debug
		Rec->DoType[EType] = true;
		return 0;
	}

	static int API_SGJCRSetStat(lua_State* L) {
		AutoRec;
		auto
			STag{ Upper(luaL_checkstring(L,2)) },
			OutDir{ Lunatic_CheckString(L,3) };
		Rec->PartyEntries[STag] = OutDir;
		Rec->DoType[SGJCR_Types::RPGStat] = true;
		return 0;
	}

	static int API_SGJCRGetStat(lua_State* L) {
		AutoRec;
		auto
			STag{ Upper(luaL_checkstring(L,2)) };
		if (!Rec->PartyEntries.count(STag)) return 0;
		Lunatic_PushString(L, Rec->PartyEntries[STag]);
		return 1;
	}

	static int API_SGJCRSave(lua_State* L) {
		AutoRec;
		if (!Rec->JCRFile.size()) { luaL_error(L, "S:No file assigned to savegame record!"); return 0; }
		Rec->DoType[SGJCR_Types::ID] = true;
		QCol->Doing("Saving", Rec->JCRFile);
		auto FullOutFile{ SaveGameDir() +"/"+ Rec->JCRFile}, OutDir{ExtractDir(FullOutFile)};
		if (!DirectoryExists(OutDir)) {
			QCol->Doing("Creating Dir", OutDir);
			MakeDir(OutDir);
		}
		auto JO{ CreateJCR6(FullOutFile) };
		if (Last()->Error) { luaL_error(L, "Error creating savegame: %s", Last()->ErrorMessage.c_str()); return 0; }
		for (auto& IJD : Rec->DoType) {
			if (IJD.second) {
				switch (IJD.first) {
				case SGJCR_Types::ID: {
					QCol->Doing("-> ID", "ID/Idenify.ini");
					// TrSPrintF("# Savegame. Created: %s\n\n[ID]\nEngine=SCI\nEngineVersion=%s\nGameID=%s\n", CurrentDate().c_str()), QVersion.Version(false).c_str(), SaveGameID().c_str()
					std::string oID{ "# Savegame. Created:" + CurrentDate() + "\n"
						"[ID]\n"
						"EngineVersion=" + QVersion.Version(false) + "\n"
						"GameID=" + SaveGameID() + "\n"
						"Engine=SCI\n"
					};

					JO->AddString(oID, "ID/Identify.ini");
				} break;
				case SGJCR_Types::TxtData:
					for (auto& TD : Rec->TextEntries) {
						QCol->Doing("-> Text", TD.first);
						std::string Storage{ "Store" };
						if (TD.second.size() > 2048) Storage = "zlib";
						JO->AddString(TD.second, "TxtData/" + TD.first, Storage, "Scyndi's Creative Interpreter", "Save game data");
					}
					break;
				case SGJCR_Types::BinData:
					for (auto& TD : Rec->Banks) {
						QCol->Doing("-> Binary", TD.first);
						std::string Storage{ "Store" };
						if (TD.second->Size() > 2048) Storage = "zlib";
						JO->AddBank(TD.second, "BinData/" + TD.first, Storage, "Scyndi's Creative Interpreter", "Save game data");
					}
					break;
				case SGJCR_Types::RPGStat:
					for (auto& TS : Rec->PartyEntries) {
						QCol->Doing("-> Statistician", TS.first, " "); QCol->LMagenta("(" + TS.second + ")\n");
						auto P = JSG_GetParty(TS.first);
						StatSaveJCR6(P, JO, "Statistician." + TS.second,"zlib"); //,"zlib");
					}
					//luaL_error(L, "RPG Stat Data is not yet supported! Please come back later for that one!");
					return 0;
				default:
					luaL_error(L, "Unknown SGJCR type (%d)", (uint32)IJD.first);
					return 0;
				}
			}
		}
		QCol->Doing("Finalizing", Rec->JCRFile);
		JO->Close();
		QCol->Doing("Done", "Game has been saved");
		return 0;
	}

	static int API_SGJCRLoad(lua_State* L) {
		AutoRec;
		auto Identified{ false };
		if (!Rec->JCRFile.size()) { luaL_error(L, "L:No file assigned to savegame record!"); return 0; }
		QCol->Doing("Analysing", Rec->JCRFile);
		auto JI{ JCR6_Dir(SaveGameDir() + "/" + Rec->JCRFile) };
		if (Last()->Error) { luaL_error(L, "Error analysing savegame: %s", Last()->ErrorMessage.c_str()); return 0; }
		map<SGJCR_Types, bool>ToDo{};
		for (auto F : JI->_Entries) {
			auto Slash{ FindFirst(F.first,'/') };
			if (Slash > 0) {
				auto Dir{ F.first.substr(0,Slash) };
				if (Nm2En.count(Dir)) ToDo[Nm2En[Dir]] = true;
				ToDo[SGJCR_Types::RPGStat] = ToDo[SGJCR_Types::RPGStat] || Prefixed(Dir, "STATISTICIAN.");
			}
		}
		for (auto& IJT : ToDo) {
			printf("Loading game. Stage: %d\n", (int)IJT.first);
			switch (IJT.first) {
			case SGJCR_Types::ID: {
				QCol->Doing("-> ID", "Identify.ini");
				auto C{ JI->GetString("ID/Identify.ini") };
				auto G{ ParseUGINIE(C) };
				if (G->Value("ID", "Engine") != "SCI") { luaL_error(L, "Savegame engine fault"); return 0; }
				if (G->Value("ID", "GameID") != SaveGameID()) { luaL_error(L, "Savegame appears to be for a different game"); return 0; }
				Identified = true;
			} break;
			case SGJCR_Types::TxtData:
				for (auto F : JI->_Entries) {
					if (ExtractDir(F.first) == "TXTDATA") {
						QCol->Doing("-> Text", F.second->Name());
						Rec->TextEntries[StripDir(F.first)] = JI->GetString(F.first);
					}
				}
				break;
			case SGJCR_Types::BinData:
				for (auto F : JI->_Entries) {
					if (ExtractDir(F.first) == "BINDATA") {
						QCol->Doing("-> Binary", F.second->Name());
						Rec->Banks[F.first] = JI->B(F.first);
					}
				}
				break;
			case SGJCR_Types::RPGStat:
				//luaL_error(L, "RPGStat data loading is not yet implemented in this version");
				//return 0;
				for (auto& TS : Rec->PartyEntries) {
					QCol->Doing("-> Statistician", TS.first, " "); QCol->LMagenta("(" + TS.second + ")\n");
					//auto P = JSG_GetParty(TS.first);
					//StatSaveJCR6(P, JO, "Statistician." + TS.second);
					auto P = JCR6_LoadParty(JI, "Statistician." + TS.second);
					JSG_SetParty(TS.first,P);
				}
			default:
				break;
			}
		}
		if (!Identified) {
			luaL_error(L, "Savegame could not be identified");
			return 0;
		}
		return 0;
	}

	static int API_SGJCRClear(lua_State* L) {
		AutoRec;
		Rec->Banks.clear();
		Rec->TextEntries.clear();
		Rec->DoType.clear();
		return 0;
	}

	static int API_SGJCRKill(lua_State* L) {
		auto Rec{ Upper(Lunatic_CheckString(L,1)) };
		if (_SGRec::Register.count(Rec)) _SGRec::Register.erase(Rec);
		return 0;
	}

	static int API_SGExists(lua_State* L) {
		//cout << _JT_Dir::Recognize(SaveGameDir() + Lunatic_CheckString(L, 1)) << (_JT_Dir::Recognize(SaveGameDir() + Lunatic_CheckString(L, 1)) != "NONE") << endl;
		lua_pushboolean(L, _JT_Dir::Recognize(SaveGameDir() + "/" + Lunatic_CheckString(L, 1)) != "NONE");
		return 1;
	}

	static int API_HasText(lua_State* L) {
		AutoRec;
		lua_pushboolean(L, Rec->TextEntries.count(Upper(luaL_checkstring(L, 2))));
		return 1;
	}

	static int API_HasBin(lua_State* L) {
		AutoRec;
		lua_pushboolean(L, Rec->Banks.count(Upper(luaL_checkstring(L, 2))));
		return 1;
	}


	void Init_API_SGJCR(){
		std::map<std::string, lua_CFunction>IAPI{
			{ "Create", API_SGJCRCreate },
			{ "Assign",API_SGJCRAssign },
			{ "Mutilate",API_SGJCRMutilate },
			{ "GetText",API_SGJCRGetText },
			{ "SetText",API_SGJCRSetText },
			{ "GetBank",API_SGJCRGetBank },
			{ "SetBank",API_SGJCRSetBank },
			{ "SetStat",API_SGJCRSetStat },
			{ "GetStat",API_SGJCRGetStat },
			{ "DoType",API_SGJCRDoType },
			{ "Save",API_SGJCRSave },
			{ "Load",API_SGJCRLoad },
			{ "Clear",API_SGJCRClear },
			{ "Exists",API_SGExists },
			{ "HasText",API_HasText },
			{ "HasBin",API_HasBin }
		};
		InstallAPI("SGJCR", IAPI);
	}
}
