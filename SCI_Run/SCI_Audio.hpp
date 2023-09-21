// Lic:
// Scyndi's Creative Interpreter
// Audio Manager (header)
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
// Version: 23.09.21
// EndLic
#pragma once
#include <TQSA.hpp>


namespace Scyndi_CI {

	Slyvina::TQSA::TAudio Aud(std::string Tag, bool crash = true);
	void Aud(std::string Tag, Slyvina::TQSA::TAudio AU);
	void Aud(std::string Tag, Slyvina::JCR6::JT_Dir J, std::string File);
	void Aud(std::string Tag, std::string File);
	bool HasAud(std::string Tag);

	int PlayAud(std::string Tag);
	int PlayAud(std::string Tag, int channel);

	int LoopAud(std::string Tag, int loops = -1);
	int LoopAud(std::string Tag, int loops, int channel);

	void KillAud(std::string Tag);

	
}