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

	this->print();
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
	xmlNodePtr cur;

	// Get root node
	cur = xmlDocGetRootElement(this->doc);


	// Get node containing items
	if(this->format == this->RSS)	
	{
		cur = cur->xmlChildrenNode;

		while(cur != NULL)
		{
			if((!xmlStrcmp(cur->name, (const xmlChar *)"channel")))
			{
				break;
			}

			cur = cur->next;
		}		
	}

	if(cur == NULL)
	{
		fprintf(stderr,"Items container not found");
		return;
	}

	this->parseItemsContainer(cur);
}

void Parser::parseItemsContainer(xmlNodePtr cur)
{

	cur = cur->xmlChildrenNode;

	while (cur != NULL)
	{
		if((!xmlStrcmp(cur->name, (const xmlChar *)"title")))
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

void Parser::print()
{
	if(this->title.length())	// != NULL throws strange error, god please don't make me use C++ again.
		cout << this->title << endl;

	if(this->output.length())
		cout << this->output << endl;
	
}