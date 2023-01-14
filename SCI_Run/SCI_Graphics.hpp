// Lic:
// Scyndi's Creative Interpreter
// Graphics Manager (header)
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
#include <string>

#include <TQSG.hpp>

namespace Scyndi_CI {

	bool WantFullScreen();
	std::string WindowCaption();

	Slyvina::TQSG::TImage Img(std::string Tag, bool crash = true);
	void Img(std::string Tag, Slyvina::TQSG::TImage _Img);
	void Img(std::string Tag, Slyvina::JCR6::JT_Dir J, std::string File);
	void Img(std::string Tag, std::string File);

	Slyvina::TQSG::TImageFont Fnt(std::string Tag, bool crash = true);
	void Fnt(std::string Tag, Slyvina::TQSG::TImageFont _Fnt);
	void Fnt(std::string Tag, Slyvina::JCR6::JT_Dir J, std::string File);
	void Fnt(std::string Tag, std::string File);

	bool UseAlt();
	int AltWidth(bool force = false);
	int AltHeight(bool force = false);

	void StartGraphics();
}