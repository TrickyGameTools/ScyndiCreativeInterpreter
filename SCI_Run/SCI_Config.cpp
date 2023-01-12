// Lic:
// Scyndi's Creative Interpreter
// Configuration
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
// Version: 23.01.12
// EndLic

#include <SlyvArgParse.hpp>
#include <SlyvString.hpp>
#include <SlyvQCol.hpp>
#include <SlyvGINIE.hpp>

#include <JCR6_Core.hpp>

#include "../SCI_Share/SCI_GlobalConfig.hpp"
#include "SCI_Config.hpp"
#include "SCI_JCR.hpp"

using namespace Slyvina;
using namespace Units;
using namespace JCR6;

namespace Scyndi_CI {
	ParsedArg Args{};

	static GINIE srf_id{ nullptr };
	
	std::string JCR_MainFile() {
		if (Args.arguments.size()) return Args.arguments[0];
		return StripExt(Args.myexe) + ".jcr";
	}

	std::string GameGlobalHome() {
		auto ret = GlobalConfig()->Value("Runtime", "Home");
		if (!DirectoryExists(ret)) {
			QCol->Doing("Creating", ret);
			MakeDir(ret);
		}
		return ret;
	}

	std::string JCR_SRF() {	return ChReplace(StripExt(Args.myexe) + ".srf",'\\','/'); }

	static void LoadSRFGINIE() {
		if (!srf_id) {
			auto src = SRF()->GetString("ID/Identify.ini");
			if (Last()->Error) { QCol->Error("JCR Error in getting SRF identification: " + Last()->ErrorMessage); exit(1); }
			QCol->Doing("Parsing", "SRF Identification");
			srf_id = ParseGINIE(src); 

		}		
	}

	std::string SRF_Date() {
		LoadSRFGINIE();
		return srf_id->Value("SRF", "Build");
	}
	
	

}