#include "stdafx.h"
#include "MagnetLinkParser.h"
#include "boost/algorithm/string.hpp"
#include "Torrent.h"

typedef std::size_t int_t;

MagnetLinkParser::MagnetLinkParser()
{

}

MagnetLinkParser::~MagnetLinkParser()
{

}

std::shared_ptr<Torrent> MagnetLinkParser::parse(std::string magnetLink, std::shared_ptr<Torrent> torrent) {
	int_t result; 
	result = magnetLink.find(std::string("magnet:?"));
	if (result == std::string::npos || result != 0) {
		torrent->SetError("The magnet link is invalid");
		return torrent;
	}
	//Removes the unimportaint part
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
			torrent->AddTracker(std::make_shared<Tracker>(value, torrent->shared_from_this()));
			//return mlStructure;
		}
		else if (key == "xt") {
			std::vector<std::string> tokens;
			ss::Split(value, ':', tokens);
			if (tokens.size() > 2 && tokens[0] == "urn" && tokens[1] == "btih") 
			{
				char infoHash[20];
				ss::HexToBytes(tokens[2], infoHash, 20);
				torrent->SetInfoHash(std::string(infoHash));
			}
			else 
			{
				torrent->SetError("The magnet link is invalid");
				return torrent;
			}
		}
		else if (key == "dn") {
			torrent->SetDisplayName(value);
		}
	}

	return torrent;
}