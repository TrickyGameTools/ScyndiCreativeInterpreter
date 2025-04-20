// License:
// 
// Scyndi's Creative Interpreter
// Kthura API
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
// Version: 25.04.01
// End License


#include <SlyvHSVRGB.hpp>

#include <Lunatic.hpp>

#include "../SCI_Script.hpp"
#include "../SCI_Kthura.hpp"
#include "../SCI_Crash.hpp"
using namespace Slyvina;
using namespace Slyvina::Units;
using namespace Slyvina::Kthura;
using namespace Slyvina::NSLunatic;

namespace Scyndi_CI {


	static int API_Kthura_Load(lua_State* L) {
		//printf("Kthura LoadRequest\n");
		auto
			MapTag = Lunatic_CheckString(L, 1),
			MapBundle = Lunatic_CheckString(L, 2);
		//printf("=> Go!\n");
		SCI_LoadKthura(MapTag, MapBundle);
		//printf("=> Done!\n");
		return 0;
	}

	static int API_Kthura_Pick(lua_State* L) {
		PickKthura(luaL_checkstring(L, 1));
		auto Lay = Lunatic_OptString(L, 2, "");
		if (Lay != "") PickKthuraLayer(Lay);
		return 0;
	}

	static int API_Kthura_PickLayer(lua_State* L) {
		PickKthuraLayer(luaL_checkstring(L, 1));
		return 0;
	}

	static int API_Kthura_CurrentMap(lua_State* L) {
		Lunatic_PushString(L, PickedMap());
		return 1;
	}

	static int API_Kthura_CurrentLayer(lua_State* L) {
		Lunatic_PushString(L, PickedLayer());
		return 1;
	}

	static int API_Kthura_LoadedMap(lua_State* L) {
		Lunatic_PushString(L, LastLoadedKthura());
		return 1;
	}

#define SetObjInt(skey,key) if (ObjKey==skey) {o->key(luaL_checkinteger(L,3)); return 0; }
#define SetObjByte(skey,key) if (ObjKey==skey) {o->key((byte)(luaL_checkinteger(L,3)%256)); return 0; }
#define SetObjString(skey,key) if (ObjKey==skey) {o->key(luaL_checkstring(L,3)); return 0; }
#define SetObjBool(skey,key) if (ObjKey==skey) {o->key(luaL_checkinteger(L,3)>0); return 0; }
	static int API_Kthura_SetObj(lua_State* L) {
		auto
			ObjTag{ Lunatic_CheckString(L,1) },
			ObjKey{ Upper(luaL_checkstring(L,2)) };
		auto
			o{ GetKthuraLayer()->Obj(ObjTag) };
		SetObjString("KIND",SKind);
		SetObjInt("X", x);
		SetObjInt("Y", y);
		SetObjInt("W", w);
		SetObjInt("H", h);
		SetObjInt("INSX", insertx);
		SetObjInt("INSY", inserty);
		SetObjInt("INSERTX", insertx);
		SetObjInt("INSERTY", inserty);
		SetObjInt("SCALEX", scalex);
		SetObjInt("SCALEY", scalex);
		SetObjInt("ROTATE", rotatedeg);
		SetObjInt("ROTATION", rotatedeg);
		SetObjInt("ROTATEDEG", rotatedeg);
		SetObjInt("ROTATEDEGREES", rotatedeg);
		SetObjInt("ROTATERAD", rotaterad);
		SetObjInt("ROTATERADIANS", rotaterad);
		SetObjByte("R", red);
		SetObjByte("G", green);
		SetObjByte("B", blue);
		SetObjByte("ALPHA", alpha);
		SetObjByte("ALPHA255", alpha);
		SetObjByte("RED", red);
		SetObjByte("GREEN", green);
		SetObjByte("BLUE", blue);
		SetObjInt("ANIMSPEED", animspeed);
		SetObjInt("DOMINANCE", dominance);
		SetObjInt("BLEND", blend);
		SetObjString("TEXTURE", texture);
		SetObjString("TAG", Tag);
		SetObjString("LABELS", labels);
		SetObjBool("IMPASSIBLE", impassible);
		SetObjBool("BLOCKED", impassible);
		SetObjBool("BLOCK", impassible);
		SetObjBool("FORCEPASSIBLE", forcepassible);
		SetObjInt("ANIMFRAME", animframe);
		SetObjInt("FRAME", animframe);
		SetObjBool("VISIBLE", visible);
		SetObjBool("NOTINMOTIONTHEN0", NotInMotionThen0);
		SetObjBool("NOTMOVINGTHEN0", NotInMotionThen0);
		// Actor only
		SetObjString("WIND", Wind);
		Crash("(Set) Unknown object field: " + ObjKey);
		return 0;
	}

#define GetObjInt(skey,key) if (ObjKey==skey) { lua_pushinteger(L,o->key()); return 1; }
#define GetObjByte(skey,key) if (ObjKey==skey) { lua_pushinteger(L,o->key()); return 1; }
#define GetObjString(skey,key) if (ObjKey==skey) {Lunatic_PushString(L,o->key()); return 1; }
#define GetObjBool(skey,key) if (ObjKey==skey) { lua_pushboolean(L,o->key()); return 1; }

