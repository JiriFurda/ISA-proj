#include "entry.hpp"

Entry::Entry(xmlNodePtr cur)
{
	// Get node containing items
	cur = cur->xmlChildrenNode;

	while(cur != NULL)
	{
		if(!xmlStrcmp(cur->name, (const xmlChar *)"title"))
		{
			this->parseTitle(cur);
		}
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"dc:date"))
		{
			this->parseDate(cur);
		}
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"pubDate"))
		{
			this->parsePubDate(cur);
		}
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"updated"))
		{
			this->parseUpdated(cur);
		}
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"dc:creator"))
		{
			this->parseDcCreator(cur);
		}
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"author"))
		{
			this->parseAuthor(cur);
		}
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"link"))
		{
			this->parseLink(cur);
		}

		cur = cur->next;
	}	
}

void Entry::parseTitle(xmlNodePtr cur)
{
	this->title = string((char*)XML_GET_CONTENT(cur->children));
	/*
	xmlChar *value = xmlNodeListGetString(this->doc, cur->xmlChildrenNode, 1);
	this->title = string((char*)value);
	xmlFree(value);
	*/
}

void Entry::parseDate(xmlNodePtr cur)
{}

void Entry::parsePubDate(xmlNodePtr cur)
{}

void Entry::parseUpdated(xmlNodePtr cur)
{}

void Entry::parseDcCreator(xmlNodePtr cur)
{}

void Entry::parseAuthor(xmlNodePtr cur)
{}

void Entry::parseLink(xmlNodePtr cur)
{}

string Entry::toString()
{
	string output = "";
	
	//if(this->title != NULL)
	output += this->title;
	output += '\n';
	return output;
}