//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER3_REQUEST_HPP
#define HTTP_SERVER3_REQUEST_HPP

#include <string>
#include <vector>
#include "header.hpp"
#include "parameter.hpp"

namespace http {
	namespace server3 {

		namespace request_headers
		{
			const std::string connection_header =
				"Connection";
		}
		namespace request_values{
			const std::string keep_alive_value =
				"keep-alive";
			const std::string close_value =
				"close";
		}

		/// A request received from a client.
		struct request
		{
			std::string method;
			std::string uri;
			int http_version_major;
			int http_version_minor;
			std::vector<header> headers;
			std::vector<parameter> parameters;

			bool get_header_value(const std::string header_str, std::string& header_value_str);

			//input: the name of one header parameter in the request
			//process: loop over the parameter vectors and compare to this string
			//out: true if one of the header parameter contain this business key
			//	   false if otherwise
			bool check_business(const std::string business_key);

		};


	} // namespace server3
} // namespace http

#endif // HTTP_SERVER3_REQUEST_HPP