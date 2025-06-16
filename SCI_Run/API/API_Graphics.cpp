// License:
// 
// Scyndi's Creative Interpreter
// Graphics API
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
// Version: 25.06.15
// End License

#include <SlyvString.hpp>

#include "../SCI_Crash.hpp"
#include "../SCI_Script.hpp"
#include "../SCI_Graphics.hpp"

using namespace Slyvina;
using namespace NSLunatic;
using namespace Units;
using namespace TQSG;

namespace Scyndi_CI {
	static int API_LoadImage(lua_State* L) {
		size_t Untagged{ 0 };
		auto
			File{ Lunatic_CheckString(L,1) },
			Tag{ Lunatic_OptString(L,2,"") };
		if (!Tag.size()) {
			Tag = TrSPrintF("***TAG:%08x",Untagged++);
		} else if (Prefixed(Tag, "*")) {
			Crash("Tags for LoadImage may not be prefixed with *");
			return 0;
		}
		Img(Tag, File);
		Lunatic_PushString(L, Tag);
		return 1;
	}

	static int API_Cls(lua_State* L) { Cls(); return 0; }

	static int API_Kill(lua_State* L) {
		auto Tag{ Lunatic_CheckString(L, 1) };
		if (Prefixed(Tag, "*") && (!Prefixed(Tag, "***"))) { Crash("Image tagged '" + Tag + "' is part of the engine and may not be killed"); return 0; }
		Img(Tag, (TImage)nullptr);
		return 0;
	}

	static int API_HasImageTag(lua_State* L) {
		lua_pushboolean(L, HasImg(luaL_checkstring(L, 1)));
		return 1;
	}

	static int API_DrawImage(lua_State* L) {
		auto
			Tag{ Lunatic_CheckString(L,1) };
		auto
			x{ luaL_checkinteger(L,2) },
			y{ luaL_checkinteger(L,3) },
			frame{ luaL_optinteger(L,4,0) };
		Img(Tag)->Draw(x, y, frame);
		return 0;
	}

	static int API_DrawRotatedImage(lua_State* L) {
		auto
			Tag{ Lunatic_CheckString(L,1) };
		auto
			x{ luaL_checkinteger(L,2) },
			y{ luaL_checkinteger(L,3) },
			deg{ luaL_checkinteger(L,4) },
			frame{ luaL_optinteger(L,5,0) };
		Rotate(deg);
		Img(Tag)->XDraw(x, y, frame);
		Rotate(0);
        return 0;
	}

	static int API_TileImage(lua_State* L) {
		auto
			Tag{ Lunatic_CheckString(L,1) };
		auto
			x{ luaL_checkinteger(L,2) },
			y{ luaL_checkinteger(L,3) },
			w{ luaL_checkinteger(L,4) },
			h{ luaL_checkinteger(L,5) },
			frame{ luaL_optinteger(L, 6, 0) },
			ix{luaL_optinteger(L,7,0)},
			iy{luaL_optinteger(L,8,0)};

		//std::cout << "DEBUG: TILE "	<< Tag << TrSPrintF(" (%d,%d) %dx%d F:%d",x,y,w,h,frame)<<std::endl;
		Img(Tag)->Tile(x, y, w, h, frame,ix,iy);
		return 0;
	}


	static int API_TrueDrawImage(lua_State* L) {
		auto
			Tag{ Lunatic_CheckString(L,1) };
		auto
			x{ luaL_checkinteger(L,2) },
			y{ luaL_checkinteger(L,3) },
			frame{ luaL_optinteger(L,4,0) };
		Img(Tag)->TrueDraw(x, y, frame);
		return 0;
	}

	static int API_HotCenter(lua_State* L) { Img(luaL_checkstring(L, 1))->HotCenter(); return 0; }

	static int API_Hot(lua_State*L) { Img(luaL_checkstring(L, 1))->Hot(luaL_checkinteger(L,2),luaL_checkinteger(L,3)); return 0; }

	static int API_Color(lua_State* L) {
		auto
			r = luaL_checkinteger(L, 1),
			g = luaL_checkinteger(L, 2),
			b = luaL_checkinteger(L, 3),
			a = luaL_optinteger(L, 4, -1);
		if (a < 0) {
			SetColor(r, g, b);
		} else {
			SetColor(r, g, b, a);
		}
		return 0;
	}

