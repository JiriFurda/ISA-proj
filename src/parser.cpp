#include "parser.hpp"

using namespace std;

bool Parser::RSS = 0;
bool Parser::Atom = 1;

Parser::Parser(string content)
{
	// Read content
	if(!this->init(content))
	{
		this->valid = false;
		return;
	}
	
	// Determine if RSS or Atom
	if(!this->determineFormat())
	{
		this->valid = false;
		return;
	}

	// Start parsing
	this->parse();

	// Mark as valid
	this->valid = true;
}

bool Parser::init(string content)
{
	// Read content
	this->doc = xmlReadDoc((const xmlChar *)content.c_str(), NULL, NULL, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);

	// Check if XML was not vaild
	if(this->doc == NULL)
	{
		cerr << "ERROR: Cannot parse feed\n";
		return false;
	}

	return true;
}

bool Parser::determineFormat()
{
	xmlNodePtr cur;

	// Get root element
	cur = xmlDocGetRootElement(this->doc);

	// Check for empty document
	if(cur == NULL)
	{
		cerr << "WARNING: Parsed document is empty\n";
		xmlFreeDoc(this->doc);
		return false;
	}

	// Check root element name
	if (!xmlStrcmp(cur->name, (const xmlChar *)"rss"))
		this->format = this->RSS;
	else if(!xmlStrcmp(cur->name, (const xmlChar *)"feed"))
		this->format = this->Atom;
	else
	{
		cerr << "WARNING: Parsed document has wrong type\n";
		xmlFreeDoc(this->doc);
		return false;
	}

	return true;
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

	// Dive into root node
	cur = cur->xmlChildrenNode;

	// Loop throught second level until <channel>
	while(cur != NULL)
	{
		// Check if node is <channel>
		if((!xmlStrcmp(cur->name, (const xmlChar *)"channel")))
		{
			break;
		}

		// Move to next node
		cur = cur->next;
	}		

	// Reached end of the XML and not found
	if(cur == NULL)
	{
		cerr << "WARNING: Node <channel> not found\n";
		return;
	}

	// Parse <channel> node
	this->RSS_parseItemsContainer(cur);
}


void Parser::Atom_parse()
{
	xmlNodePtr cur;

	// Get root node
	cur = xmlDocGetRootElement(this->doc);

	// Dive into root node
	cur = cur->xmlChildrenNode;

	// Loop throught second level
	while(cur != NULL)
	{
		// Parse <title> node
		if(!xmlStrcmp(cur->name, (const xmlChar *)"title"))
		{
			this->parseTitle(cur);	
		}
		// Pase <entry> node
		else if(!xmlStrcmp(cur->name, (const xmlChar *)"entry"))
		{
			this->parseItem(cur);
		}

		// Move to next node
		cur = cur->next;
	}		
}


void Parser::RSS_parseItemsContainer(xmlNodePtr cur)
{
	// Dive into <channel> node
	cur = cur->xmlChildrenNode;

	// Loop throught <channel> node
	while (cur != NULL)
	{
		// Parse <title> node
		if(!xmlStrcmp(cur->name, (const xmlChar *)"title"))
		{
			this->parseTitle(cur);	
		}
		// Pase <entry> node
		else if((!xmlStrcmp(cur->name, (const xmlChar *)"item")))
		{
			this->parseItem(cur);
		}

		// Move to next node
		cur = cur->next;
	}
}


void Parser::parseTitle(xmlNodePtr cur)
{
	if(cur->xmlChildrenNode)
	{
		xmlChar *value = xmlNodeListGetString(this->doc, cur->xmlChildrenNode, 1);
		this->title = string((char*)value);
		xmlFree(value);
	}
}


void Parser::parseItem(xmlNodePtr cur)
{
	// Parse entry
	Entry entry(cur);

	// Save parsed entry
	this->entries.push_back(entry);
}


string Parser::toString(unordered_multimap<int, string> flags)
{
	string output;

	// Title
	if(this->title.length())
		output += "*** " + this->title + " ***\n";
	else
		output += "*** N/A ***\n";

	// Entries
	for(int i=0; i < this->entries.size(); i++)
	{
		output += this->entries[i].toString(flags);

		// Extra line between entries
		if((flags.count('u') || flags.count('T') || flags.count('a')) && i+1 < this->entries.size())
			output += '\n';
	}

	return output;
}