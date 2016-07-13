#pragma once

#include "boost/asio.hpp"
//#include "Torrent.h"
class Torrent;

class Tracker : public std::enable_shared_from_this<Tracker>
{
private:
	boost::asio::ip::udp::socket *_mpSocket;
	std::shared_ptr<Torrent> _mTorrent;
	char mUdpPackageBuffer[65535];
	void SendStart();
public:
	std::string infoHash;
	std::string protocole;
	std::string hostName;
	std::string address;
	int port;

	Tracker(std::string _trackerLink, std::shared_ptr<Torrent> torrent);
	~Tracker();

	void Connect(boost::asio::ip::udp::resolver &resolver);
};

