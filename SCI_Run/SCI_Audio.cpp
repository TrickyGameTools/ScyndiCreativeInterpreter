// Lic:
// Scyndi's Creative Interpreter
// Audio Manager
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
// Version: 23.09.21
// EndLic
#include "SCI_JCR.hpp"
#include "SCI_Audio.hpp"

using namespace Slyvina;
using namespace Units;
using namespace TQSA;
using namespace std;

namespace Scyndi_CI {

	static map<string, TAudio> AudioRegister{};

	Slyvina::TQSA::TAudio Aud(std::string Tag, bool crash) {
		Trans2Upper(Tag);
		if (!AudioRegister.count(Tag)) {
			if (crash) { throw std::runtime_error(TrSPrintF("There is no audio sample tagged '%s'", Tag.c_str())); }
			return nullptr;
		}
		return AudioRegister[Tag];
	}

	void Aud(std::string Tag, Slyvina::TQSA::TAudio AU) { 
		Trans2Upper(Tag);
		AudioRegister[Tag] = AU;		
	}

	void Aud(std::string Tag, Slyvina::JCR6::JT_Dir J, std::string File) { Aud(Tag, LoadAudio(Resource(), File)); }
	void Aud(std::string Tag, std::string File) { Aud(Tag, JCR6::JCR6_Dir(File), File); }
	bool HasAud(std::string Tag) { return AudioRegister.count(Upper(Tag)); }

	int PlayAud(std::string Tag) {
		return Aud(Tag)->Play();
	}

	int PlayAud(std::string Tag, int channel) {
		Aud(Tag)->ChPlay(channel);
		return channel;
	}

	int LoopAud(std::string Tag, int loops) { return Aud(Tag)->Play(loops); }

	int LoopAud(std::string Tag, int loops, int channel) {
		Aud(Tag)->ChPlay(channel,loops);
		return channel;
	}

	void KillAud(std::string Tag) {
		Trans2Upper(Tag);
		if (AudioRegister.count(Tag)) AudioRegister.erase(Tag);
	}

}