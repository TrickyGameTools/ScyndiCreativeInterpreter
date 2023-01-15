// Lic:
// Scyndi's Creative Interpreter
// Config (header)
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
// Version: 23.01.15
// EndLic

#pragma once
#include <SlyvArgParse.hpp>
#include <SlyvGINIE.hpp>



namespace Scyndi_CI {

	enum class RunType { Unknown, Straight, Flow, Callback };

	extern Slyvina::Units::ParsedArg Args;

	std::string JCR_MainFile();
	std::string GameGlobalHome();
	std::string SaveGameDir(); // Savegame dir generated on GameGlobalHome and the game's ID

#pragma region SRF
	std::string JCR_SRF();
	std::string SRF_Date();
#pragma endregion

#pragma region "Game ID"
	Slyvina::Units::GINIE GameID_GINIE();
	std::string GameTitle();
	std::string GameAuthor();
	std::string GameCopyright();

	bool VersionMatch(bool ask = false);

	bool HideSystemMouse();
#pragma endregion

	RunType GetRunType();
}