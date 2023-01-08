#pragma once
#include <SlyvGINIE.hpp>

// This header and source that belong with it
// Handles the configuration that handles BOTH the builder and the Runtimer

namespace Scyndi_CI {
	std::string SCIHome();
	std::string GlobalConfigFile();
	Slyvina::Units::GINIE GlobalConfig();
}