#include "stdafx.h"
#include "Tracker.h"
#include "Torrent.h"
using asio::ip::udp;


Tracker::Tracker(std::string trackerString, std::shared_ptr<Torrent> torrent) 
{
	_mTorrent = torrent;
	if (trackerString.substr(0, 3) == "udp") {
		protocole = "udp";
		int startPort = 0;
		for (size_t j = 6; j < trackerString.size(); j++) {
			if (trackerString[j] == ':') {
				startPort = j;
				hostName = trackerString.substr(6, startPort - 6);
			}
			else if (trackerString[j] == '/') {
				if (!startPort) {
					port = 80; // Has no port. 80 is default?
					hostName = trackerString.substr(6, j - 6);
				}
				else {
					port = std::stoi(trackerString.substr(startPort + 1, j - startPort - 1));
					trackerString.erase(startPort, j - startPort);
				}
				break; 

			}
		}
		address = trackerString.substr(6, trackerString.size() - 6);
	}
}


Tracker::~Tracker()
{
	delete _mpSocket;
}

void Tracker::Connect(udp::resolver &resolver) 
{
	_mpSocket = new udp::socket(resolver.get_io_service());
	//Time to send tracker GET
	udp::resolver::query query(hostName, std::to_string(port));
	resolver.async_resolve(query, [this](const boost::system::error_code& err, udp::resolver::iterator endpoint_iterator) {
		if (!err) {
			// Try each endpoint until we successfully establish a connection.
			_mpSocket->connect( endpoint_iterator->endpoint());
			std::string s = _mpSocket->remote_endpoint().address().to_string();

			SendStart();
		}
		else {
			std::cout << "Error " << err.message() << std::endl;
		}
	});
}

void Tracker::SendStart() {
	//send GET to tracker
	std::string getString = "GET " + address
		+ "?info_hash=" + ss::UrlEncode(_mTorrent->GetInfoHash())
		+ "&peer_id=" + ss::UrlEncode(_mTorrent->GetPeerId())
		+ "&port=" + std::to_string(port)
		+ "&uploaded=0"
		+ "&downloaded=0"
		+ "&event=started"
		+ "&numwant=50"
		+ " HTTP/1.1\r\n";

	_mpSocket->send(boost::asio::buffer(getString));
	//std::cout << "Tracker request sent. bytes " << bytes_transferred << std::endl;
	_mpSocket->async_receive_from(boost::asio::buffer(mUdpPackageBuffer, sizeof(mUdpPackageBuffer)), _mpSocket->remote_endpoint(), [this](const boost::system::error_code& err, std::size_t bytes_transferred) {
		if (!err) {
			std::string s(mUdpPackageBuffer, bytes_transferred);
			std::cout << "Message resices";
		}
		else {
			std::cout << "Error " << err.message() << std::endl;
		}
	});
}
