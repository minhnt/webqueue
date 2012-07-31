//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"

namespace http {
	namespace server3 {
		
		std::string business_key = "adm";

		request_handler::request_handler(const std::string& doc_root)
			: doc_root_(doc_root)
		{
		}

		void request_handler::handle_request(request& req, reply& rep)
		{

			// parsing url into parameter and its corresponding value
			uri_process(req.uri,req.parameters);

			if(req.check_business(business_key)){
				std::cout << "Found business adm";
			}

			// Decode url to path.
			std::string request_path;
			if (!url_decode(req.uri, request_path))
			{
				rep = reply::stock_reply(reply::bad_request);
				return;
			}

			// Request path must be absolute and not contain "..".
			if (request_path.empty() || request_path[0] != '/'
				|| request_path.find("..") != std::string::npos)
			{
				rep = reply::stock_reply(reply::bad_request);
				return;
			}

			// If path ends in slash (i.e. is a directory) then add "index.html".
			if (request_path[request_path.size() - 1] == '/')
			{
				request_path += "index.html";
			}

			// Determine the file extension.
			std::size_t last_slash_pos = request_path.find_last_of("/");
			std::size_t last_dot_pos = request_path.find_last_of(".");
			std::string extension;
			if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
			{
				extension = request_path.substr(last_dot_pos + 1);
			}

			// Open the file to send back.
			std::string full_path = doc_root_ + request_path;
			std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
			if (!is)
			{
				rep = reply::stock_reply(reply::not_found);
				return;
			}

			// Fill out the reply to be sent to the client.
			rep.status = reply::ok;
			char buf[512];
			while (is.read(buf, sizeof(buf)).gcount() > 0)
				rep.content.append(buf, is.gcount());
			rep.headers.resize(2);
			rep.headers[0].name = "Content-Length";
			rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
			rep.headers[1].name = "Content-Type";
			rep.headers[1].value = mime_types::extension_to_type(extension);
		}

		bool request_handler::url_decode(const std::string& in, std::string& out)
		{
			out.clear();
			out.reserve(in.size());
			for (std::size_t i = 0; i < in.size(); ++i)
			{
				if (in[i] == '%')
				{
					if (i + 3 <= in.size())
					{
						int value = 0;
						std::istringstream is(in.substr(i + 1, 2));
						if (is >> std::hex >> value)
						{
							out += static_cast<char>(value);
							i += 2;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else if (in[i] == '+')
				{
					out += ' ';
				}
				else
				{
					out += in[i];
				}
			}
			return true;
		}

		bool request_handler::url_decode(std::string& encodeString)
		{
			std::string out;			
			//return url_decode(encodeString, out);
			bool result = url_decode(encodeString, out);
			//std::cout<<"\n encoded string: "<<encodeString<<"\tdecoded String: "<<out;
			encodeString = out;
			return result;
		}


		bool request_handler::uri_process(std::string& iString, std::vector<parameter>& parameters){

			//Bo dau / o dau uri
			if(iString[0] = '/')
				iString = iString.substr(1,iString.length());

			//Luu tru cac parameter bao gom: name (ten parameter), value (gia tri cua parameter)
			//Mang chua decode cac parameter
			std::vector<parameter> e_parameters;
			//Mang cac parameter (da decode)

			//Chia theo method bang dau ?
			std::vector<std::string> tempVector;
			splitWithouEmpty(iString,'?',tempVector);


			if(tempVector.size() > 0)
			{
				parameter temp_para;
				temp_para.name = "method_resource";
				temp_para.value = tempVector[0];
				e_parameters.push_back(temp_para);
			}

			//Chia theo parameter bang dau &
			if(tempVector.size() > 1){
				std::string paraString = tempVector[1];
				tempVector.clear();
				splitWithouEmpty(paraString, '&', tempVector);

				std::cout<<"\n\n----------\nSau khi chia theo &\n-----------\n";
				
				for(int i= 0; i < tempVector.size(); i++)
				{
					std::string para = tempVector[i];
					std::vector<std::string> paraVector;

					//Lay thong tin parameter bao gom name va value ngan cach boi dau =
					splitWithouEmpty(para, '=', paraVector);
					if(paraVector.size() != 2)
					{
						return false;
					}
					else
					{
						//e_parameters.insert(std::pair<std::string, std::string>(paraVector[0], paraVector[1]));
						parameter temp_para;
						temp_para.name = paraVector[0];
						temp_para.value = paraVector[1];
						e_parameters.push_back(temp_para);
					}
				}
			}

			

			//decode parameters:
			for(std::vector<parameter>::iterator p = e_parameters.begin(); p != e_parameters.end(); ++p)
			{
				std::string paraName = p->name;
				if(!url_decode(paraName))
					return false;		

				std::string paraValue = p->value;
				if(!url_decode(paraValue))
					return false;	
				parameter para;
				para.name = paraName;
				para.value = paraValue;
				parameters.push_back(para);
			}
			
		}


	} // namespace server3
} // namespace http