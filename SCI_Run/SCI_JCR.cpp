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
        bool done{ false };
        if (done) return;
        QCol->Doing("Init", "zlib for JCR6");  init_zlib();
        QCol->Doing("Init", "JCR Quick Link"); InitJQL();
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
}