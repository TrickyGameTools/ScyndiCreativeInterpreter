// Lic:
// Scyndi's Creative Interpreter
// Crash Handler (header)
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
#pragma once
#include <Slyvina.hpp>


namespace Scyndi_CI {
	bool Crashed();

	void Crash(std::string ErrorMessage, Slyvina::VecString VerdereUitleg = nullptr, bool dothrow = true);
	void Crash(std::string ErrorMessage, std::string VerdereUitleg, bool dothrow = true);
}