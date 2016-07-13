#pragma once

#include <mutex>
#include "boost/asio.hpp"
#include "Tracker.h"
//class Tracker;

class Torrent : public std::enable_shared_from_this<Torrent>
{
private:
	std::mutex mThisMutex;
	std::vector<std::shared_ptr<Tracker>> mTrackers;
	std::string mErrorMessage;

	std::string _mDisplayName;
	std::string _mPeerId;
	std::string _mInfoHash;
	int mPeers;
	int mPices;
public:
	
	Torrent();
	~Torrent();

	void AddTracker(std::shared_ptr<Tracker> tracker);

	//Trys to connect to each tracker
	void ConnectToTrackers(boost::asio::ip::udp::resolver &udpResolver);

	std::string GetPeerId();

	void SetPeerId(std::string peerId);

	std::string GetInfoHash();

	void SetInfoHash(std::string infoHash);

	void SetDisplayName(std::string dn);
	

	//Rippes out the latest error message for this torrent
	std::string GetError();

	//Sets the error message
	void SetError(std::string err);

	bool HasError();
};

