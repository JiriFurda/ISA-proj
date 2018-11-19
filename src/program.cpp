#include "program.hpp"


Program::Program(int argc, char* argv[])
{
	this->processArguments(argc, argv);
	this->execute();
}


void Program::processArguments(int argc, char* argv[])
{
	// Check for no parameters
	if(argc == 1)
		this->printHelpExit();

	// Reorder arguments for server eva
	if(argc > 1 && argv[1][0] != '-')
	{
		char* url = argv[1];

		for(int i = 2; i < argc; i++)
		{
			argv[i-1] = argv[i];
		}

		argv[argc-1] = url;
	}

	// Read arguments
	int opt;
	while((opt = getopt(argc, argv, "f:c:C:Tau")) != -1)
	{
		switch(opt)
		{
			// Flags without value
			case 'T':
			case 'a':
			case 'u':
				this->flags.insert({opt, ""});
				break;

			// Flags with value
			case 'f':
			case 'c':
			case 'C':
				this->flags.insert({opt, optarg});
				break;

			// Wrong flag
			default: 
				this->printHelpExit();
				break;
	   }
	}


	// Check if URL is specified
	if(optind < argc)
	{
		// Check wrong arguments combination
		if(this->flags.count('f'))
		{
			cerr << "Error: Cannot combine URL parameter with -f option";
			exit(1);			
		}

		// Save the URL to feeds
		this->feeds.push_back(Feed(this, argv[optind]));
	}


	// Check if feedfile was specified
	if(this->flags.count('f'))
	{
		this->processFeedFile();			
	}	
}

void Program::printHelpExit()
{
	cerr << "Usage: feedreader <URL | -f <feedfile>> [-c <certfile>] [-C <certaddr>] [-T] [-a] [-u]\n";
	exit(2);
}

void Program::execute()
{
	// Read every feed used
	for(int i=0; i < this->feeds.size(); i++)
	{
		// Print only if feed is valid
		if(feeds[i].valid)
		{
			// Print entries in the feed
			if(!feeds[i].read())
				continue; // Skip new line if something failed

			// Add extra new line after the feed except very last feed
			if(i+1 < this->feeds.size())
				cout << "\n";
		}
	}
}

void Program::processFeedFile()
{
	// Loop throught every flag
	for(auto const& flag: this->flags)
	{
		// Skip if not -f flag
		if(flag.first != 'f')
			continue; // I hate C++

		// Read file
		ifstream file(flag.second);
		if(file.is_open())
		{
		    string line;
		    
		    // Read line by line
		    while(getline(file, line))
		    {
		    	// Skip commentaries and empty lines
		    	smatch matches;
				if(regex_search(line, matches, regex("^\\s*#")) ||
					line.length() == 0)
				{
			        continue;
				}

				// Save feed URL
		        this->feeds.push_back(Feed(this, line));
		    }
		    
		    // Close file
		    file.close();
		}
		else
		{
			fprintf(stderr, "Error: File problem");
			exit(1);
		}
	}
}