// chat_client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <thread>
#include <deque>
#include "ChatMessage.cpp"

using asio::ip::tcp;

std::string offsetText;

class chat_client
{
public:
	chat_client(boost::asio::io_service& io_service,
		tcp::resolver::iterator endpoint_iterator)
		: io_service_(io_service),
		socket_(io_service)
	{
		do_connect(endpoint_iterator);
	}

	void write(const chat_message& msg)
	{
		io_service_.post(
			[this, msg]()
		{
			bool write_in_progress = !write_msgs_.empty();
			write_msgs_.push_back(msg);
			if (!write_in_progress)
			{
				do_write();
			}
		});
	}

	void close()
	{
		io_service_.post([this]() { socket_.close(); });
	}

	void setName(std::string newName) {
		name = newName;
	}

private:
	void do_connect(tcp::resolver::iterator endpoint_iterator)
	{
		asio::async_connect(socket_, endpoint_iterator,
			[this](const boost::system::error_code ec, tcp::resolver::iterator)
		{
			if (!ec)
			{
				std::cout << "\r";
				std::cout << "Connection sucsessful";
				std::cout << "                \n" << offsetText;
				do_read_header();
			}
		});
	}

	void do_read_header()
	{
		asio::async_read(socket_,
			asio::buffer(read_msg_.data(), chat_message::header_length),
			[this](const boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec && read_msg_.decode_header())
			{
				do_read_body();
			}
			else
			{
				socket_.close();
			}
		});
	}

	void do_read_body()
	{
		asio::async_read(socket_,
			asio::buffer(read_msg_.body(), read_msg_.body_length()),
			[this](const boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				std::cout << "\r";
				std::cout.write(read_msg_.body(), read_msg_.body_length());
				std::cout << "                \n" << offsetText;
				do_read_header();
			}
			else
			{
				socket_.close();
			}
		});
	}

	void do_write()
	{
		asio::async_write(socket_,
			asio::buffer(write_msgs_.front().data(),
				write_msgs_.front().length()),
			[this](const boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				write_msgs_.pop_front();
				if (!write_msgs_.empty())
				{
					do_write();
				}
			}
			else
			{
				socket_.close();
			}
		});
	}

private:
	asio::io_service& io_service_;
	tcp::socket socket_;
	chat_message read_msg_;
	std::deque<chat_message> write_msgs_;
	std::string name;
};

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 4)
		{
			std::cerr << "Usage: chat_client <name> <host> <port>\n";
			return 1;
		}

		asio::io_service io_service;

		tcp::resolver resolver(io_service);
		asio::ip::basic_resolver_iterator<tcp> endpoint_iterator = resolver.resolve({ argv[2], argv[3] });
		chat_client _chat_client(io_service, endpoint_iterator);
		std::string uName(argv[1]);
		offsetText.append(argv[1]);
		offsetText.append(": ");
		_chat_client.setName(uName);
		std::cout << "Username: " << argv[1] << std::endl;
		std::cout << "Connected to server " << argv[2] << ":" << argv[3] << std::endl;
		std::thread t([&io_service]() { io_service.run(); });
		

		char line[chat_message::max_body_length + 1];
		//uName.copy(line, uName.size(), 0);
		strcpy_s(line, offsetText.data());
		int offset = offsetText.size();
		std::cout << offsetText;
		while (std::cin.getline(line + offset, chat_message::max_body_length + 1 - offset))
		{
			std::cout << offsetText;
			chat_message msg;
			msg.body_length(std::strlen(line));
			std::memcpy(msg.body(), line, msg.body_length());
			msg.encode_header();
			_chat_client.write(msg);
		}

		_chat_client.close();
		t.join();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}

