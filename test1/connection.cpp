//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <vector>
#include <boost/bind.hpp>
#include "request_handler.hpp"
#include "request.cpp"

//this comment has been added by KhanhHH 11h21 
namespace http {
	namespace server2 {

		connection::connection(boost::asio::io_service& io_service,
			request_handler& handler)
			: socket_(io_service),
			request_handler_(handler)
		{
		}

		boost::asio::ip::tcp::socket& connection::socket()
		{
			return socket_;
		}

		void connection::start()
		{
			socket_.async_read_some(boost::asio::buffer(buffer_),
				boost::bind(&connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}

		void connection::handle_read(const boost::system::error_code& e,
			std::size_t bytes_transferred)
		{
			std::string str_connection_value;
			if (!e)
			{
				boost::tribool result;
				boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
					request_, buffer_.data(), buffer_.data() + bytes_transferred);

				/// if parse successfully
				if (result)
				{
					if(request_.get_header_value(request_string::connection_header, str_connection_value)
						&& str_connection_value.compare(request_string::connection_value))
					{
						/// start timer.
						/// stop connection after 20s if there isn't any connection during this time.

					}
					request_handler_.handle_request(request_, reply_);
					boost::asio::async_write(socket_, reply_.to_buffers(),
						boost::bind(&connection::handle_write, shared_from_this(),
						boost::asio::placeholders::error));
				}
				else if (!result)
				{
					reply_ = reply::stock_reply(reply::bad_request);
					boost::asio::async_write(socket_, reply_.to_buffers(),
						boost::bind(&connection::handle_write, shared_from_this(),
						boost::asio::placeholders::error));
				}
				else
				{
					socket_.async_read_some(boost::asio::buffer(buffer_),
						boost::bind(&connection::handle_read, shared_from_this(),
						boost::asio::placeholders::error,\
						boost::asio::placeholders::bytes_transferred));
				}
			}

			// If an error occurs then no new asynchronous operations are started. This
			// means that all shared_ptr references to the connection object will
			// disappear and the object will be destroyed automatically after this
			// handler returns. The connection class's destructor closes the socket.
		}

		void connection::handle_write(const boost::system::error_code& e)
		{
			if (!e)
			{
				// Initiate graceful connection closure.
				boost::system::error_code ignored_ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			}

			// No new asynchronous operations are started. This means that all shared_ptr
			// references to the connection object will disappear and the object will be
			// destroyed automatically after this handler returns. The connection class's
			// destructor closes the socket.
		}

	} // namespace server2
} // namespace http