#include "program.hpp"

Program::Program(int argc, char* argv[])
{
	string url;


	this->processArguments(argc, argv);

	this->execute();

}

void Program::processArguments(int argc, char* argv[])
{
	int opt;
	while((opt = getopt(argc, argv, "f:c:C:Tau")) != -1)
	{
		switch(opt)
		{
			case 'T':
			case 'a':
			case 'u':
				this->flags[opt] = true;
				break;
			case 'f':
				this->feedFile = optarg;
			case 'c':
			case 'C':
				this->flags[opt] = true;
				//flags[opt] = optarg;
				break;
			default: // '?' 
				fprintf(stderr, "Usage: %s <URL | -f <feedfile>> [-c <certfile>] [-C <certaddr>] [-T] [-a] [-u]\n", argv[0]);
				exit(1);
	   }
	}

	if(optind < argc)	// has URL parameter
	{
		if(this->flags['f'])
		{
			fprintf(stderr, "Error: Cannot combine URL parameter with -f option");
			exit(1);			
		}

		this->feeds.push_back(Feed(argv[optind]));
	}


	if(this->flags['f'])
	{
		this->processFeedFile();			
	}	
}


void Program::execute()
{
	for(int i=0; i < this->feeds.size(); i++)
	{
		feeds[i].read(this->flags);

		if(i+1 < this->feeds.size())
			cout << "\n";
	}
}

void Program::processFeedFile()
{
	ifstream file(this->feedFile);

	if(file.is_open())
	{
	    string line;
	    
	    while(getline(file, line))
	    {
	    	smatch matches;
			if(regex_search(line, matches, regex("^\\s*#")))
			{
		        continue;
			}


	        this->feeds.push_back(Feed(line));
	    }
	    
	    file.close();
	}
	else
	{
		fprintf(stderr, "Error: File problem");
		exit(1);
	}
}