
#include "request.hpp"
#include <string>
#include <boost/lexical_cast.hpp>

namespace http {
	namespace server2 {

		namespace request_string{
			const std::string connection_header = 
				"Connection";
			const std::string connection_value =
				"keep-alive";
		}

		bool request::get_header_value(const std::string header_name, std::string& header_value)
		{
			std::vector<header>::iterator it;
			for (it = headers.begin(); it < headers.end();it++)
			{
				if((*it).name.compare(header_name))
				{
					header_value = (*it).value;
					return true;
				}
			}
			return false;
		}
	}
}