	static int API_ColorHSV(lua_State* L) {
		auto
			hue = luaL_checknumber(L, 1),
			saturation = luaL_checknumber(L, 2),
			value = luaL_checknumber(L, 3);
		auto
			alpha = luaL_optinteger(L, 4, -1);
		SetColorHSV(hue, saturation, value);
		if (alpha >= 0) SetAlpha(alpha);
		return 0;
	}

	static int API_SWidth(lua_State* L) {
		lua_pushinteger(L,ScreenWidth((bool)luaL_checkinteger(L, 1)));
		return 1;
	}

	static int API_SHeight(lua_State* L) {
		lua_pushinteger(L, ScreenHeight((bool)luaL_checkinteger(L, 1)));
		return 1;
	}

	static int API_ImgFmt(lua_State* L) {
		auto imgtag{ luaL_checkstring(L,1) };
		auto _img{ Img(imgtag) };
		int w{ 0 }, h{ 0 };
		_img->GetFormat(&w, &h);
		lua_pushinteger(L, w);
		lua_pushinteger(L, h);
		return 2;
	}

	static int API_LinkFont(lua_State* L) {
		size_t Untagged{ 0 };
		auto
			File{ Lunatic_CheckString(L,1) },
			Tag{ Lunatic_OptString(L,2,"") };
		if (!Tag.size()) {
			Tag = TrSPrintF("***TAG:%08x", Untagged++);
		} else if (Prefixed(Tag, "*")) {
			Crash("Tags for LoadImage may not be prefixed with *");
			return 0;
		}
		Fnt(Tag, File);
		Lunatic_PushString(L, Tag);
		return 1;
	}

	static int API_Text(lua_State* L) {
		auto
			Tag{ Lunatic_CheckString(L,1) },
			Text{ Lunatic_CheckString(L,2) };
		auto
			X{ luaL_checkinteger(L,3) },
			Y{ luaL_checkinteger(L,4) },
			HAlignment{ luaL_optinteger(L,5,0) },
			VAlignment{ luaL_optinteger(L,6,0) };
		Fnt(Tag)->Text(Text, X, Y, (Align)HAlignment, (Align)VAlignment);
		return 0;
	}

	static int API_TextSize(lua_State* L) {
		auto
			Tag{ Lunatic_CheckString(L,1) },
			Text{ Lunatic_CheckString(L,2) };
		lua_pushinteger(L, Fnt(Tag)->Width(Text));
		lua_pushinteger(L, Fnt(Tag)->Height(Text));
		return 2;
	}

	static int API_KillFont(lua_State* L) {
		Fnt(Lunatic_CheckString(L, 1), (TImageFont)nullptr);
		return 0;
	}

	static int API_HasFontTag(lua_State* L) {
		lua_pushboolean(L, HasFnt(luaL_checkstring(L, 1)));
		return 1;
	}

	static int API_Strech(lua_State* L) {
		auto
			Tag{ Lunatic_CheckString(L,1) };
		auto
			x{ luaL_checkinteger(L,2) },
			y{ luaL_checkinteger(L,3) },
			w{ luaL_checkinteger(L,4) },
			h{ luaL_checkinteger(L,5) },
			frame{ luaL_optinteger(L,6,0) };
		Img(Tag)->StretchDraw(x, y, w, h, frame);
		return 0;
	}

	static int API_Rect(lua_State* L) {
		ARect(
			luaL_checkinteger(L, 1),
			luaL_checkinteger(L, 2),
			luaL_checkinteger(L, 3),
			luaL_checkinteger(L, 4),
			luaL_optinteger(L, 5, 0)
		);
		return 0;
	}

	static int API_Flip(lua_State* L) {
		Flip();
		return 0;
	}

	static int API_DarkText(lua_State* L) {
		auto
			Tag{ Lunatic_CheckString(L,1) },
			Text{ Lunatic_CheckString(L,2) };
		auto
			X{ luaL_checkinteger(L,3) },
			Y{ luaL_checkinteger(L,4) },
			AlignmentH{ luaL_optinteger(L,5,0) },
			AlignmentV{ luaL_optinteger(L,6,0) };
		Fnt(Tag)->Dark(Text, X, Y, (Align)AlignmentH, (Align)AlignmentV);
		return 0;
	}

	static int API_SetAlpha(lua_State* L) {
		SetAlpha((byte)luaL_checkinteger(L, 1));
		return 0;
	}

