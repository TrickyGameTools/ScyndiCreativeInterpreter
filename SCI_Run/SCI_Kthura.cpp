// Lic:
// Scyndi's Creative Interpreter
// Kthura Manager
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
// Version: 23.10.07
// EndLic

#include <SlyvQCol.hpp>

#include <Kthura_Core.hpp>
#include <Kthura_Draw.hpp>
#include <Kthura_Walk_Dijkstra.hpp>
#include <Kthura_Draw_TQSG.hpp>

#include "SCI_JCR.hpp"
#include "SCI_Kthura.hpp"
#include "SCI_Crash.hpp"


using namespace Slyvina;
using namespace Kthura;
using namespace Units;

namespace Scyndi_CI {

	KthuraDraw SCI_KthuraDraw{ nullptr };
	static std::string MapLastLoaded{""};
	static std::string MapLastPicked{""};
	static std::string LayerLastPicked{""};
	static std::map<std::string, Slyvina::Kthura::Kthura> MapRegister;
	static Slyvina::Kthura::Kthura ChosenMap{nullptr};
	static KthuraLayer* ChosenLayer{nullptr};

	static void SCIKthuraCrash(std::string errormessage, std::string xdata) {
		Crash("Kthura error: " + errormessage, xdata);
	}
	static void SCIKthuraDrawCrash(std::string em) { SCIKthuraCrash(em, "Error in Kthura_Draw_TQSG"); }

	static void Init_SCI_Kthura() {
		if (!SCI_KthuraDraw) {
			QCol->Doing("Setting up", "Kthura");
			QCol->Doing("=>", "Dijkstra");
			InitDijkstraForKthura();
			QCol->Doing("=>", "Draw routines");
			SCI_KthuraDraw = Init_TQSG_For_Kthura(Resource());
			QCol->Doing("=>", "Panic");
			KthuraPanic = SCIKthuraCrash;
			TQSG_Kthura_Panic = SCIKthuraDrawCrash;
			QCol->Doing("=>", "Done");
		}
	}
	
	Slyvina::Kthura::Kthura GetKthura(std::string Tag) {
		Init_SCI_Kthura();
		Trans2Upper(Tag);
		if (!MapRegister.count(Tag)) Crash("Non-existent Kthura Map requested", "Tag:" + Tag);
		return MapRegister[Tag];
	}

	Slyvina::Kthura::Kthura GetKthura() { 
		Init_SCI_Kthura();
		if (!ChosenMap) Crash("Requested the chosen map, but none have been chosen yet!");
		return ChosenMap; 
	}

	void SCI_LoadKthura(std::string Tag, std::string MapDir) {
		Init_SCI_Kthura();
		Trans2Upper(Tag);
		QCol->Doing("Loading Kthura", MapDir, " ");
		QCol->Doing("to", Tag);
		MapRegister[Tag] = LoadKthura(Resource(), MapDir);
		MapLastLoaded = MapDir;
	}

	std::string LastLoadedKthura() { return MapLastLoaded; }

	KthuraLayer* GetKthuraLayer(std::string MapTag, std::string LayerTag) {
		Init_SCI_Kthura();
		auto m{ GetKthura(MapTag) };
		if (!m->HasLayer(LayerTag)) {
			//auto l{ m->Layers() }; std::cout << "Layers: " << l->size() << "\n"; for (auto il : *l) std::cout << "Lay: " + il << "\n"; // debug
			Crash("Non-existent Kthura Layer requested", "Map Tag: " + MapTag + "\nLayer Tag: " + LayerTag);
		}
		return m->Layer(LayerTag);
	}

	Slyvina::Kthura::KthuraLayer* GetKthuraLayer(std::string LayerTag) {
		auto m{ GetKthura() };
		if (!m->HasLayer(LayerTag)) {
			//auto l{ m->Layers() }; std::cout << "Layers: " << l->size() << "\n"; for (auto il : *l) std::cout << "Lay: " + il << "\n"; // debug
			Crash("Non-existent Kthura Layer requested", "Map Tag:" + MapLastPicked + "\nLayer Tag:" + LayerTag);
		}
		return m->Layer(LayerTag);
	}

	Slyvina::Kthura::KthuraLayer* GetKthuraLayer() {
		if (!ChosenLayer) Crash("Requested the chosen layer, but none have been chosen yet!");
		return ChosenLayer;
	}

	void PickKthuraLayer(std::string MapTag, std::string LayerTag) {
		ChosenLayer = GetKthuraLayer(MapTag,LayerTag);
	}

	void PickKthuraLayer(std::string LayerTag) {		
		ChosenLayer = GetKthuraLayer(LayerTag);
		LayerLastPicked = LayerTag;
	}

	std::string PickedMap() { return MapLastPicked; }

	std::string PickedLayer() { return LayerLastPicked; }

	bool MapPicked() { return MapLastPicked != ""; }

	bool LayerPicked() { return LayerLastPicked != ""; }

	void PickKthura(std::string Tag) { 
		Init_SCI_Kthura();
		ChosenMap = GetKthura(Tag); 
		if (Tag == MapLastPicked) { ChosenMap = nullptr; ChosenLayer = nullptr; }
		MapLastPicked = Tag;
		LayerLastPicked = "";
	}

}