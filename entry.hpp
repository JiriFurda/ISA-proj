#ifndef __ENTRY_H_INCLUDED__
#define __ENTRY_H_INCLUDED__

#include <iostream>
#include <libxml/parser.h>
#include <unordered_map>
#include <vector>

using namespace std;

class Entry
{
      public:
            Entry(xmlNodePtr cur);
            string toString(unordered_multimap<int, string> flags) const;

      private:
            string title;
            string updated;
            vector<string> authors;
            string url;

            void parseTitle(xmlNodePtr cur);
            void parseUpdated(xmlNodePtr cur);
            void parseDcCreator(xmlNodePtr cur);
            void parseAuthor(xmlNodePtr cur);
            void parseLink(xmlNodePtr cur);

};

#endif // __ENTRY_H_INCLUDED__