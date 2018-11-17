#ifndef __ENTRY_H_INCLUDED__
#define __ENTRY_H_INCLUDED__

#include <iostream>
#include <libxml/parser.h>

using namespace std;

class Entry
{
      public:
            Entry(xmlNodePtr cur);
            string toString();

      private:
            string title;
            string updated;
            string author;
            string url;

            void parseTitle(xmlNodePtr cur);
            void parseDcDate(xmlNodePtr cur);
            void parsePubDate(xmlNodePtr cur);
            void parseUpdated(xmlNodePtr cur);
            void parseDcCreator(xmlNodePtr cur);
            void parseAuthor(xmlNodePtr cur);
            void parseLink(xmlNodePtr cur);

};

#endif // __ENTRY_H_INCLUDED__