	static int API_Kthura_GetObj(lua_State* L) {
		auto
			ObjTag{ Lunatic_CheckString(L,1) },
			ObjKey{ Upper(luaL_checkstring(L,2)) };
		auto
			o{ GetKthuraLayer()->Obj(ObjTag) };
		// Method

		// Object
		GetObjString("KIND",SKind);
		GetObjInt("X", x);
		GetObjInt("Y", y);
		GetObjInt("W", w);
		GetObjInt("H", h);
		GetObjInt("INSX", insertx);
		GetObjInt("INSY", inserty);
		GetObjInt("INSERTX", insertx);
		GetObjInt("INSERTY", inserty);
		GetObjInt("SCALEX", scalex);
		GetObjInt("SCALEY", scalex);
		GetObjInt("ROTATE", rotatedeg);
		GetObjInt("ROTATION", rotatedeg);
		GetObjInt("ROTATEDEG", rotatedeg);
		GetObjInt("ROTATEDEGREES", rotatedeg);
		GetObjInt("ROTATERAD", rotaterad);
		GetObjInt("ROTATERADIANS", rotaterad);
		GetObjByte("R", red);
		GetObjByte("G", green);
		GetObjByte("B", blue);
		GetObjByte("RED", red);
		GetObjByte("GREEN", green);
		GetObjByte("BLUE", blue);
		GetObjByte("ALPHA", alpha);
		GetObjByte("ALPHA255", alpha);
		GetObjInt("ANIMSPEED", animspeed);
		GetObjInt("DOMINANCE", dominance);
		GetObjInt("BLEND", blend);
		GetObjString("TEXTURE", texture);
		GetObjString("TAG", Tag);
		GetObjString("LABELS", labels);
		GetObjBool("IMPASSIBLE", impassible);
		GetObjBool("BLOCKED", impassible);
		GetObjBool("BLOCK", impassible);
		GetObjBool("FORCEPASSIBLE", forcepassible);
		GetObjInt("ANIMFRAME", animframe);
		GetObjInt("FRAME", animframe);
		GetObjBool("VISIBLE", visible);
		GetObjBool("WALKING", Walking); // Read-Only
		GetObjString("KIND", SKind); // Read-Only
		GetObjBool("NOTINMOTIONTHEN0", NotInMotionThen0);
		GetObjBool("NOTMOVINGTHEN0", NotInMotionThen0);
		// Actor only
		GetObjString("WIND", Wind);
		Crash("(Get) Unknown object field: " + ObjKey);
		return 0;
	}

	static int API_Kthura_HSV(lua_State* L) {
		auto Tag{ Lunatic_CheckString(L,1) };
		auto o{ GetKthuraLayer()->Obj(Tag) };
		hsv _hsv{ luaL_checknumber(L,2),luaL_checknumber(L,3) ,luaL_checknumber(L,4) };
		rgb _rgb = hsv2rgb(_hsv);
		byte r{ (byte)floor(_rgb.r * 255) };
		byte g{ (byte)floor(_rgb.g * 255) };
		byte b{ (byte)floor(_rgb.b * 255) };
		o->r(r);
		o->g(g);
		o->b(b);
		return 0;
	}

