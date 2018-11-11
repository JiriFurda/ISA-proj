#include "parser.hpp"

using namespace std;

Parser::Parser(string content)
{
	//cout << content << endl;
	
	
	xmlNodePtr cur;

	this->doc = xmlParseDoc((const xmlChar *)content.c_str());

	if(this->doc == NULL)
	{
		cerr << "Parsing failed" << endl;
		exit(1);
	}

	cur = xmlDocGetRootElement(this->doc);

	if (cur == NULL)
	{
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(this->doc);
		return;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *)"rss") && xmlStrcmp(cur->name, (const xmlChar *)"feed"))
	{
		fprintf(stderr,"document of the wrong type");
		xmlFreeDoc(this->doc);
		return;
	}

	cur = cur->xmlChildrenNode;
	while (cur != NULL)
	{
		if((!xmlStrcmp(cur->name, (const xmlChar *)"channel")))
		{
			cur = cur->xmlChildrenNode;
		}

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

	this->print();
}

void Parser::parseTitle(xmlNodePtr cur)
{
	xmlChar *value = xmlNodeListGetString(this->doc, cur->xmlChildrenNode, 1);
	this->title = string("*** "+string((char*)value)+" ***");
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