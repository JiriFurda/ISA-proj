#ifndef __PARSER_H_INCLUDED__
#define __PARSER_H_INCLUDED__

#include <iostream>
#include <vector>
#include <libxml/parser.h>
#include "entry.hpp"

using namespace std;

class Parser
{
      public:
            Parser(string content);

      private:
            bool format;
            string title;
            string output;
            xmlDocPtr doc;
            vector<Entry> entries;

            static bool RSS;
            static bool Atom;

            void init(string content);
            void determineFormat();
            void parse();
            void RSS_parse();
            void Atom_parse();
            void RSS_parseItemsContainer(xmlNodePtr cur);
            void parseTitle(xmlNodePtr cur);
            void parseItem(xmlNodePtr cur);
            void print();
};

#endif // __PARSER_H_INCLUDED__