#include "stdafx.h"
#include "MagnetLinkParser.h"
#include "boost/algorithm/string.hpp"

typedef std::size_t int_t;

MLStructure::MLStructure() {

}

MLStructure::~MLStructure() {

}

MagnetLinkParser::MagnetLinkParser()
{

}

MagnetLinkParser::~MagnetLinkParser()
{

}

MLStructure* MagnetLinkParser::parse(std::string magnetLink, MLStructure *mlStructure = new MLStructure()) {
	int_t result; 
	result = magnetLink.find(std::string("magnet:?"));
	if (result == std::string::npos || result != 0) {
		mlStructure->items["error"] = "The magnet link is invalid";
		return mlStructure;
	}
	magnetLink.erase(0, 8);
	std::vector<std::string> parameters;
	ss::Split(magnetLink, '&', parameters);
	std::string key;
	std::string value;
	std::string parameter;
	for (int_t i = 0; i < parameters.size(); i++) {
		parameter = parameters[i];
		result = parameter.find('=');
		key = parameter.substr(0, result);
		value = parameter.substr(result + 1, parameter.size() - result - 1);
		if (key == "tr") {
			mlStructure->trackers.push_back(value);
			//return mlStructure;
		}
		else if (key == "xt") {
			mlStructure->exactTopic = value;
			std::vector<std::string> tokens;
			ss::Split(value, ':', tokens);
			if (tokens.size() > 2 && tokens[0] == "urn" && tokens[1] == "btih") 
			{
				ss::HexToBytes(tokens[2], mlStructure->infoHash, 20);
			}
			else 
			{
				mlStructure->items["error"] = "The magnet link is invalid";
				return mlStructure;
			}
		}
		else if (key == "dn") {
			mlStructure->displayName = value;
		}
		mlStructure->items[key] = value;
	}

	//As debug help
	mlStructure->items["debug"].assign(magnetLink);

	return mlStructure;
}