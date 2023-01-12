// Lic:
// Scyndi's Creative Interpreter
// Load
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

#include <SlyvString.hpp>
#include <SlyvQCol.hpp>
#include <SlyvGINIE.hpp>
#include <TQSE.hpp>
#include "SCI_Graphics.hpp"
#include "SCI_Load.hpp"
#include "SCI_JCR.hpp"

using namespace Slyvina;
using namespace Units;
using namespace JCR6;
using namespace TQSE;
namespace Scyndi_CI {
	void LoadAllStartStuff() {
		//auto Entries = Resource()->Entries();
		JT_Entry Ent{ nullptr };
		for (auto& EK : Resource()->_Entries) {
			auto& E{ EK.second };
			//std::cout << "Entry: " << Upper(StripDir(E->Name())) << "\t"<<Entries->size()<< std::endl; // debug
			//std::cout << "Entry: " << Upper(StripDir(E->Name())) << std::endl; // debug
			if (Upper(StripDir(E->Name())) == "STARTLOAD.INI") {
				Ent = E;
				break;
			}
		}
		if (!Ent) {
			QCol->Doing("Start Load", "* NOT FOUND*");
			return;
		}
		QCol->Doing("Start Load", Ent->Name());
		auto stuff{ Resource()->GetString(Ent->Name()) };
		auto startload{ ParseGINIE(stuff) };
		auto images{ startload->Values("Images") };
		auto fonts{ startload->Values("Fonts") };
		auto audio{ startload->Values("Audio") };	
		for (auto f : *images) { QCol->Doing("Load image", startload->Value("Images", f), "\t"); QCol->Doing("to", f); Img(f, startload->Value("Images", f)); Poll(); }
		for (auto f : *fonts) { QCol->Doing("Load font", startload->Value("Fonts", f), "\t"); QCol->Doing("to", f); Fnt(f, startload->Value("Fonts", f)); Poll(); }
		QCol->Doing("TO DO:", "Audio\x7");
	}
}