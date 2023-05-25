#include <iostream>

#include "HttpConnect.h"

/***
 * 
 * Constructor and Destructor
*/
HttpConnect::HttpConnect()
{
}
HttpConnect::~HttpConnect()
{
}

/**
 * PostSendJSONToAIServerAndRetContent
 * 
 **/
std::string HttpConnect::PostSendJSONToAIServerAndRetContent(
    const std::string host, int port,
    const std::string path, const std::string json_str){
    return "";
}


// type = 0 Get One tcp Package Msg
// type = 1 Get All Content-length Msg
// type = 2 Get Nothing Msg
void HttpConnect::SocketHttp(
    const std::string host, int port, std::string buff,
    int len, int type){
}
