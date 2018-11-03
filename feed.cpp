#include "feed.hpp"

using namespace std;

Feed::Feed(string url)
{
	this->determinePort(url);
	this->determineHost(url);
	this->determinePath(url);
}

bool Feed::determinePort(string url)
{
	if(regex_match(url, regex("^http://.*$")))
		this->port = 80;
	else if(regex_match(url, regex("^https://.*$")))
		this->port = 443;
	else
		return false;

	return true;
}

bool Feed::determineHost(string url)
{
	smatch matches;
	if(regex_search(url, matches, regex("(?:^http://|^https://)([^/]+)")))
	{
        if(matches.size() == 2)
        	this->host = matches[1];
    	else
    		return false;
	}
	else
		return false;

	return true;
}

bool Feed::determinePath(string url)
{
	smatch matches;
	if(regex_search(url, matches, regex("(?:^http://|^https://)[^/]*/(.+)")))
	{
        if(matches.size() == 2)
        	this->path = matches[1];
    	else
    	{
    		cerr << "aaa";
    		return false;
    	}
	}

	return true;
}

void Feed::print()
{
	cout << this->host << "\n";
	cout << this->port << "\n";
	cout << this->path << "\n";
}

void Feed::read()
{
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
	
	BIO * bio;

	bio = BIO_new_connect(string(this->host+":"+to_string(this->port)).c_str());
	if(bio == NULL)
	{
		fprintf(stderr, "Error: Couldn't create connection");
		exit(1);		
	}



	if(BIO_do_connect(bio) <= 0)
	{
		fprintf(stderr, "Error: Couldn't connect");
		exit(1);		
	}


	char buffer[1024];	
	memset(buffer, '\0', sizeof(buffer)); 	// Earse buffer

	printf("Started reading\n");

	string httpRequest("GET /"+this->path+" HTTP/1.1\r\n"
					"HOST: "+this->host+"\r\n"
					"Connection: close\r\n\r\n");

    const char *httpRequestC = httpRequest.c_str();


    if(BIO_write(bio, httpRequestC, strlen(httpRequestC)) <= 0)
    {
            if(! BIO_should_retry(bio))
            {
                    /* Handle failed write here */
            }
        /* Do something to handle the retry */
            std::cout << "Failed write" << std::endl;
    }

    //char buf[MAX_PACKET_SIZE];

    int p;
    char r[1024];
    for(;;)
    {
    		printf("\ncycle\n");
            p = BIO_read(bio, r, 1023);
            if(p <= 0) break;
            r[p] = 0;
            printf("%s", r);
    }

    std::cout << "Done reading" << std::endl;

    /* To free it from memory, use this line */

    ERR_print_errors_fp(stderr);
    BIO_free_all(bio);
}