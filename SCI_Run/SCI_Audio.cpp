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
