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
// Version: 23.01.13
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
		SetAltScreen(0, 0);
		Cls();
		SetColor(0, 0, 127, 255);
		Rect(0, 0, ScreenWidth(), ScreenHeight());
		auto death = Img("*DEATH", false);
		if (death) {
			SetColor(0, 0, 255);
			auto y = std::max(0,ScreenHeight() - death->Height());
			death->Draw(0, y);
		}
		auto fnt = Fnt("*SYSFONT");
		SetColor(255, 100, 50);
		fnt->Text("Oops! You tried something I didn't think of!", 5, 5);
		SetColor(255, 255, 0);
		fnt->Text(ErrorMessage,5,30);
		SetColor(0, 180, 255);
		if (VerdereUitleg) {
			for (size_t i = 0; i < VerdereUitleg->size(); i++) {
				fnt->Text((*VerdereUitleg)[i], 5, 90 + (i * 30));
			}
		}
		SetColor(255, 255, 255);
		fnt->Text("Press Escape to exit this application!", 5, ScreenHeight() - 35, Align::Left);
		Flip();
		Flush();
		do { Poll(); } while (!KeyHit(SDLK_ESCAPE));

		
		if (dothrow) throw std::runtime_error("A fatal error occurred");
	}

	void Crash(std::string ErrorMessage, std::string VerdereUitleg, bool dothrow) { Crash(ErrorMessage, Split(VerdereUitleg, '\n'), dothrow); }
}