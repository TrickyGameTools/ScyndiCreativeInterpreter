// Lic:
// Scyndi's Creative Interpreter
// Graphics Manager
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

#include <SlyvString.hpp>
#include <SlyvQCol.hpp>

#include <TQSG.hpp>

#include "SCI_Crash.hpp"
#include "SCI_Graphics.hpp"
#include "SCI_Config.hpp"
#include "SCI_JCR.hpp"

using namespace Slyvina;
using namespace Units;
using namespace TQSG;
using namespace JCR6;

namespace Scyndi_CI {
	static map<std::string, TImage> ImageRegister{};
	static map<std::string, TImageFont> FontRegister{};

	bool WantFullScreen() {
		return Upper(GameID_GINIE()->Value("Window", "Full")) == "TRUE";
	}

	std::string WindowCaption() { return GameID_GINIE()->Value("Window", "Caption"); }

	TImage Img(std::string Tag, bool crash) {
		Trans2Upper(Tag);
		if (!ImageRegister.count(Tag)) {
			if (crash) { throw std::runtime_error(TrSPrintF("There is no image tagged '%s'", Tag.c_str())); }
			return nullptr;
		}
		return ImageRegister[Tag];
	}

	void Img(std::string Tag, Slyvina::TQSG::TImage _Img) {
		Trans2Upper(Tag);
		if (!_Img) {
			if (!ImageRegister.count(Tag)) return;
			ImageRegister.erase(Tag);
		} else {
			ImageRegister[Tag] = _Img;
		}
	}

	void Img(std::string Tag, Slyvina::JCR6::JT_Dir J, std::string File) {
		if (!J->EntryExists(File)) {
			Crash("Image name '" + File + "' (for tag '" + Tag + "') not found");
			return;
		}
		Trans2Upper(Tag);
		Img(Tag, LoadImage(J, File));
		if (Last()->Error) Crash("JCR6 error: " + Last()->ErrorMessage, "MainFile: " + Last()->MainFile + "\nEntry: " + Last()->Entry);
	}

	void Img(std::string Tag, std::string File) { Img(Tag, Resource(), File); }

	Slyvina::TQSG::TImageFont Fnt(std::string Tag, bool crash) {
		Trans2Upper(Tag);
		if (!FontRegister.count(Tag)) {
			if (crash) { throw std::runtime_error(TrSPrintF("There is no font tagged '%s'", Tag.c_str())); }
			return nullptr;
		}
		return FontRegister[Tag];
	}

	void Fnt(std::string Tag, Slyvina::TQSG::TImageFont _Fnt) {
		Trans2Upper(Tag);
		if (!_Fnt) {
			if (!FontRegister.count(Tag)) return;
			FontRegister.erase(Tag);
		} else {
			FontRegister[Tag] = _Fnt;
		}
	}

	void Fnt(std::string Tag, Slyvina::JCR6::JT_Dir J, std::string File) {
		if (!J->DirectoryExists(File)) {
			Crash("Font name '" + File + "' (for tag '" + Tag + "') not found");
			return;
		}
		Trans2Upper(Tag);
		Fnt(Tag, LoadImageFont(J,File));
		if (Last()->Error) Crash("JCR6 error: " + Last()->ErrorMessage, "MainFile: " + Last()->MainFile + "\nEntry: " + Last()->Entry);
	}

	void Fnt(std::string Tag, std::string File) { Fnt(Tag, Resource(), File); }

	void StartGraphics() {
		if (WantFullScreen()) {
			QCol->Doing("Entering", "FullScreen");
			QCol->Doing("Caption", WindowCaption());
			Graphics(WindowCaption());
		} else {
			QCol->Error("Windowed mode not yet implemented");
			exit(500);
		}
		Img("*SCIPOWER", SRF(), "GFX/PoweredBySCI.png");
		Img("*SCIPOWER")->HotCenter();
		Fnt("*SYSFONT", SRF(), "Font/DosFont.jfbf");
		Cls();
		Img("*SCIPOWER")->Draw(ScreenWidth() / 2, ScreenHeight() / 2);
		Flip();
		Img("*DEATH", SRF(), "GFX/Death.png");
		// SDL_Delay(4000); // debug
	}

}