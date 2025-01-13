// License:
//
// Scyndi's Creative Interpreter
// Graphics Manager
//
//
//
// 	(c) Jeroen P. Broks, 2023, 2024
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
// Version: 24.11.16
// End License


// Debug tag! Should be off in release
#define SCI_NOFULL

#include <SlyvString.hpp>
#include <SlyvQCol.hpp>
#include <SlyvSTOI.hpp>

#include <Lunatic.hpp>

#include <TQSG.hpp>

#include "SCI_Crash.hpp"
#include "SCI_Graphics.hpp"
#include "SCI_Config.hpp"
#include "SCI_JCR.hpp"
#include "SCI_Script.hpp"
#include <SlyvMD5.hpp>

using namespace Slyvina;
using namespace Units;
using namespace TQSG;
using namespace JCR6;
using namespace std;

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
		if (!ImageRegister[Tag]) QCol->Warn("Image Tag '"+Tag+"' turns out to contain a null pointer!");
		if (!ImageRegister[Tag]->Valid()) QCol->Warn("Image Tag "+Tag+" turns out to be invalid!");
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
		if ((!J->EntryExists(File)) && (!J->DirectoryExists(File))) {
			Crash("Image name '" + File + "' (for tag '" + Tag + "') not found");
			return;
		}
		Trans2Upper(Tag);
		Img(Tag, LoadImage(J, File));
		if (Last()->Error) Crash("JCR6 error: " + Last()->ErrorMessage, "MainFile: " + Last()->MainFile + "\nEntry: " + Last()->Entry);
	}

	void Img(std::string Tag, std::string File) { Img(Tag, Resource(), File); }

	bool HasImg(std::string Tag) {
		return ImageRegister.count(Upper(Tag));
	}

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

	bool HasFnt(std::string Tag) {
		Trans2Upper(Tag);
		return FontRegister.count(Tag);
	}

	bool UseAlt() {
		return Upper(GameID_GINIE()->Value("Alt", "Use")) == "TRUE";
	}

	int AltWidth(bool force) {
		if (force || UseAlt())
			return GameID_GINIE()->IntValue("Alt", "Width");
		return 0;
	}

	int AltHeight(bool force) {
		if (force || UseAlt())
			return GameID_GINIE()->IntValue("Alt", "Height");
		return 0;
	}

	static int WV(string v, int dv) {
		if (Suffixed(v, "%")) {
			int vi{ ToInt(Left(v,v.size()-1)) };
			double vd{ (double)vi };
			double pd{ vd / 100.0 };
			double sz{ dv * pd };
			return (int)floor(sz);
		}
		return ToInt(v);
	}


#pragma region AltPic for scripted pictures
	class SCIAP {
	private:
	public:
		_____TIMAGE* TiedToPic{ nullptr };
		int W{ 0 }, H{ 0 };
		string ScriptTag{ "" };
		Slyvina::NSLunatic::SLunatic APState;
	};
	static std::map<uint64, SCIAP> SCIAPReg{};
	static int SCIAP_W(_____TIMAGE* img) { return SCIAPReg[img->ID()].W; }
	static int SCIAP_H(_____TIMAGE* img) { return SCIAPReg[img->ID()].H; }
	static void SCIAP_Draw(_____TIMAGE* img, int x, int y, int f) {
		auto APD{ &SCIAPReg[img->ID()] };
		auto APDS{ APD->APState };
		if (!APDS) { Crash("Alt image null state!", TrSPrintF("SCIAP driver has to call a script on ID #%d, but no state seems to be present (", img->ID()) + APD->ScriptTag + ")"); return; }
		APDS->QDoString(TrSPrintF("Scyndi.Globals.DrawAltImage(%d,%d,%d,%d)", img->ID(), x, y, f));
	}
	static void SCIAP_Destroy(_____TIMAGE* img) {
		KillState(SCIAPReg[img->ID()].ScriptTag);
	}
	static void SCIAP_Load(_____TIMAGE* img, JT_Dir J, String Ent) {
		GINIE GD{};
		String GDF = StripExt(Ent) + ".ini";
		GD = ParseGINIE(J->EntryExists(GDF) ? J->GetString(GDF) : "[sizes]\nW=256\nH=256\n[frames]\nframes=1\n");
		SCIAPReg[img->ID()] = SCIAP();
		auto NR{ &SCIAPReg[img->ID()] };
		NR->W = GD->IntValue("Sizes", "W");
		NR->H = GD->IntValue("Sizes", "H");
		NR->ScriptTag= "ALTPICSCRIPT" + ChReplace(Ent, '/', '_') + md5(Ent) + TrSPrintF("_%09x", img->ID());
		NR->TiedToPic = img;
		State(NR->ScriptTag, Ent);
		NR->APState = State(NR->ScriptTag);
	}
	static void SCIAP_GetFormat(_____TIMAGE* img, int* w, int* h) {
		auto APD{ &SCIAPReg[img->ID()] };
		if (!APD) { Crash("GetFormat::Null AltPic Image Error"); return; }
		*w = APD->W;
		*h = APD->H;
	}
	static size_t SCIAP_Frames(_____TIMAGE*) { return 1; }
	static void SCIAP_Init() {
		QCol->Doing("Init", "Scripted pictures");
		auto IAP{ TQAltPic::Create() };
		IAP->Destroy = SCIAP_Destroy;
		IAP->Draw = SCIAP_Draw;
		IAP->Ext("lua;luac;lbc");
		IAP->Height = SCIAP_H;
		IAP->Width = SCIAP_W;
		IAP->LoadJCR6 = SCIAP_Load;
		IAP->Frames = SCIAP_Frames;
		IAP->GetFormat = SCIAP_GetFormat;
		TQAltPic::ReIndex(false);


	}

