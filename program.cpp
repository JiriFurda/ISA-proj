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
}


void Program::execute()
{
	for(auto & feed : this->feeds)
	{
		feed.read(this->flags);
	}
}