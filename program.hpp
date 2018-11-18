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

class Program
{
      public:
            Program(int argc, char* argv[]);
            unordered_map<int, int> flags;
            vector<Feed> feeds;


      private:
            char* feedFile;

            void processArguments(int argc, char* argv[]);
            void execute();
            void processFeedFile();

};

#endif // __PROGRAM_H_INCLUDED__