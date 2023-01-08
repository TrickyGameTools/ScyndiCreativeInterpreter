#include <SlyvStream.hpp>
#include <SlyvGINIE.hpp>
#include <SlyvDirry.hpp>
#include <SlyvQCol.hpp>
#include <SlyvTime.hpp>

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
}

