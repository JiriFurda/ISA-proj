#ifndef __PROGRAM_H_INCLUDED__
#define __PROGRAM_H_INCLUDED__

#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#include <getopt.h>
#include <fstream>
#include <regex>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

#include "parser.hpp"
#include "feed.hpp"

using namespace std;

class Feed;

class Program
{
      public:
            Program(int argc, char* argv[]);
            unordered_multimap<int, string> flags;
            vector<Feed> feeds;


      private:
            void processArguments(int argc, char* argv[]);
            void execute();
            void processFeedFile();
            void printHelpExit();

};

#endif // __PROGRAM_H_INCLUDED__