	static int API_Kthura_WalkTo(lua_State* L) {
		auto Tag{ Lunatic_CheckString(L,1) };
		auto o{ GetKthuraLayer()->Obj(Tag) };
		auto n{ lua_gettop(L) }, t{ lua_type(L,2) };
		auto errtype = std::string("");
		// Who said function overloading was not possible in Lua? ;)
		switch (t) {
		case LUA_TSTRING:
			o->WalkTo(luaL_checkstring(L, 2));
			return 0;
		case LUA_TNUMBER:
			if (t == 2)
				o->WalkTo(GetKthuraLayer()->Obj(luaL_checkinteger(L, 2)));
			else
				o->WalkTo(luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));
			return 0;
		case LUA_TBOOLEAN:
			errtype = "boolean";
			break;
		case LUA_TTABLE:
			errtype = "table";
			break;
		case LUA_TTHREAD:
			errtype = "thread";
			break;
		case LUA_TFUNCTION:
			errtype = "function";
			break;
		case LUA_TNIL:
			errtype = "nil";
			break;
		case LUA_TLIGHTUSERDATA:
			errtype = "userdata";
			break;
		default:
			errtype = TrSPrintF("Unknown type (%d)");
			break;
		}
		Crash("Illegal \"WalkTo\" call. Type '" + errtype + "' is not allowed to define a target spot!");
		return 0;
	}

	static int API_Kthura_HasObj(lua_State* L) {
		auto n{ lua_gettop(L) }, t{ lua_type(L,1) };
		auto errtype = std::string("");
		// Who said function overloading was not possible in Lua? ;)
		switch (t) {
		case LUA_TSTRING:
			lua_pushboolean(L, GetKthuraLayer()->HasTag(luaL_checkstring(L, 1)));
			return 1;
		case LUA_TNUMBER:
			lua_pushboolean(L, GetKthuraLayer()->HasID(luaL_checkinteger(L, 1)));
			return 1;
		case LUA_TBOOLEAN:
			errtype = "boolean";
			break;
		case LUA_TTABLE:
			errtype = "table";
			break;
		case LUA_TTHREAD:
			errtype = "thread";
			break;
		case LUA_TFUNCTION:
			errtype = "function";
			break;
		case LUA_TNIL:
			errtype = "nil";
			break;
		case LUA_TLIGHTUSERDATA:
			errtype = "userdata";
			break;
		default:
			errtype = TrSPrintF("Unknown type (%d)");
			break;
		}
		Crash("Illegal \"HasObj\" call. Type '" + errtype + "' is not allowed to define the identification of an object");
		return 0;
	}

	static int API_Kthura_MoveTo(lua_State* L) {
		auto Tag{ Lunatic_CheckString(L,1) };
		auto o{ GetKthuraLayer()->Obj(Tag) };
		auto n{ lua_gettop(L) }, t{ lua_type(L,2) };
		auto errtype = std::string("");
		// Who said function overloading was not possible in Lua? ;)
		switch (t) {
		case LUA_TSTRING:
			o->MoveTo(luaL_checkstring(L, 2));
			return 0;
		case LUA_TNUMBER:
			if (t == 2)
				o->MoveTo(GetKthuraLayer()->Obj(luaL_checkinteger(L, 2)));
			else
				o->MoveTo(luaL_checkinteger(L, 2), luaL_checkinteger(L, 3));
			return 0;
		case LUA_TBOOLEAN:
			errtype = "boolean";
			break;
		case LUA_TTABLE:
			errtype = "table";
			break;
		case LUA_TTHREAD:
			errtype = "thread";
			break;
		case LUA_TFUNCTION:
			errtype = "function";
			break;
		case LUA_TNIL:
			errtype = "nil";
			break;
		case LUA_TLIGHTUSERDATA:
			errtype = "userdata";
			break;
		default:
			errtype = TrSPrintF("Unknown type (%d)");
			break;
		}
		Crash("Illegal \"MoveTo\" call. Type '" + errtype + "' is not allowed to define a target spot!");
		return 0;
	}

	static int API_Kthura_Draw(lua_State* L) {
		auto
			ScrollX{ luaL_optinteger(L,1,0) },
			ScrollY{ luaL_optinteger(L,2,0) };
		SCI_KthuraDraw->DrawLayer(GetKthuraLayer(), (int)ScrollX, (int)ScrollY);
		return 0;
	}

	static int API_Kthura_Kill(lua_State* L) {
		auto t{ lua_type(L,1) };
		switch (t) {
		case LUA_TSTRING:
			GetKthuraLayer()->Kill(luaL_checkstring(L, 1));
			return 0;
		case LUA_TNUMBER:
			GetKthuraLayer()->Kill(GetKthuraLayer()->Obj(luaL_checkinteger(L, 1)));
			return 0;
		default:
			Crash("Invalid input for KILL. String for a tag or an integer for an ID expected!");
			return 0;
		}
	}


	static int API_Kthura_Spawn(lua_State* L) {
		auto Tag{ Lunatic_CheckString(L,1) };
		//auto o{ GetKthuraLayer()->Obj(Tag) };
		auto n{ lua_gettop(L) }, t{ lua_type(L,2) };
		auto errtype = std::string("");
		// Who said function overloading was not possible in Lua? ;)
		switch (t) {
		case LUA_TSTRING: {
			auto Spot{ Lunatic_CheckString(L,2) };
			if (!GetKthuraLayer()->HasTag(Spot)) {
				printf("Spot failure. Remapping tags and trying it again!\n");
				GetKthuraLayer()->RemapTags();
				if (!GetKthuraLayer()->HasTag(Spot)) {
					Crash("Kthura Layer '" + PickedLayer() + "' has no spot called '" + Spot + "'");
					return 0;
				}
			}
			GetKthuraLayer()->Spawn(Spot)->Tag(Tag);
			GetKthuraLayer()->RemapTags();
		}
						return 0;
		case LUA_TNUMBER:
			if (n == 2) {
				auto o{ GetKthuraLayer()->Obj(luaL_checkinteger(L, 2)) };
				GetKthuraLayer()->Spawn(o->x(), o->y())->Tag(Tag);
			} else
				GetKthuraLayer()->Spawn((int)luaL_checkinteger(L, 2), (int)luaL_checkinteger(L, 3))->Tag(Tag);
			GetKthuraLayer()->RemapTags();
			return 0;
		case LUA_TBOOLEAN:
			errtype = "boolean";
			break;
		case LUA_TTABLE:
			errtype = "table";
			break;
		case LUA_TTHREAD:
			errtype = "thread";
			break;
		case LUA_TFUNCTION:
			errtype = "function";
			break;
		case LUA_TNIL:
			errtype = "nil";
			break;
		case LUA_TLIGHTUSERDATA:
			errtype = "userdata";
			break;
		default:
			errtype = TrSPrintF("Unknown type (%d)");
			break;
		}
		Crash("Illegal \"Spawn\" call. Type '" + errtype + "' is not allowed to define a target spot!");
		return 0;
	}

	static int API_Kthura_SetMeta(lua_State* L) {
		auto
			Key{ Lunatic_CheckString(L,1) },
			Value{ Lunatic_CheckString(L,2) };
		(*GetKthura()->MetaData)[Key] = Value;
		return 0;
	}

	static int API_Kthura_GetMeta(lua_State* L) {
		auto
			Key{ Lunatic_CheckString(L,1) };
		Lunatic_PushString(L, (*GetKthura()->MetaData)[Key]);
		return 1;
	}

	static int API_Kthura_InObj(lua_State* L) {
		auto t{ lua_type(L,1) };
		KthuraObject* o{ nullptr };
		switch (t) {
		case LUA_TNUMBER:
			o = GetKthuraLayer()->Obj(luaL_checkinteger(L, 1));
			break;
		case LUA_TSTRING:
			o = GetKthuraLayer()->Obj(luaL_checkstring(L, 1));
			break;
		default:
			Crash("Illegal object identification!");
			break;
		}
		auto
			x{ luaL_checkinteger(L,2) },
			y{ luaL_checkinteger(L,3) };
		lua_pushboolean(L, InKthuraObject(o, x, y));
		return 1;
	}

	static int API_Kthura_Walking(lua_State* L) {
		auto t{ lua_type(L,1) };
		KthuraObject* o{ nullptr };
		switch (t) {
		case LUA_TNUMBER:
			o = GetKthuraLayer()->Obj(luaL_checkinteger(L, 1));
			break;
		case LUA_TSTRING:
			o = GetKthuraLayer()->Obj(luaL_checkstring(L, 1));
			break;
		default:
			Crash("Illegal object identification!");
			break;
		}
		lua_pushboolean(L, o->Kind() == KthuraKind::Actor && o->Walking());
		return 1;
	}

	static int API_Kthura_Moving(lua_State* L) {
		auto t{ lua_type(L,1) };
		KthuraObject* o{ nullptr };
		switch (t) {
		case LUA_TNUMBER:
			o = GetKthuraLayer()->Obj(luaL_checkinteger(L, 1));
			break;
		case LUA_TSTRING:
			o = GetKthuraLayer()->Obj(luaL_checkstring(L, 1));
			break;
		default:
			Crash("Illegal object identification!");
			break;
		}
		lua_pushboolean(L, o->Kind() == KthuraKind::Actor && o->Moving());
		return 1;
	}

	static int API_Kthura_StopWalking(lua_State* L) {
		auto t{ lua_type(L,1) };
		KthuraObject* o{ nullptr };
		switch (t) {
		case LUA_TNUMBER:
			o = GetKthuraLayer()->Obj(luaL_checkinteger(L, 1));
			break;
		case LUA_TSTRING:
			o = GetKthuraLayer()->Obj(luaL_checkstring(L, 1));
			break;
		default:
			Crash("Illegal object identification!");
			break;
		}
		o->StopWalking();
		return 0;
	}

	static int API_Kthura_Block(lua_State* L) {
		lua_pushboolean(L, GetKthuraLayer()->Block((int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2)));
		return 1;
	}

	static int API_Kthura_Remap(lua_State* L) {
		switch (luaL_optinteger(L, 1, 0)) {
		case 0:
			GetKthuraLayer()->TotalRemap(); break;
		case 1:
			GetKthuraLayer()->RemapDominance(); break;
		case 2:
			GetKthuraLayer()->RemapTags(); break;
		case 3:
			GetKthuraLayer()->RemapLabels(); break;
		case 4:
			GetKthuraLayer()->BuildBlockmap(); break;
		default:
			luaL_error(L, "Unknown remap instruction: %d", luaL_optinteger(L, 1, 0));
			break;
		}
		return 0;
	}

	static int API_Kthura_HideByLabel(lua_State* L) {
		GetKthuraLayer()->HideByLabel(luaL_checkstring(L, 1));
		return 0;
	}
	static int API_Kthura_HideButLabel(lua_State* L) {
		GetKthuraLayer()->HideButLabel(luaL_checkstring(L, 1));
		return 0;
	}
	static int API_Kthura_ShowButLabel(lua_State* L) {
		GetKthuraLayer()->ShowButLabel(luaL_checkstring(L, 1));
		return 0;
	}

	static int API_Kthura_ShowByLabel(lua_State* L) {
		GetKthuraLayer()->ShowByLabel(luaL_checkstring(L, 1));
		return 0;
	}
	static int API_Kthura_ObjDataSet(lua_State* L) {
		auto o{ GetKthuraLayer()->Obj(luaL_checkstring(L, 1)) };
		o->data(luaL_checkstring(L, 2), luaL_checkstring(L, 3));
		return 0;
	}
	static int API_Kthura_ObjDataGet(lua_State* L) {
		auto o{ GetKthuraLayer()->Obj(luaL_checkstring(L, 1)) };
		Lunatic_PushString(L, o->data(luaL_checkstring(L, 2)));
		return 1;
	}
	static int API_Kthura_CreateObject(lua_State* L) {
		auto layer{ GetKthuraLayer() };
		auto newobj{ layer->NewObject(Lunatic_CheckString(L,1)) };
		layer->RemapID();
		lua_pushinteger(L, newobj->ID());
		return 1;
	}
	static int API_Kthura_TagList(lua_State* L) {
		static std::vector<String> Tags{};
		auto i{ luaL_optinteger(L,1,-1) };
		if (i < 0) {
			Tags.clear();
			auto layer{ GetKthuraLayer() };
			layer->Tags(&Tags);
			lua_pushinteger(L,Tags.size());
			return 1;
		}
		if (i >= Tags.size()) { Crash("TagList out of bounds!", TrSPrintF("%d/%d", i, Tags.size())); return 0; }
		Lunatic_PushString(L, Tags[i]);
		return 1;
	}
	static int API_Kthura_LayerList(lua_State* L) {
		static std::vector<String> LayList{};
		auto todo{ luaL_checkinteger(L,1) };
		auto kmap{ GetKthura() };
		switch (todo) {
		case 0:
			LayList = *kmap->Layers(); // Copying all contents is safer at this point!
			lua_pushinteger(L, LayList.size());
			return 1;
		case 1: {
			auto idx{ luaL_checkinteger(L,2) };
			if (idx < 0 && idx >= LayList.size()) { luaL_error(L, "Layerlist index (%d) out of bounds (%d)", idx, LayList.size()); return 0; }
			Lunatic_PushString(L, LayList[idx]);
			return 1;
		}
		case 2:
			lua_pushinteger(L, LayList.size());
			return 1;
		}
	}
	static int API_Kthura_SetAutoRemap(lua_State* L) {
		auto kth{ GetKthura() };
		auto val{ Lunatic_CheckBoolean(L,1) };
		auto lay{ Lunatic_OptString(L,2,"") };
		lay == "" ?
			kth->AutoRemap(val) :
			kth->Layer(lay)->AutoRemap(val);
		return 0;
	}
	static int API_Kthura_GetAutoRemap(lua_State* L) {
		lua_pushboolean(L, GetKthuraLayer()->AutoRemap());
		return 1;
	}
	static int API_Kthura_AnyThingMoving(lua_State* L) {
		auto lay{ GetKthuraLayer() };
		auto chk{ luaL_optinteger(L,1,0) };
		auto ret{ false };
		for (auto o = lay->FirstObject(); o; o = o->Next()) {
			if (o->Kind()==KthuraKind::Actor)
				ret = (chk == 0 ? o->Moving() : o->Walking());
			if (ret) break;
		}
		lua_pushboolean(L, ret);
		return 1;
	}


	void Init_API_Kthura() {
		std::map<std::string, lua_CFunction>IAPI{
			{ "Load", API_Kthura_Load },
			{ "Pick",API_Kthura_Pick },
			{ "PickLayer",API_Kthura_PickLayer },
			{ "CurrentMap",API_Kthura_CurrentMap },
			{ "CurrentLayer",API_Kthura_CurrentLayer },
			{ "SetObj",API_Kthura_SetObj },
			{ "GetObj",API_Kthura_GetObj },
			{ "ColorObjHSV",API_Kthura_HSV },
			{ "WalkTo", API_Kthura_WalkTo },
			{ "MoveTo",API_Kthura_MoveTo },
			{ "Draw",API_Kthura_Draw },
			{ "Kill",API_Kthura_Kill },
			{ "Spawn",API_Kthura_Spawn },
			{ "GetMeta",API_Kthura_GetMeta },
			{ "SetMeta",API_Kthura_SetMeta },
			{ "HasObj",API_Kthura_HasObj },
			{ "LastLoadedMap",API_Kthura_LoadedMap },
			{ "InObj",API_Kthura_InObj },
			{ "Walking",API_Kthura_Walking },
			{ "Moving",API_Kthura_Moving },
			{ "Block",API_Kthura_Block },
			{ "StopWalking",API_Kthura_StopWalking },
			{ "Remap",API_Kthura_Remap },
			{ "HideByLabel",API_Kthura_HideByLabel },
			{ "ShowByLabel", API_Kthura_ShowByLabel },
			{ "HideButLabel",API_Kthura_HideButLabel },
			{ "ShowButLabel",API_Kthura_ShowButLabel },
			{ "ObjDataGet",API_Kthura_ObjDataGet },
			{ "ObjDataSet",API_Kthura_ObjDataSet },
			{ "NewObj",API_Kthura_CreateObject },
			{ "TagList",API_Kthura_TagList },
			{ "GetLayers",API_Kthura_LayerList },
			{ "SetAutoRemap",API_Kthura_SetAutoRemap },
			{ "GetAutoRemap",API_Kthura_GetAutoRemap },
			{ "AnythingMoving",API_Kthura_AnyThingMoving }
		};

		InstallAPI("Kthura", IAPI);
	}
}
