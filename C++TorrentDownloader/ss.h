#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <iomanip>
#include <algorithm>

class ss {

public:
	static std::vector<std::string>& Split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	static std::vector<char> HexToBytes(const std::string& hex) {
		std::vector<char> bytes;

		for (unsigned int i = 0; i < hex.length(); i += 2) {
			std::string byteString = hex.substr(i, 2);
			char byte = (char)strtol(byteString.c_str(), NULL, 16);
			bytes.push_back(byte);
		}

		return bytes;
	}

	static void HexToBytes(const std::string& source, char destination [], size_t count) {

		for (unsigned int i = 0; i/2 < count && i < source.size(); i += 2) {
			std::string byteString = source.substr(i, 2);
			char byte = (char)strtol(byteString.c_str(), NULL, 16);
			destination[i/2] = byte;
		}
	}

	static std::string UrlEncode(const std::string &value) {
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << std::hex;

		for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
			std::string::value_type c = (*i);

			// Keep alphanumeric and other accepted characters intact
			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
				escaped << c;
				continue;
			}

			// Any other characters are percent-encoded
			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << int((unsigned char)c);
			escaped << std::nouppercase;
		}

		return escaped.str();
	}

	static std::string ToLower(std::string &s) {
		std::transform(s.begin(), s.end(), s.begin(), ::tolower);
		return s;
	}
};