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

using boost::asio::deadline_timer;

namespace http {
	namespace server3 {
		int time_out =  100000;
		int connection::num_connection= 0;
		connection::connection(boost::asio::io_service& io_service,
			request_handler& handler)
			: strand_(io_service),
			socket_(io_service),
			request_handler_(handler),
			timer_(io_service)
		{
			std::cout<< "\n New Connection: " << num_connection;
			soc_id = num_connection;
			num_connection++;
		}

		boost::asio::ip::tcp::socket& connection::socket()
		{
			return socket_;
		}

		void connection::start()
		{
			/// std::cout << "\nStart socket: " <<soc_id;
			socket_.async_read_some(boost::asio::buffer(buffer_),
				strand_.wrap(
					boost::bind(&connection::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)));

			/// shutdown connection if there's not request after timeout.
			timer_.expires_from_now(boost::posix_time::seconds(time_out));
			timer_.async_wait(
				strand_.wrap(
				boost::bind(&connection::handle_stop, shared_from_this(),
				boost::asio::placeholders::error)));
		}

		/// handler for processing data from socket.
		void connection::handle_read(const boost::system::error_code& e,
			std::size_t bytes_transferred)
		{
			std::cout << "\n Receive data from socket: " <<soc_id;
			std::string connection_value_str;
			if (!e)
			{
				boost::tribool result;
				boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
					request_, buffer_.data(), buffer_.data() + bytes_transferred);

				if (result)
				{
					/// std::cout << "\nGood request to socket: " <<soc_id;

					// if this request contains keep-alive connection:token --> start timer expire after 10s 
					if(request_.get_header_value(request_headers::connection_header, connection_value_str))
					{
						
						if(connection_value_str.compare(request_values::keep_alive_value)==0)
						{
							/// if connection header == keep-alive 
							/// ==> Do not close connection.
							keep_alive_ = true; 
						/// if connection == close|anything else --> close connection and connection:close to the response
						}else{
							keep_alive_ = false;
						}
					}else{
					/// if there's no connection header --> close connection after response.
						keep_alive_ = false;
					}

					if(!keep_alive_)
					{
						reply_.add_header(request_headers::connection_header, request_values::close_value);
					}

					/// prepare replay for this request
					request_handler_.handle_request(request_, reply_);

					/// response to the client
					boost::asio::async_write(socket_, reply_.to_buffers(),
						strand_.wrap(
						boost::bind(&connection::handle_write, shared_from_this(),
						boost::asio::placeholders::error)));
				}
				else if (!result)
				{
					/// if bad request ==> add connection:close header ==> resonse and then stop connection.
					keep_alive_ = false;
					reply_ = reply::stock_reply(reply::bad_request);
					reply_.add_header(request_headers::connection_header, request_values::close_value);
					
					boost::asio::async_write(socket_, reply_.to_buffers(),
						strand_.wrap(
						boost::bind(&connection::handle_write, shared_from_this(),
						boost::asio::placeholders::error)));
					
				}
				else
				{
					std::cout << "\nUndefined";
					socket_.async_read_some(boost::asio::buffer(buffer_),
						strand_.wrap(
						boost::bind(&connection::handle_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred)));
				}


			}else{
			// If an error occurs then no new asynchronous operations are started. This
			// means that all shared_ptr references to the connection object will
			// disappear and the object will be destroyed automatically after this
			// handler returns. The connection class's destructor closes the socket.
				stop();
			}
		}/// end handle_read

		void connection::handle_write(const boost::system::error_code& e)
		{
			//if no error and no demand for closing connection.
			if (!e && keep_alive_)
			{
				std::cout <<"\nWrite successfully to socket: " << soc_id;
				
				/// reset parse state and replay
				request_parser_.reset();

				/// keep-alive --> start waiting for ending again.
				timer_.expires_from_now(boost::posix_time::seconds(time_out));
				timer_.async_wait(
					strand_.wrap(
					boost::bind(&connection::handle_stop, shared_from_this(),
					boost::asio::placeholders::error)));

				buffer_.empty();

				/// size_t size_ = socket_.available();
				/// std::cout << "\n Number of byte in socket "<< soc_id << " = " << size_;
				socket_.async_read_some(boost::asio::buffer(buffer_),
					strand_.wrap(
					boost::bind(&connection::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)));
			}else
			{
				std::cout << "\nWrite failed to socket: " << soc_id;
				stop();
			}

			// No new asynchronous operations are started. This means that all shared_ptr
			// references to the connection object will disappear and the object will be
			// destroyed automatically after this handler returns. The connection class's
			// destructor closes the socket.
		}

		void connection::handle_stop(const boost::system::error_code& e)
		{
			// Check whether the deadline has passed. We compare the deadline against
			// the current time since a new asynchronous operation may have moved the
			// deadline before this actor had a chance to run.
			if(e == boost::asio::error::operation_aborted)
			{
				std::cout << "\n Handle stop was canceled: "<<e.message();
			}else
			{
				std::cout << "\n Handle was expired: " <<e.message();
				// The deadline has passed. The socket is closed so that any outstanding
				// asynchronous operations are canceled.
				// Initiate graceful connection closure.
				stop();
				// There is no longer an active deadline. The expiry is set to positive
				// infinity so that the actor takes no action until a new deadline is set.
				timer_.expires_at(boost::posix_time::pos_infin);
				// Put the actor back to sleep.
				timer_.async_wait(
					strand_.wrap(
					boost::bind(&connection::handle_stop, shared_from_this(),
					boost::asio::placeholders::error)));
			}
		}
		void connection::stop()
		{
			--num_connection;
			std::cout << "\nShutdown connection: " << num_connection;
			boost::system::error_code ignored_ec;
			socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
		}

	} // namespace server3
} // namespace http