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
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"creator") &&
				cur->ns &&
				!xmlStrcmp(cur->ns->prefix, (const xmlChar *)"dc"))
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
		output += this->title;
	else
		output += "N/A";
	output += "\n";


	if(printURL)
	{
		output += "URL: ";

		if(this->url.length())
			output += this->url;
		else 
			output += "N/A";

		output += "\n";
	}


	if(printUpdated)
	{
		output += "Aktualizace: ";

		if(this->updated.length())
			output += this->updated;
		else 
			output += "N/A";

		output += "\n";
	}


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
	
	
	return output;
}