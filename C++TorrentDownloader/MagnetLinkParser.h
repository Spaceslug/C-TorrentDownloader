#pragma once
#include <unordered_map>
#include <string>
class MLStructure
{
public:
	MLStructure();
	~MLStructure();
	std::unordered_map<std::string, std::string> items;
	std::vector<std::string> trackers;
	std::string displayName;
	std::string exactTopic;
	char infoHash[20];
	
	std::string get(std::string key) {
		return items[key];
	}

	std::string operator[](std::string s) {
		return items[s];
	}
};

class MagnetLinkParser
{
public:
	MagnetLinkParser();
	~MagnetLinkParser();

	static MLStructure* parse(std::string magnetLink, MLStructure *mlStructure);
};

