#include "parser.hpp"

using namespace std;

bool Parser::RSS = 0;
bool Parser::Atom = 1;

Parser::Parser(string content)
{
	xmlNodePtr cur;

	this->init(content);
	this->determineFormat();
	this->parse();
}

void Parser::init(string content)
{
	this->doc = xmlParseDoc((const xmlChar *)content.c_str());

	if(this->doc == NULL)
	{
		cerr << "Parsing failed" << endl;
		exit(1);
	}
}

void Parser::determineFormat()
{
	xmlNodePtr cur;
	cur = xmlDocGetRootElement(this->doc);

	if(cur == NULL)
	{
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(this->doc);
		return;
	}

	if (!xmlStrcmp(cur->name, (const xmlChar *)"rss"))
		this->format = this->RSS;
	else if(!xmlStrcmp(cur->name, (const xmlChar *)"feed"))
		this->format = this->Atom;
	else
	{
		fprintf(stderr,"document of the wrong type");
		xmlFreeDoc(this->doc);
		return;
	}
}

void Parser::parse()
{
	if(this->format == this->RSS)
		this->RSS_parse();
	else
		this->Atom_parse();
}


void Parser::RSS_parse()
{
	xmlNodePtr cur;

	// Get root node
	cur = xmlDocGetRootElement(this->doc);


	// Get node containing items
	cur = cur->xmlChildrenNode;

	// Loop throught second level until <channel>
	while(cur != NULL)
	{
		if((!xmlStrcmp(cur->name, (const xmlChar *)"channel")))
		{
			break;
		}

		cur = cur->next;
	}		

	if(cur == NULL)
	{
		fprintf(stderr,"Items container not found");
		return;
	}

	this->RSS_parseItemsContainer(cur);
}


void Parser::Atom_parse()
{
	xmlNodePtr cur;

	// Get root node
	cur = xmlDocGetRootElement(this->doc);


	cur = cur->xmlChildrenNode;

	// Loop throught second level
	while(cur != NULL)
	{
		
		if(!xmlStrcmp(cur->name, (const xmlChar *)"title"))
		{
			this->parseTitle(cur);	
		}
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"entry"))
		{
			this->parseItem(cur);
		}

		cur = cur->next;
	}		
}

void Parser::RSS_parseItemsContainer(xmlNodePtr cur)
{

	cur = cur->xmlChildrenNode;

	while (cur != NULL)
	{
		if(!xmlStrcmp(cur->name, (const xmlChar *)"title"))
		{
			this->parseTitle(cur);	
		}
		else if((!xmlStrcmp(cur->name, (const xmlChar *)"item")))
		{
			this->parseItem(cur);
		}

		cur = cur->next;
	}
}

void Parser::parseTitle(xmlNodePtr cur)
{
	xmlChar *value = xmlNodeListGetString(this->doc, cur->xmlChildrenNode, 1);
	this->title = "*** "+string((char*)value)+" ***";
	xmlFree(value);
}

void Parser::parseItem(xmlNodePtr cur)
{
	Entry entry(cur);
	this->entries.push_back(entry);

	//cout << entry.toString();
	/*
	if(!xmlStrcmp(cur->name, (const xmlChar *)"item")) // RSS
	{
		cur = cur->xmlChildrenNode;
		while(cur != NULL)
		{
			if((!xmlStrcmp(cur->name, (const xmlChar *)"title")))
			{
				xmlChar *value = xmlNodeListGetString(this->doc, cur->xmlChildrenNode, 1);
				this->output += string((char*)value) + "\n";
				xmlFree(value);	
			}
			else if((!xmlStrcmp(cur->name, (const xmlChar *)"item")))
			{
				this->parseItem(cur);
			}

			cur = cur->next;
		}
	}
	else
	*/
}

string Parser::toString(unordered_multimap<int, string> flags)
{
	string output;

	if(this->title.length())
		output += this->title + "\n";

	for(auto const& entry: this->entries)
	{
    	output += entry.toString(flags);
	}

	return output;
}