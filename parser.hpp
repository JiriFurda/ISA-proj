#ifndef __PARSER_H_INCLUDED__
#define __PARSER_H_INCLUDED__

#include <iostream>
#include <libxml/parser.h>

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

            static bool RSS;
            static bool Atom;

            void init(string content);
            void determineFormat();
            void parse();
            void parseItemsContainer(xmlNodePtr cur);
            void parseTitle(xmlNodePtr cur);
            void parseItem(xmlNodePtr cur);
            void print();
};

#endif // __PARSER_H_INCLUDED__