// Lic:
// Scyndi's Creative Interpreter - Shared
// Global Configuration (accessed by both runtime as builder)
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
// Version: 23.01.15
// EndLic
#include <SlyvStream.hpp>
#include <SlyvGINIE.hpp>
#include <SlyvDirry.hpp>
#include <SlyvQCol.hpp>
#include <SlyvTime.hpp>
#include <SlyvVolumes.hpp>

#include "SCI_GlobalConfig.hpp"

using namespace Slyvina::Units;

namespace Scyndi_CI {
	std::string SCIHome() { 
		auto r = Dirry("$AppSupport$/ScyndiCreativeInterpreter/");
		if (!DirectoryExists(r)) {
			QCol->Doing("Creating", r);
			MakeDir(r);
		}
		return r;
	}
	std::string GlobalConfigFile() { return SCIHome() + "GlobalConfig.ini"; }

	GINIE GlobalConfig() {
		static GINIE ret{ nullptr };
		if (!ret) {
			if (!FileExists(GlobalConfigFile())) {
				QCol->Doing("Creating", GlobalConfigFile());
				SaveString(GlobalConfigFile(), TrSPrintF("[Global]\nCreated=%s; %s\n[RunTime]\nHome=%sGames/\n", CurrentDate().c_str(), CurrentTime().c_str(), SCIHome().c_str()));
			}
			QCol->Doing("Reading", GlobalConfigFile());
			ret = LoadGINIE(GlobalConfigFile(), GlobalConfigFile(), "Scyndi's Creative Interpreter\nGlobal Configuration file\n(c) Jeroen P. Broks");
		}
		return ret;
	}

	std::string GeneralSaveGameDir() {
		std::string ret{ "" };
		if (!GlobalConfig()->HasValue("Directory", "GlobSave"))
			ret = SCIHome();
		else
			ret = AVolPath(GlobalConfig()->Value("Directory", "GlobSave"));
		if (!DirectoryExists(ret)) {
			QCol->Doing("Creating", ret);
		}
		return ret;
	}


}