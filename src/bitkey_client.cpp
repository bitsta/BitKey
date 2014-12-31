
//###############################################################
//# project: bitkey												#
//# author:  bitsta												#
//# date:    18.12.2014											#
//# spec:    bitkey client software			 					#
//# version: 0.9 - THIS IS EXPERIMENTAL SOFTWARE! 				#
//###############################################################


#include <iostream>
#include <cstdlib>
#include "asio.hpp"
#include "asio/ssl.hpp"
#include <boost/bind.hpp>
#include "lib/librootcoin.h"

enum { bitkey_max_length = 1024 };

class client
{
public:
  client(asio::io_service& io_s, asio::ssl::context& context,
      asio::ip::tcp::resolver::iterator endpoint_iterator)
    : socket_(io_s, cx)
  {
    asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
    socket_.lowest_layer().async_connect(endpoint,
        boost::bind(&client::connect, this,
          asio::placeholders::error, ++endpoint_iterator));
  }

  void connect(const asio::error_code& error,
      asio::ip::tcp::resolver::iterator endpoint_iterator)
  {
    if (!error)
    {
      socket_.async_handshake(asio::ssl::stream_base::client,
          boost::bind(&client::handshake, this,
            asio::placeholders::error));
    }
    else if (endpoint_iterator != asio::ip::tcp::resolver::iterator())
    {
      socket_.lowest_layer().close();
      asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
      socket_.lowest_layer().async_connect(endpoint,
          boost::bind(&client::connect, this,
            asio::placeholders::error, ++endpoint_iterator));
    }
    else
    {
      std::cout << "conn fail! err: " << error << "\n";
    }
  }

  void handshake(const asio::error_code& error)
  {
    if (!error)
    {
    
      // bitkey command
      std::cout << "command: ";
      std::cin.getline(request_, max_length);
      size_t request_length = strlen(request_);

      asio::async_write(socket_,
          asio::buffer(request_, request_length),
          boost::bind(&client::send, this,
            asio::placeholders::error,
            asio::placeholders::bytes_transferred));
    }
    else
    {
      std::cout << "handshake fail! err: " << error << "\n";
    }
  }

  void transfer(const asio::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      asio::async_read(socket_,
          asio::buffer(reply_, bytes_transferred),
          boost::bind(&client::rec, this,
            asio::placeholders::error,
            asio::placeholders::bytes_transferred));
    }
    else
    {
      std::cout << "transaction fail! err:" << error << "\n";
    }
  }

  void income_dat(const asio::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
    	//INCOMING
      std::cout << "Confirmation: ";
      std::cout.write(confirm_, by_tx);
      std::cout << "\n";
    }
    else
    {
      std::cout << "transmission error! :" << error << "\n";
    }
  }

private:
  asio::ssl::stream<asio::ip::tcp::socket> sslsocket_;
  char request_[ml];
  char confirm_[ml];
};

int main(int argc, char* argv[])
{

    if (argc != 3)  
    {
      std::cerr << "Usage: bitkey <tx-recepient> <port>\n"; //manually define the node 
      return 1;
    }

    asio::io_service io_s; 

    asio::ip::tcp::resolver resolver(io_s);
    asio::ip::tcp::resolver::query query(argv[1], argv[2]);
    asio::ip::tcp::resolver::iterator itr = resolver.resolve(query);

    asio::ssl::context cx(io_s, asio::ssl::context::sslv23);
    cx.set_verify_mode(asio::ssl::context::verify_peer);
    cx.load_verify_file("bitkey.crt");
    
    bitkey_c c(io_s, ctx, itr);
    io_s.run();
  

  return 0;
}
