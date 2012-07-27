//
// request_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER3_REQUEST_HANDLER_HPP
#define HTTP_SERVER3_REQUEST_HANDLER_HPP

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include "parameter.hpp"
namespace http {
	namespace server3 {

		struct reply;
		struct request;

		/// The common handler for all incoming requests.
		class request_handler
			: private boost::noncopyable
		{
		public:
			/// Construct with a directory containing files to be served.
			explicit request_handler(const std::string& doc_root);

			/// Handle a request and produce a reply.
			void handle_request(const request& req, reply& rep);

		private:
			/// The directory containing the files to be served.
			std::string doc_root_;

			/// input: 
			///		iString: request.uri
			/// process:
			///		This function will extract parameter and its values
			///		and then set them into the vector<parameter>
			/// return
			///		false: if the uri is in the wrong format
			///		true:  ok
			bool uri_process(std::string& iString, std::vector<parameter>& parameters);

			/// Perform URL-decoding on a string. Returns false if the encoding was
			/// invalid.
			static bool url_decode(const std::string& in, std::string& out);
			//
			bool url_decode(std::string& encodeString);

			///chia string s thanh cac substring ngan cach boi ky tu c vao vector v
			void splitWithouEmpty(const std::string& s, char c,
				std::vector<std::string>& v) {
					std::string::size_type i = 0;
					std::string::size_type j = s.find(c);

					while (j != std::string::npos) {
						if(i!=j){
							v.push_back(s.substr(i, j-i));
						}
						i = ++j;
						j = s.find(c, j);

						if (j == std::string::npos){
							v.push_back(s.substr(i, s.length( )));
						}
					}
			}
		};

	} // namespace server3
} // namespace http

#endif // HTTP_SERVER3_REQUEST_HANDLER_HPP	