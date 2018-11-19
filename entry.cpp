#include "entry.hpp"

Entry::Entry(xmlNodePtr cur)
{
	// Dive into entry node
	cur = cur->xmlChildrenNode;

	// Loop throught nodes in entry
	while(cur != NULL)
	{
		// Parser <title> node
		if(!xmlStrcmp(cur->name, (const xmlChar *)"title"))
		{
			this->parseTitle(cur);
		}
		// Parse <pubDate> node or <updated> node or <dc:date> node
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"pubDate") ||
				!xmlStrcmp(cur->name, (const xmlChar *)"updated") ||
				(
					!xmlStrcmp(cur->name, (const xmlChar *)"date") &&
					cur->ns &&
					!xmlStrcmp(cur->ns->prefix, (const xmlChar *)"dc")
				))
		{
			this->parseUpdated(cur);
		}
		// Parse <dc:creator> node
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"creator") &&
				cur->ns &&
				!xmlStrcmp(cur->ns->prefix, (const xmlChar *)"dc"))
		{
			this->parseDcCreator(cur);
		}
		// Parse <author> node
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"author"))
		{
			this->parseAuthor(cur);
		}
		// Parse <link> node
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"link"))
		{
			this->parseLink(cur);
		}

		// Move to next node
		cur = cur->next;
	}	
}


void Entry::parseTitle(xmlNodePtr cur)
{
	if(cur->children)
		this->title = string((char*)XML_GET_CONTENT(cur->children));
}


void Entry::parseUpdated(xmlNodePtr cur)
{
	if(cur->children)
		this->updated = string((char*)XML_GET_CONTENT(cur->children));
}


void Entry::parseDcCreator(xmlNodePtr cur)
{
	if(cur->children)
		this->authors.push_back(string((char*)XML_GET_CONTENT(cur->children)));
}


void Entry::parseAuthor(xmlNodePtr cur)
{
	// <author> has no children nodes
	if(cur->xmlChildrenNode == NULL)
	{
		if(cur->children)
			this->authors.push_back(string((char*)XML_GET_CONTENT(cur->children)));
	}
	// <author> has children nodes
	else
	{
		// Dive into <author> node
		cur = cur->xmlChildrenNode; 

		// Loop throught nodes in <author> node
		while(cur != NULL)
		{
			// Parse <name> node
			if(!xmlStrcmp(cur->name, (const xmlChar *)"name"))
				this->authors.push_back(string((char*)XML_GET_CONTENT(cur->children)));

			// Move to next node
			cur = cur->next;
		}
	}
}


void Entry::parseLink(xmlNodePtr cur)
{
	xmlChar *href;
	if((href = xmlGetProp(cur, (const xmlChar *)"href")))
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

	// Title
	if(this->title.length())
		output += this->title;
	else
		output += "N/A";
	output += "\n";

	// URL
	if(printURL)
	{
		output += "URL: ";

		if(this->url.length())
			output += this->url;
		else 
			output += "N/A";

		output += "\n";
	}

	// Updated
	if(printUpdated)
	{
		output += "Aktualizace: ";

		if(this->updated.length())
			output += this->updated;
		else 
			output += "N/A";

		output += "\n";
	}

	// Author
	if(printAuthor)
	{
		if(this->authors.size() == 0)
			output += "Autor: N/A\n";

		for(const string& author : this->authors)
		{
			output += "Autor: ";

			if(author.length())
				output += author;
			else 
				output += "N/A";

			output += "\n";
		}
	}
	
	// Return string
	return output;
}