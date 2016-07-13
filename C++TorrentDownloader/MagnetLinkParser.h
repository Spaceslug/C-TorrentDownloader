#pragma once
#include <unordered_map>
#include <string>
#include "Torrent.h"
class MagnetLinkParser
{
public:
	MagnetLinkParser();
	~MagnetLinkParser();

	static std::shared_ptr<Torrent> parse(std::string magnetLink, std::shared_ptr<Torrent> torrent = std::make_shared<Torrent>());
};

