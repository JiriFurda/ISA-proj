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
            string title;
            string output;
            xmlDocPtr doc;

            void parseTitle(xmlNodePtr cur);
            void parseItem(xmlNodePtr cur);
            void print();
};

#endif // __PARSER_H_INCLUDED__