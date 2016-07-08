// C++TorrentDownloader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "boost/thread.hpp"
#include "MagnetLinkParser.h" 
using asio::ip::tcp;
using asio::ip::udp;

std::string globalPeerIdStart = "-AS0100-";
std::string peerId = globalPeerIdStart + "somerandomdu";
std:srand

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 2)
		{
			std::cerr << "Usage: C++TorrentDownloader <magnetlink>\n";
			return 1;
		}



		MLStructure mlStructure;
		//Parses the magnet link
		MagnetLinkParser::parse(std::string(argv[1]), &mlStructure);

		std::cout << "Error: " << mlStructure["error"] << std::endl;
		//std::cout << "MagnetLink: " << mlStructure["debug"] << std::endl;

		asio::io_service io_service;
		tcp::resolver tcpResolver(io_service);
		udp::resolver udpResolver(io_service);
		for (int i = 0; i < mlStructure.trackers.size(); i++) {  //"udp://tracker.publicbt.com:80/announce"	
			short port;
			if (mlStructure.trackers[i].substr(0,3) == "udp") {
				std::vector<std::string> host;
				int startPort = 0;
				for (int j = 6; j < mlStructure.trackers[i].size(); j++) {
					if (mlStructure.trackers[i][j] == ':') {
						startPort = j;
					}
					else if (mlStructure.trackers[i][j] == '/') {
						if (!startPort) {
							port = 80;
							break; // Has no port. 80 is default?
						}
						port = std::stoi(mlStructure.trackers[i].substr(startPort + 1, j - startPort - 1));
						mlStructure.trackers[i].erase(startPort, j - startPort);
						mlStructure.trackers[i].erase(6);
					}
				}
				std::string* tracker = &mlStructure.trackers[i];
				//Time to send tracker GET
				udp::resolver::query query(mlStructure.trackers[i].substr(6, mlStructure.trackers[i].size() - 6));
				udpResolver.async_resolve(query, [&io_service, tracker, mlStructure](const boost::system::error_code& err, udp::resolver::iterator endpoint_iterator) {
					if (!err) {
						// Try each endpoint until we successfully establish a connection.
						udp::socket socket(io_service);
						boost::asio::connect(socket, endpoint_iterator);
						//send GET to tracker
						std::string getString = "GET " + *tracker + "?info_hash=" + ss::UrlEncode(mlStructure.infoHash)
							+ "&peer_id"
							+ " HTTP/1.1\r\n";
						socket.async_send(boost::asio::buffer(getString), [](const boost::system::error_code& err, std::size_t bytes_transferred) {
							if (!err) {
							
							}
							else {
								printf(err.message().c_str());
								int a; std::cin >> a; return 1;
							}
						});
					}
					else {
						printf(err.message().c_str());
						int a; std::cin >> a; return 1;
					}
				});

			}
		}
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

		io_service.stop();

		worker_threads.join_all();


		udp::resolver::query query(udp::v4(), argv[1]);
		udp::endpoint receiver_endpoint = *udpResolver.resolve(query);
		//sends a request to each tracker for peers
		std::vector<std::string> trackerGets(mlStructure.trackers.size());
		
		asio::ip::basic_resolver_iterator<tcp> endpoint_iterator = tcpResolver.resolve({ argv[1], argv[2] });
		asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
		//chat_client _chat_client(io_service, endpoint_iterator);

		//std::thread t([&io_service]() { io_service.run(); });

		//char line[chat_message::max_body_length + 1];
	/*	while (std::cin.getline(line, chat_message::max_body_length + 1))
		{
			chat_message msg;
			msg.body_length(std::strlen(line));
			std::memcpy(msg.body(), line, msg.body_length());
			msg.encode_header();
			_chat_client.write(msg);
		}

		_chat_client.close();*/
		//t.join();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}

