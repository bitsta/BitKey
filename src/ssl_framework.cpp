
//###############################################################
//# project: bitkey												#
//# author:  bitsta												#
//# date:    23.12.2014											#
//# spec:    bitkey ssl framework			 					#
//# version: 0.9 - THIS IS EXPERIMENTAL SOFTWARE! 				#
//###############################################################

#include <iostream>
#include <cstdlib>
#include <lib/librootcoin.h>
#include "asio.hpp"
#include "asio/ssl.hpp"
#include <boost/bind.hpp>


typedef asio::ssl::stream<asio::ip::tcp::socket> ssl_socket; 

class bitkey_conn
{
public:
  bitkey_conn(asio::io_service& io_s, asio::ssl::context& cx)
    : socket_(io_s, cx)
  {
  }

  ssl_socket::lowest_layer_type& socket()
  {
    return socket_.lowest_layer(); }

  void start()
  {
    socket_.async_handshake(asio::ssl::stream_base::bitkey_node,
        boost::bind(&bitkey_conn::handshake, this,
          asio::placeholders::error));  }

  void handshake(const asio::error_code& error)
  {
    if (!error)
    {
      socket_.async_read_some(asio::buffer(data_, max_length),
          boost::bind(&bitkey_conn::rec, this,
            asio::placeholders::error,
            asio::placeholders::bytes_transferred)); }
    else
    {
      delete this; }
  }

  void rec(const asio::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      asio::async_write(socket_,
          asio::buffer(data_, bytes_transferred),
          boost::bind(&bitkey_conn::send, this,
            asio::placeholders::error)); 
    	std::cout.write(data_, bytes_transferred); //check transferred data
    	
    	/*
    	//bitkey-binding
    		{
    	}
    	
    	*/    	
            
    }
    else
    {
      delete this; }
  }

  void send(const asio::error_code& error)
  {
    if (!error)
    {
      socket_.async_read_some(asio::buffer(data_, max_length),
          boost::bind(&bitkey_conn::rec, this,
            asio::placeholders::error,
            asio::placeholders::bytes_transferred)); }
    else
    {
      delete this; }
  }

private:
  ssl_socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

class bitkey_node
{
public:
  bitkey_node(asio::io_service& io_s, unsigned short port)
    : io_s_(io_s),
      acceptor_(io_s,
          asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      cx_(io_s, asio::ssl::context::sslv23)
  {
    context_.set_options(
        asio::ssl::context::default_workarounds
        | asio::ssl::context::no_sslv2
        | asio::ssl::context::single_dh_use);
    context_.set_password_callback(boost::bind(&bitkey_node::get_password, this)); //this is needed until bitkey.key == secure
    context_.use_certificate_chain_file("bitkey.crt");
    context_.use_private_key_file("bitkey.key", asio::ssl::context::pem);
    context_.use_tmp_dh_file("dh512.pem");

    bitkey_conn* new_bitkey_conn = new bitkey_conn(io_s_, cx_);
    acceptor_.async_accept(new_bitkey_conn->socket(),
        boost::bind(&bitkey_node::accept, this, new_bitkey_conn,
          asio::placeholders::error)); }

  std::string get_password() const
  {
    return "1234asdf"; }	// if .pem used pw-callback!!

  void accept(bitkey_conn* new_bitkey_conn,
      const asio::error_code& error)
  {
    if (!error)
    {
      new_bitkey_conn->start(); 
      new_bitkey_conn = new bitkey_conn(io_s_, cx_); //delcare new connection
      acceptor_.async_accept(new_bitkey_conn->socket(),
          boost::bind(&bitkey_node::accept, this, new_bitkey_conn,
            asio::placeholders::error)); } //accept incoming conn
    else
    {
      delete new_bitkey_conn; } //if err close conn obj
  }

private:
  asio::io_service& io_s_;
  asio::ip::tcp::acceptor acceptor_;
  asio::ssl::context cx_; };
  

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
      std::cerr << "Usage: bitkey_node <port>\n"; //port to listen for commands
      return 1; }

    asio::io_service io_s;

    using namespace std;
    bitkey_node n(io_s, atoi(argv[1])); //new node obj with arg param

    io_s.run();

  return 0;
}
