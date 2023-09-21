#pragma once
#include <TQSA.hpp>


namespace Scyndi_CI {

	Slyvina::TQSA::TAudio Aud(std::string Tag, bool crash = true);
	void Aud(std::string Tag, Slyvina::TQSA::TAudio AU);
	void Aud(std::string Tag, Slyvina::JCR6::JT_Dir J, std::string File);
	void Aud(std::string Tag, std::string File);
	bool HasAud(std::string Tag);

	int PlayAud(std::string Tag);
	int PlayAud(std::string Tag, int channel);

	int LoopAud(std::string Tag, int loops = -1);
	int LoopAud(std::string Tag, int loops, int channel);

	void KillAud(std::string Tag);

	
}
