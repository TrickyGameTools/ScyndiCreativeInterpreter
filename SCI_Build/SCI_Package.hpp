// License:
// 
// Scyndi's Creative Interpreter - Builder
// Package (header)
// 
// 
// 
// 	(c) Jeroen P. Broks, 2023, 2024
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
// Version: 24.11.16
// End License
// Lic:
// Scyndi's Creative Interpreter - Builder
// Package (header)
// 
// 
// 
// (c) Jeroen P. Broks, 2023, 2024
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
// Version: 24.03.16
// EndLic
#pragma once
#include <memory>
#include <string>


#include <JCR6_Core.hpp>

//#ifndef SCI_PROJECT_LOADED
//class SCI_Project;
#include "SCI_Project.hpp"
//#endif

namespace Scyndi_CI {
	namespace Builder {
		class _Package; typedef std::shared_ptr<_Package> Package;
		
		struct PackResult;

		class _Package {
		private:

			static std::map<std::string, std::string> ReservedDirs;

			Slyvina::Units::GINIE PrjData{ nullptr };
			SCI_Project* _Parent{ nullptr };

			std::map<std::string, Slyvina::JCR6::JT_Create> Packages;
			std::map<std::string, Slyvina::JCR6::JT_CreateBlock> Blocks;
			bool _debug{ false };
			std::string OutputJQL;
			Slyvina::JCR6::JT_Create MainPackage;
			Slyvina::JCR6::JT_Create GetPackage(std::string pkg);
			Slyvina::JCR6::JT_CreateBlock GetBlock(std::string pkg, std::string dir);
			

			std::string Package(std::string dir);

			bool EligableScriptBundle(std::string bundle);
			bool PackDir(std::string srcdir,Slyvina::VecString D, std::string Package, std::string Author, std::string Notes);
			PackResult PackScript(std::string bundle, bool bytecode, std::string entry, std::string package, std::string directory);			

		public:

			bool PackScript();
			bool AddString(std::string package, std::string entry, std::string content,std::string author,std::string notes);
			bool AddMedals(std::string package);
			bool Pack();
			bool Aliases();
			_Package(SCI_Project*P,Slyvina::Units::GINIE D);
			~_Package();

		};
	}
}
