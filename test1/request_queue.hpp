//
// request_queue.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER3_REQUEST_QUEUE_HPP
#define HTTP_SERVER3_REQUEST_QUEUE_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace http {
	namespace server3 {

		/// Represents a single connection from a client.
		class request_queue
			: public boost::enable_shared_from_this<request_queue>,
			private boost::noncopyable
		{
		public:
			/// Construct a connection with the given io_service.
			explicit request_queue(boost::asio::io_service& io_service);

		private:
			/// Handle completion of a read operation.
			void handle_read(const boost::system::error_code& e,
				std::size_t bytes_transferred);

			void reset_timer();

			/// Handle completion of a write operation.
			void handle_write(const boost::system::error_code& e);

			/// Handle for stop connection.
			void handle_stop(const boost::system::error_code& e);

			void stop();

			/// Strand to ensure the connection's handlers are not called concurrently.
			boost::asio::io_service::strand strand_;

		};

		typedef boost::shared_ptr<request_queue> request_queue_ptr;

	} // namespace server3
} // namespace http

#endif // HTTP_SERVER3_REQUEST_QUEUE_HPP