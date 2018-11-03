#ifndef __FEED_H_INCLUDED__
#define __FEED_H_INCLUDED__

#include <iostream>
#include <regex>
#include <string>
#include  "openssl/bio.h"
#include  "openssl/ssl.h"
#include  "openssl/err.h"

using namespace std;

class Feed
{
   public:
      string host;
      string path;
      int port;
      Feed(string url);
      bool determinePort(string url);
      bool determineHost(string url);
      bool determinePath(string url);
      void print();
      void read();
};

#endif // __FEED_H_INCLUDED__