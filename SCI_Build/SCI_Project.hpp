// Lic:
// Scyndi's Creative Interpreter - Builder
// Project Manager (header)
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

#pragma once
#include <string>
#include <SlyvGINIE.hpp>
#include <SlyvTime.hpp>

//namespace Scyndi_CI { namespace Builder { class SCI_Project; } }
//#include "SCI_Package.hpp"
//#define SCI_PROJECT_LOADED

namespace Scyndi_CI {
	namespace Builder {



		class SCI_Project {
		private:

			//Package Pack;

			Slyvina::Units::GINIE Data{ nullptr };
			Slyvina::Units::GINIE ID{ Slyvina::Units::ParseGINIE("[ID]\nBuild=" + Slyvina::Units::CurrentDate()) };
			std::string Project{ "" };

			bool CreateIfNeeded();

			bool WindowSettings();

		public:

			std::string Title();
			std::string Author();
			std::string Copyright();
			std::string License();
			std::string File();


			std::string OutputName();
			std::string DebugJQLFile();

			std::string AssetsDir();
			bool AssetsMultiDir();
			

			static Slyvina::uint64 Processed;
			static Slyvina::uint64 Success;
			static Slyvina::uint64 Fail;
			
			inline SCI_Project() {}; // Fuck it! C++ want it to exist for no reason!
			SCI_Project(std::string _Project);
		};




		// Please note, this lives in SCI_Package.cpp
		// Placed here as a conflict preventer
		bool HandlePackage(SCI_Project* P, Slyvina::Units::GINIE G);
	}
}