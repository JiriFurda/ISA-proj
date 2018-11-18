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
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"dc:date") ||
				!xmlStrcmp(cur->name, (const xmlChar *)"pubDate") ||
				!xmlStrcmp(cur->name, (const xmlChar *)"updated"))
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
}

void Entry::parseUpdated(xmlNodePtr cur)
{
	this->updated = string((char*)XML_GET_CONTENT(cur->children));
}

void Entry::parseDcCreator(xmlNodePtr cur)
{
	this->authors.push_back(string((char*)XML_GET_CONTENT(cur->children)));
}

void Entry::parseAuthor(xmlNodePtr cur)
{
	if(cur->xmlChildrenNode == NULL)
	{
		this->authors.push_back(string((char*)XML_GET_CONTENT(cur->children)));
	}
	else
	{
		cur = cur->xmlChildrenNode; // Dive into <author> tag

		while(cur != NULL)
		{
			if(!xmlStrcmp(cur->name, (const xmlChar *)"name"))
				this->authors.push_back(string((char*)XML_GET_CONTENT(cur->children)));

			cur = cur->next;
		}
	}
}

void Entry::parseLink(xmlNodePtr cur)
{
	xmlChar *href;
	if(href = xmlGetProp(cur, (const xmlChar *)"href"))
	{
		this->url = string((char*)href);
	}
}

string Entry::toString(unordered_multimap<int, string> flags) const
{
	string output = "";
	bool printURL = flags.count('u');
	bool printUpdated = flags.count('T');
	bool printAuthor = flags.count('a');
	
	if(this->title.length())
	{
		output += this->title + '\n';

		if(printURL && this->url.length())
			output += "URL: " + this->url + '\n';

		if(printUpdated && this->updated.length())
			output += "Aktualizace: " + this->updated + '\n';

		if(printAuthor)
		{
			for(const string& author : this->authors) {
			  output += "Autor: " + author + '\n';
			}
		}

		// Extra line between entries
		if(printURL || printUpdated || printAuthor)
			output += '\n';
	}
	
	return output;
}