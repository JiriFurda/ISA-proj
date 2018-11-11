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

void Feed::dumpInfo()
{
	cout << this->host << "\n";
	cout << this->port << "\n";
	cout << this->path << "\n";
}

void Feed::read()
{
	BIO *bio;

	this->connectHost(&bio);
	this->sendRequest(&bio);
	string content = this->discardHeader(this->readResponse(&bio));

    ERR_print_errors_fp(stderr);
    BIO_free_all(bio);

    this->parse(content);
}


void Feed::connectHost(BIO **bio)
{
	
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();

	*bio = BIO_new_connect(string(this->host+":"+to_string(this->port)).c_str());
	if(*bio == NULL)
	{
		fprintf(stderr, "Error: Couldn't create connection");
		exit(1);		
	}

	if(BIO_do_connect(*bio) <= 0)
	{
		fprintf(stderr, "Error: Couldn't connect");
		exit(1);		
	}
	
}


void Feed::sendRequest(BIO **bio)
{
	
	string httpRequest("GET /"+this->path+" HTTP/1.0\r\n"
					"HOST: "+this->host+"\r\n"
					"Connection: close\r\n\r\n");

    const char *httpRequestC = httpRequest.c_str();

    if(BIO_write(*bio, httpRequestC, strlen(httpRequestC)) <= 0)
    {
            if(!BIO_should_retry(*bio))
            {
                // Handle failed write here
            	std::cerr << "Failed write" << std::endl;
            }
        	// Do something to handle the retry 
            std::cerr << "Failed write (retrying)" << std::endl;
    }
    
}


string Feed::readResponse(BIO **bio)
{
	
	char buffer[65535];	
	memset(buffer, '\0', sizeof(buffer)); 	// Earse buffer
    int len;
    string response;

    while(true)
    {
        len = BIO_read(*bio, buffer, sizeof(buffer)-1);
        if(len <= 0)
        	break;
        buffer[len] = 0;

        response += buffer;
    }

    return response;
}


string Feed::discardHeader(string content)
{
	size_t pos;

	pos = content.find("\r\n\r\n");
	if(pos != string::npos)
		pos += 4;
	else
	{
		pos = content.find("\n\n");
		if(pos != string::npos)
			pos += 2;
		else
		{
			cerr << "Couldn't find header in response" << endl;
			exit(1);
		}
	}   

	return content.substr(pos);
}

void Feed::parse(string content)
{
	Parser parser(content);
}