// License:
// 
// Scyndi's Creative Interpreter - Builder
// Script Manager (header)
// 
// 
// 
// 	(c) Jeroen P. Broks, 2023, 2025
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
// Version: 25.03.05
// End License

#pragma once
#include <SlyvGINIE.hpp>

#include "../SCI_Share/SCI_GlobalConfig.hpp"
#include "SCI_Project.hpp"

namespace Scyndi_CI {
	namespace Builder {

		std::string Scyndi();

		/// <summary>
		/// Handles the scripts with the Scyndi compiler (needed as dependency).
		/// </summary>
		/// <param name="Prj"></param>
		/// <returns>true if succesful, false if Scyndi detected any failures</returns>
		bool CompileScripts(SCI_Project* Prj, Slyvina::Units::GINIE PrjData);
	}
}
