#pragma once

#include <config/config.h> // TODO move config in config/config/

namespace Exporter::GCode
{

class Format
{
public:
	explicit Format(const Config::Config::Gcode &format);

	std::string toolOn;
	std::string toolOff;
	std::string linearMove;
	std::string fastMove;
	std::string cwArc;
	std::string ccwArc;
};

}
