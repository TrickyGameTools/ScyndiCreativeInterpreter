// Lic:
// Scyndi's Creative Interpreter
// API install header
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
// Version: 23.01.18
// EndLic
namespace Scyndi_CI{

	void Init_API_Events();
	void Init_API_Graphics();
	void Init_API_InterState();
	void Init_API_JCR();
	void Init_API_SGFile();
	void Init_API_Vars();
	void Init_SCI_String();


	inline void InstallAllAPIS() {
		Init_API_Events();
		Init_API_Graphics();
		Init_API_InterState();
		Init_API_JCR();
		Init_API_SGFile();
		Init_API_Vars();
		Init_SCI_String();
	}


}