	static int API_Line(lua_State* L) {
		ALine(
			luaL_checkinteger(L, 1),
			luaL_checkinteger(L, 2),
			luaL_checkinteger(L, 3),
			luaL_checkinteger(L, 4)
		);
		return 0;
	}

	static int API_ImageFrames(lua_State* L) {
		auto
			Tag{ Lunatic_CheckString(L,1) };
		lua_pushinteger(L, Img(Tag)->Frames());
		return 1;
	}

	static int API_SetScale(lua_State* L) {
		auto
			sx{ luaL_checknumber(L,1) },
			sy{ luaL_optnumber(L,2,sx) };
		SetScale(sx, sy);
		return 0;
	}

	static int API_Blit(lua_State* L) {
		auto
			Tag{ Lunatic_CheckString(L,1) };
		auto
			BImg{ Img(Tag) };
		switch (lua_gettop(L)) {
		case 7:
		case 8:
			BImg->Blit(
				luaL_checkinteger(L, 2), // x
				luaL_checkinteger(L, 3), // y
				luaL_checkinteger(L, 4), // insert start x
				luaL_checkinteger(L, 5), // insert start y
				luaL_checkinteger(L, 6), // insert end x
				luaL_checkinteger(L, 7), // insert end y
				luaL_optinteger(L, 8, 0)); // frame
			break;
		case 9:
		case 10:
			BImg->Blit(
				luaL_checkinteger(L, 2), // x
				luaL_checkinteger(L, 3), // y
				luaL_checkinteger(L,4), // width
				luaL_checkinteger(L,5), // height
				luaL_checkinteger(L, 6), // insert start x
				luaL_checkinteger(L, 7), // insert start y
				luaL_checkinteger(L, 8), // insert end x
				luaL_checkinteger(L, 9), // insert end y
				luaL_optinteger(L, 10, 0)); // frame
			break;
		default:
			Crash(TrSPrintF("TImage::Blit> Illegal function call! (%d parameters?)", lua_gettop(L)));
		}
		return 0;
	}

	static int API_GetColor(lua_State* L) {
		byte r, g, b;
		GetColor(r, g, b);
		lua_pushinteger(L, r);
		lua_pushinteger(L, g);
		lua_pushinteger(L, b);
		return 3;
	}

	static int API_Plot(lua_State*L){
		auto
			X{luaL_checkinteger(L,1)},
			Y{luaL_checkinteger(L,2)};
		Plot(X,Y);
		return 0;
	}

	static int API_ListImages(lua_State *L) { ListImages(); return 0;}

	static int API_Circle(lua_State* L) {
		auto
			X{luaL_checkinteger(L,1)},  	// X
			Y{luaL_checkinteger(L,2)},  	// Y
			R{luaL_checkinteger(L,3)},  	// Radio
			S{luaL_optinteger(L,4,100)};	// Segments
		ACircle(X,Y,R,S);
		return 0;
	}


	void Init_API_Graphics() {
		std::map<std::string, lua_CFunction>IAPI{
			{ "KillImage", API_Kill},
			{ "LoadImage",API_LoadImage },
			{ "HasImageTag",API_HasImageTag },
			{ "Draw",API_DrawImage },
			{ "DrawRotated",API_DrawRotatedImage },
			{ "TrueDraw",API_TrueDrawImage },
			{ "Tile",API_TileImage },
			{ "Cls",API_Cls },
			{ "HotCenter",API_HotCenter },
			{ "Hot",API_Hot },
			{ "Color",API_Color },
			{ "ColorHSV",API_ColorHSV },
			{ "GetColor",API_GetColor },
			{ "Alpha",API_SetAlpha },
			{ "Width",API_SWidth },
			{ "Height",API_SHeight },
			{ "Scale",API_SetScale },
			{ "GetImageFormat",API_ImgFmt },
			{ "ImageFrames",API_ImageFrames },
			{ "LinkFont",API_LinkFont },
			{ "Text",API_Text },
			{ "TextSize",API_TextSize },
			{ "DarkText",API_DarkText },
			{ "KillFont", API_KillFont },
			{ "HasFontTag",API_HasFontTag },
			{ "Stretch",API_Strech },
			{ "Rect",API_Rect },
			{ "Line",API_Line },
			{ "Blit",API_Blit },
			{ "Plot",API_Plot },
			{ "Flip",API_Flip },
			{ "ListImages",API_ListImages	},
			{ "Circle", API_Circle }
		};
		InstallAPI("Graphics", IAPI);
	}
}
