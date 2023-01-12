// Lic:
// Scyndi's Creative Interpreter
// Crash handler
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
#include <TQSG.hpp>
#include <TQSE.hpp>
#include <SlyvString.hpp>
#include <SlyvQCol.hpp>

#include "SCI_Crash.hpp"
#include "SCI_Graphics.hpp"

using namespace Slyvina;
using namespace Units;
using namespace TQSG;
using namespace TQSE;

namespace Scyndi_CI {

	static bool _Crashed{ false };
	bool Crashed() {
		return _Crashed;
	}
	void Crash(std::string ErrorMessage, Slyvina::VecString VerdereUitleg, bool dothrow) {
		_Crashed = true;
		QCol->Error(ErrorMessage);
		if (VerdereUitleg) {
			for (auto ul : *VerdereUitleg) QCol->Magenta(ul + "\n");
		}
		Cls();
		SetColor(0, 0, 127, 255);
		Rect(0, 0, ScreenWidth(), ScreenHeight());
		auto death = Img("*DEATH", false);
		if (death) {
			SetColor(0, 0, 255);
			auto y = std::max(0,ScreenHeight() - death->Height());
			death->Draw(0, y);
		}
		Flip();
		Flush();
		do { Poll(); } while (!KeyHit(SDLK_ESCAPE));

		
		if (dothrow) throw std::runtime_error("A fatal error occurred");
	}

	void Crash(std::string ErrorMessage, std::string VerdereUitleg, bool dothrow) { Crash(ErrorMessage, Split(VerdereUitleg, '\n'), dothrow); }
}