// License:
// 
// Scyndi's Creative Interpreter
// API JCR6 cross-API linkup
// 
// 
// 
// 	(c) Jeroen P. Broks, 2026
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
// Version: 26.10.23
// End License
#ifndef API_JCR6_HPP
#define API_JCR6_HPP

#include <JCR6_Core.hpp>

namespace Scyndi_CI {
    Slyvina::JCR6::JT_Dir GetFromReg(std::string Tag);
}
#endif // API_JCR6_HPP
