// C++TorrentDownloader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "boost/thread.hpp"
#include "MagnetLinkParser.h" 
#include "Tracker.h"
#include "Torrent.h"
#include <cstdlib>
#include <cstring>
#include <vector>
using asio::ip::tcp;
using asio::ip::udp;

char peerId[20] = { '-','A','S','0','1','0','0','-'};
std::string gPeerIdUrlEncoded;
std::string gPortClientTracker("6886");

asio::streambuf trackerResiver;

char udpPackage[65535];

int main(int argc, char* argv[])
{
	//Setting up peer id
	std::srand(std::time(0));
	for (int i = 8; i < 20; i += 4) {
		int x = std::rand();
		peerId[i] = (char)x;
		peerId[i + 1] = (char)(x >> 4);
		peerId[i + 2] = (char)(x >> 8);
		peerId[i + 3] = (char)(x >> 12);
	}
	
	gPeerIdUrlEncoded = ss::UrlEncode(std::string(peerId));
		
	try
	{
		if (argc != 2)
		{
			std::cerr << "Usage: C++TorrentDownloader <magnetlink>\n";
			return 1;
		}



		std::shared_ptr<Torrent> torrent = std::make_shared<Torrent>();
		torrent->SetPeerId(std::string(peerId));

		//Parses the magnet link
		MagnetLinkParser::parse(std::string(argv[1]), torrent);
		if (torrent->HasError()) {
			std::cout << "Error: " << torrent->GetError() << std::endl;
			std::cout << "Enter any key to quit...";
			std::cin.get();
			return 0;
		}
		//std::cout << "MagnetLink: " << mlStructure["debug"] << std::endl;

		asio::io_service io_service;
		tcp::resolver tcpResolver(io_service);
		udp::resolver udpResolver(io_service);
		std::vector<Tracker> trackers;

		torrent->ConnectToTrackers(udpResolver);

		//time to kick off some worker threads
		boost::thread_group worker_threads;
		for (int i = 0; i < 10; i++)
		{
			worker_threads.create_thread([&io_service]() {
				io_service.run();
			});
		}

		std::string command;
		bool quit = false;
		while (!quit) {
			std::cin >> command;
			ss::ToLower(command);
			if (command.find("quit")) {
				quit = true;
			}
		}
		int i;
		std::cin >> i;

		io_service.stop();

		worker_threads.join_all();


		
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}