#pragma endregion

	void StartGraphics() {
		if (WantFullScreen()) {
			QCol->Doing("Entering", "FullScreen");
			QCol->Doing("Caption", WindowCaption());
#ifdef SCI_NOFULL
			QCol->Warn("DEBUG lockout on fullscreen.");
			int
				snfw{ (int)floor(DesktopWidth() * .99) },
				snfh{ (int)floor(DesktopHeight() * .94) };
			QCol->Doing("Desktop", TrSPrintF("%04dx%04d", DesktopWidth(), DesktopHeight()));
			QCol->Doing("DebugWin", TrSPrintF("%04dx%04d", snfw, snfh));
			Graphics(snfw, snfh, WindowCaption());
#else
			Graphics(WindowCaption());
#endif
		} else {
			auto
				WS{ GameID_GINIE()->Value("Window","Width") },
				HS{ GameID_GINIE()->Value("Window","Height") };
			QCol->Doing("Measuring","Desktop size");
			auto
				WI{ WV(WS,DesktopWidth(true)) },
				HI{ WV(HS,DesktopHeight(true)) };
			QCol->Doing("Desktop", TrSPrintF("%dx%d", WI, HI));
			//cout << AltWidth() << "/" << AltHeight() << "/" << Upper(Left(GameID_GINIE()->Value("Window", "WINADEPTALT"), 1)) << (Upper(Left(GameID_GINIE()->Value("Window", "WINADEPTALT"), 1)) == "Y") << "\n"; // debug
			if (AltWidth() && AltHeight() && Upper(Left(GameID_GINIE()->Value("Window", "WINADEPTALT"), 1)) == "Y") {
				//double WR{ (double)AltWidth() / (double)WI };
				//double HR{ (double)AltHeight() / (double)HI };
				double WR{ (double)WI / (double)AltWidth() };
				double HR{ (double)HI / (double)AltHeight() };
				QCol->Doing("AutoAltRates", TrSPrintF("%f x %f (%dx%d) -> (%dx%d)", WR, HR, WI, HI, AltWidth(), AltHeight()));
				if (WR < HR) {
					QCol->Doing("AutoAlt", "Height adept to width");
					HI = (int)floor((double)AltHeight() * WR);
				} else if (WR >  HR) {
					QCol->Doing("AutoAlt", "Width adept to height");
					WI = (int)floor((double)AltWidth() * HR);
				}
			}
			QCol->Doing("Graphics", TrSPrintF("%dx%d", WI, HI));
			Graphics(WI, HI, WindowCaption());
			//QCol->Error("Windowed mode not yet implemented");
			//exit(500);
		}
		SCIAP_Init();
		InitJCRPaniek();
		Img("*SCIPOWER", SRF(), "GFX/PoweredBySCI.png");
		Img("*SCIPOWER")->HotCenter();
		Fnt("*SYSFONT", SRF(), "Font/DosFont.jfbf");
		Cls();
		SetColor(0, 0, 105); Rect(0, 0, ScreenWidth(), ScreenHeight()); SetColor(255, 255, 255);
		Img("*SCIPOWER")->Draw(ScreenWidth() / 2, ScreenHeight() / 2);
		Flip();
		Img("*DEATH", SRF(), "GFX/Death.png");
		if (AltWidth() && AltHeight()) {
			QCol->Doing("AltScreen", TrSPrintF("%05dx%05d", AltWidth(), AltHeight()));
			SetAltScreen(AltWidth(), AltHeight());
		}
		if (HideSystemMouse()) SDL_ShowCursor(0);
		// SDL_Delay(4000); // debug
	}

}
