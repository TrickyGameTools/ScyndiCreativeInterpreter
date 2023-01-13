// Lic:
// Scyndi's Creative Interpreter
// Script & State Manager (header)
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
// Version: 23.01.13
// EndLic

#pragma once

#include <Lunatic.hpp>

#include <string>

namespace Scyndi_CI {
	std::string MainScript();

	bool GotState(std::string _State);

	Slyvina::Lunatic::SLunatic State(std::string _State);
	void State(std::string _State, Slyvina::Lunatic::SLunatic _Lun);
	void State(std::string _Stage, Slyvina::JCR6::JT_Dir _Res, std::string _Entry);
	
}