// Lic:
// Scyndi's Creative Interpreter
// Kthura Manager (header)
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

#pragma once
#include <Kthura_Draw.hpp>

namespace Scyndi_CI {
	extern Slyvina::Kthura::KthuraDraw SCI_KthuraDraw;


	Slyvina::Kthura::Kthura GetKthura(std::string Tag);
	Slyvina::Kthura::Kthura GetKthura();
	void PickKthura(std::string Tag);
	void SCI_LoadKthura(std::string Tag,std::string MapDir);	
	std::string LastLoadedKthura();
	//KthuraLayer* GetKthuraLayer(std::string MapTag, std::string LayerTag);
	Slyvina::Kthura::KthuraLayer* GetKthuraLayer(std::string MapTag, std::string LayerTag);
	Slyvina::Kthura::KthuraLayer* GetKthuraLayer(std::string LayerTag);
	Slyvina::Kthura::KthuraLayer* GetKthuraLayer();
	void PickKthuraLayer(std::string MapTag, std::string LayerTag);
	void PickKthuraLayer(std::string LayerTag);
	std::string PickedMap();
	std::string PickedLayer();
	bool MapPicked();
	bool LayerPicked();
	bool InKthuraObject(Slyvina::Kthura::KthuraObject* o, int x, int y);
}