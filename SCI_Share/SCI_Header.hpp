// License:
// 
// Scyndi's Creative Interpreter
// Header
// 
// 
// 
// 	(c) Jeroen P. Broks, 2023, 2025, 2026
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
// Version: 26.02.11
// End License

#include <string>
#include <Slyvina.hpp>
#include <SlyvQCol.hpp>

#include "Version.hpp"

namespace Scyndi_CI {

	inline void Header(std::string Name) {
		using namespace Slyvina::Units;
		QCol->Green(Name + "\n");
		QCol->Doing("Version", QVersion.Version());
		QCol->Doing("Platform",Slyvina::Platform());
		QCol->Doing("Coded by", "Jeroen P. Broks");
		QCol->Doing("License", "General Public License 3");
		QCol->Magenta("(c) Jeroen P. Broks, 2023, 2024, 2025\n\n");
		QCol->Reset();
	}
}
