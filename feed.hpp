#ifndef __FEED_H_INCLUDED__
#define __FEED_H_INCLUDED__

#include <iostream>
#include <regex>
#include <string>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "parser.hpp"

using namespace std;

class Feed
{
      public:
            Feed(string url);
            void read();
            void dumpInfo();


      private:
            string host;
            string path;
            int port;
            
            bool determinePort(string url);
            bool determineHost(string url);
            bool determinePath(string url);  
            
            void connectHost(BIO **bio);
            void sendRequest(BIO **bio);
            string readResponse(BIO **bio);
            string discardHeader(string content);

            void parse(string content);
};

#endif // __FEED_H_INCLUDED__