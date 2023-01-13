// Lic:
// Scyndi's Creative Interpreter
// Script & State Manager
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
#include <Lunatic.hpp>

#include "SCI_Script.hpp"
#include "SCI_JCR.hpp"

using namespace Slyvina;
using namespace Units;
using namespace Lunatic;

namespace Scyndi_CI {
    static std::map < std::string, Slyvina::Lunatic::Lunatic > StateRegister{};

    std::string MainScript() {
        static std::string ret{ "" };
        if (!ret.size()) {
            auto E = Resource()->Entries();
            for (auto f : *E) {
                auto mf = Upper(f->Name());
                auto tf = ExtractDir(mf);
                if (tf == "MAIN.LBC" || tf == "MAIN.LUA") ret = f->Name();
            }
        }
        if (!ret.size()) throw std::runtime_error("Neither MAIN.LBC nor MAIN.LUA found in any directory of the game's resource");
        return ret;
    }
    bool GotState(std::string _State) {
        Trans2Upper(_State);
        return StateRegister.count(_State);
    }
}