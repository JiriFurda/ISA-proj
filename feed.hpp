#ifndef __FEED_H_INCLUDED__
#define __FEED_H_INCLUDED__

#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "parser.hpp"
#include "program.hpp"

using namespace std;

class Program;

class Feed
{
      public:
            Feed(Program *program, string url);
            void read();
            void dumpInfo();


      private:
            string host;
            string path;
            int port;
            Program *program;
            
            
            bool determinePort(string url);
            bool determineHost(string url);
            bool determinePath(string url);  
            
            void connectHost(BIO **bio);
            void connectHttpHost(BIO **bio);
            void connectHttpsHost(BIO **bio);
            void setupCertificates(SSL_CTX **ctx);
            void sendRequest(BIO **bio);
            string readResponse(BIO **bio);
            string discardHeader(string content);
            void parse(string content, unordered_multimap<int, string> flags);
};

#endif // __FEED_H_INCLUDED__