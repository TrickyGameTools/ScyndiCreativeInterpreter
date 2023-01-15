// Lic:
// Scyndi's Creative Interpreter
// Graphics API
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
#include <SlyvString.hpp>

#include "../SCI_Crash.hpp"
#include "../SCI_Script.hpp"
#include "../SCI_Graphics.hpp"

using namespace Slyvina;
using namespace Lunatic;
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
	}

	static int API_TrueDrawImage(lua_State* L) {
		auto
			Tag{ Lunatic_CheckString(L,1) };
		auto
			x{ luaL_checkinteger(L,2) },
			y{ luaL_checkinteger(L,3) },
			frame{ luaL_optinteger(L,4,0) };
		Img(Tag)->TrueDraw(x, y, frame);
	}

	static int API_HotCenter(lua_State* L) { Img(luaL_checkstring(L, 1))->HotCenter(); return 0; }
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
			Alignment{ luaL_optinteger(L,5,0) };
		Fnt(Tag)->Text(Text, X, Y, (Align)Alignment);
	}

	static int API_KillFont(lua_State* L) {
		Fnt(Lunatic_CheckString(L, 1), (TImageFont)nullptr);
	}

	static int API_HasFontTag(lua_State* L) {
		lua_pushboolean(L, HasFnt(luaL_checkstring(L, 1)));
		return 1;
	}


	void Init_API_Graphics() {
		std::map<std::string, lua_CFunction>IAPI{
			{"KillImage",API_Kill},
			{"LoadImage",API_LoadImage},
			{"HasImageTag",API_HasImageTag},
			{"Draw",API_DrawImage},
			{"TrueDraw",API_TrueDrawImage},
			{"Cls",API_Cls},
			{"HotCenter",API_HotCenter},
			{"Color",API_Color},
			{"ColorHSV",API_ColorHSV},
			{"Width",API_SWidth},
			{"Height",API_SHeight},
			{"GetImageFormat",API_ImgFmt},
			{"LinkFont",API_LinkFont},
			{"Text",API_Text},
			{"KillFont",API_KillFont},
			{"HasFontTag",API_HasFontTag}
		};
		InstallAPI("Graphics", IAPI);
	}
}