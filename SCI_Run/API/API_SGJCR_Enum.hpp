// Lic:
// Scyndi's Creative Interpreter
// JCR Savegame - Enums and stuff (Generated by a Python Script)
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
// Version: 23.01.20
// EndLic
// Generated header

#include <map>
#include <string>

namespace Scyndi_CI {

	enum class SGJCR_Types { ID, TxtData, BinData, RPGStat };

	static std::map<SGJCR_Types, std::string> En2Nm {
		{SGJCR_Types::ID,"ID"},
		{SGJCR_Types::TxtData,"TxtData"},
		{SGJCR_Types::BinData,"BinData"},
		{SGJCR_Types::RPGStat,"RPGStat"}
	};

	static std::map<std::string, SGJCR_Types> Nm2En {
		{"ID",SGJCR_Types::ID},
		{"TxtData",SGJCR_Types::TxtData},
		{"BinData",SGJCR_Types::BinData},
		{"RPGStat",SGJCR_Types::RPGStat}
	};