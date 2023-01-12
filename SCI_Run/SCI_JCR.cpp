// Lic:
// Scyndi's Creative Interpreter
// JCR management
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
#include <SlyvQCol.hpp>

#include <JCR6_Core.hpp>
#include <JCR6_JQL.hpp>
#include <JCR6_zlib.hpp>

#include "SCI_JCR.hpp"
#include "SCI_Config.hpp"

using namespace Slyvina::JCR6;
using namespace Slyvina::Units;

namespace  Scyndi_CI {
    inline void InitJCR6forSCI() {
        static bool done{ false };
        if (done) return;
        QCol->Doing("Init", "zlib for JCR6");  init_zlib();
        QCol->Doing("Init", "JCR Quick Link"); InitJQL();
        done = true;
    }

    static JT_Dir _srf{nullptr};
    JT_Dir SRF() {
        InitJCR6forSCI();
        if (!_srf) {
            QCol->Doing("SRF", JCR_SRF());
            _srf = JCR6_Dir(JCR_SRF());
            if (Last()->Error) QCol->Error("JCR Error:" + Last()->ErrorMessage);
            if (!_srf) QCol->Error("Loading '" + JCR_SRF() + "' failed for unknown reasons");
        }
        return _srf;
    }

    static JT_Dir _Resource{ nullptr };
    Slyvina::JCR6::JT_Dir Resource() {
        InitJCR6forSCI();
        if (!_Resource) {
            QCol->Doing("Reading", JCR_MainFile());
            _Resource = JCR6_Dir(JCR_MainFile());
            if (Last()->Error) QCol->Error("JCR Error:" + Last()->ErrorMessage);
            if (!_Resource) QCol->Error("Loading '" + JCR_MainFile() + "' failed for unknown reasons");
        }
        return _Resource;
    }
}