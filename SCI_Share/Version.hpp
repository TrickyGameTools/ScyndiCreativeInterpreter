// License:
// 
// Scyndi's Creative Interpreter
// Version Data
// 
// 
// 
// 	(c) Jeroen P. Broks, 2023, 2024, 2025
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
// Version: 25.06.21
// End License
#pragma once
#include <string>

// This code is generated by QVersion! It's best NOT to manutally alter it!

namespace Scyndi_CI {
	class QVersionClass {
	public:
		unsigned int giant{0}, major{0}, minor{0}; 
		std::string dt{"00000000000000"};
		std::string Status{"Stable"};
		inline std::string Version(bool full=true) {
			std::string ret{ std::to_string(giant)+"."+std::to_string(major)+"."+std::to_string(minor) };
			if (full) ret+="."+dt;
			if (Status!="Stable") ret += " (" + Status + ")";
			return ret;
		}
		inline QVersionClass(){} // Fuck you!
		inline QVersionClass(unsigned int _giant,unsigned int _major,unsigned int _minor,std::string _dt="00000000000000",std::string _status="Stable") {
			giant=_giant; major=_major; minor=_minor; dt=_dt; Status=_status;
		}
	};

	static QVersionClass QVersion{1,7,10,"20250619135752","Alpha"};

} // namespace Scyndi_CI
