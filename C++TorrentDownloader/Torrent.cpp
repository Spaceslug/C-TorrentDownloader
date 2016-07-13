#include "stdafx.h"
#include "Torrent.h"

Torrent::Torrent()
{
}


Torrent::~Torrent()
{
}


void Torrent::AddTracker(std::shared_ptr<Tracker> tracker) {
	mThisMutex.lock();
	mTrackers.push_back(tracker);
	mThisMutex.unlock();
}

//Trys to connect to each tracker
void Torrent::ConnectToTrackers(boost::asio::ip::udp::resolver &udpResolver) {
	for each (auto tracker in mTrackers)
	{
		tracker->Connect(udpResolver);
	}
}

std::string Torrent::GetPeerId() {
	return _mPeerId;
}

void Torrent::SetPeerId(std::string peerId) {
	mThisMutex.lock();
	_mPeerId = peerId;
	mThisMutex.unlock();
}

std::string Torrent::GetInfoHash() {
	return _mInfoHash;
}

void Torrent::SetInfoHash(std::string infoHash) {
	mThisMutex.lock();
	_mInfoHash = infoHash;
	mThisMutex.unlock();
}

void Torrent::SetDisplayName(std::string dn) {
	mThisMutex.lock();
	_mDisplayName = dn;
	mThisMutex.unlock();
}


//Rippes out the latest error message for this torrent
std::string Torrent::GetError() {
	std::string err;
	mThisMutex.lock();
	if (mErrorMessage.size() > 0) {
		err.swap(mErrorMessage);
	}
	mThisMutex.unlock();
	return err;
}

//Sets the error message
void Torrent::SetError(std::string err) {
	mThisMutex.lock();
	mErrorMessage = err;
	mThisMutex.unlock();
}

bool Torrent::HasError() {
	return !mErrorMessage.empty();
}