//
// reply.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request.hpp"
#include <string>
#include <boost/lexical_cast.hpp>

namespace http {
	namespace server3 {

		bool request::get_header_value(const std::string header_str, std::string& header_value_str)
		{
			std::vector<header>::iterator it;
			for ( it = headers.begin(); it < headers.end(); it++ )
			{
				if(it->name.compare(header_str) == 0 )
				{
					header_value_str = it->value;
					return true;
				}
			}
			return false;

		}

		bool request::check_business(const std::string business_key)
		{
			std::vector<parameter>::iterator it;
			size_t found;
			for ( it = parameters.begin(); it < parameters.end(); it++ )
			{
				found = it->name.find(business_key);
				if(found != std::string::npos)
					return true;
			}
			return false;
		}

	} // namespace server3
} // namespace http	