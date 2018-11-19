#include "feed.hpp"

Feed::Feed(Program *program, string url)
{
	// Save pointer to the program (to have access to flags)
	this->program = program;

	// Determine basic informations from given URL
	this->valid = false;
	if(this->determinePort(url) && this->determineHost(url) && this->determinePath(url))
		this->valid = true;
}


bool Feed::determinePort(string url)
{
	// Determine if secure connection should be used
	if(regex_match(url, regex("^http://.*$")))
		this->secure = false;
	else if(regex_match(url, regex("^https://.*$")))
		this->secure = true;
	else
	{
		cerr << "ERROR: http:// or https:// is missing in URL\n";
		return false;
	}

	// Determine what port should be used
	smatch matches;
	if(regex_search(url, matches, regex("(?:^http://|^https://)(?:[^/]+):([0-9]+)")))
	{
		// Port is specified in URL
		
        if(matches.size() == 2)
        	this->port = stoi(matches[1]);
    	else
    	{
    		cerr << "ERROR: Unexpected error in determining specified port\n";
    		return false;
    	}
	}
	else
	{
		// Port is not specified in URL
		
		if(this->secure)
			this->port = 443;
		else
			this->port = 80;
	}

	return true;
}


bool Feed::determineHost(string url)
{
	smatch matches;
	if(regex_search(url, matches, regex("(?:^http://|^https://)([^/:]+)")))
	{
        if(matches.size() == 2)
        	this->host = matches[1];
    	else
    	{
    		cerr << "ERROR: Unexpected error in determining hostgname\n";
    		return false;
    	}
	}
	else
	{
		cerr << "ERROR: Cannot determine hostname for connection\n";
		return false;
	}

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
			cerr << "ERROR: Cannot determine path for connection\n";
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


bool Feed::read()
{
	BIO *bio;

	// Connect to server
	if(!this->connectHost(&bio))
		return this->closeBio(&bio);

	// Send HTTP request to server
	if(!this->sendRequest(&bio))
		return this->closeBio(&bio);

	// Get HTTP response from server
	string response = this->readResponse(&bio);

	// Verify returned status code from server
	if(!this->checkHeader(response))
		return this->closeBio(&bio);

	// Truncate header from the response
	string content;
	try
	{
		content = this->discardHeader(response);
	}
	catch(bool x)
	{
		return this->closeBio(&bio);
	}

	// Close connection
    BIO_free_all(bio);

    // Parse the response
    this->parse(content, this->program->flags);

    return true;
}

bool Feed::closeBio(BIO **bio)
{
	BIO_free_all(*bio);
	return false;	
}


bool Feed::connectHost(BIO **bio)
{
	if(secure == false)
	{
		return this->connectHttpHost(bio);
	}
	else if(secure == true)
	{
		return this->connectHttpsHost(bio);
	}
	else
	{
		cerr << "Cannot determine if secure connection should be used\n";
		return false;
	}
}


bool Feed::connectHttpHost(BIO **bio)
{
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();

	*bio = BIO_new_connect(string(this->host+":"+to_string(this->port)).c_str());
	if(*bio == NULL)
	{
		cerr << "ERROR: Couldn't create connection\n";
		return false;		
	}

	if(BIO_do_connect(*bio) <= 0)
	{
		cerr << "ERROR: Connection failed\n";
		return false;		
	}

	return true;
}


bool Feed::connectHttpsHost(BIO **bio)
{
	SSL *ssl;
    SSL_CTX *ctx;

    // Init SSL stuff
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
	SSL_library_init();

	// Create context structure
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL)
    {
        cerr << "ERROR: Couldn't create context strucutre\n";
        return false;
    }

    // Setup ceritificates locations
    this->setupCertificates(&ctx);

    // Setup SSL connection
    *bio = BIO_new_ssl_connect(ctx);
    BIO_get_ssl(*bio, & ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
	BIO_set_conn_hostname(*bio, string(this->host+":"+to_string(this->port)).c_str());

	// Connect to server
    if(BIO_do_connect(*bio) <= 0)
    {
        cerr << "ERROR: Connection failed\n";
        return false;
    }

    // Verify certificate
    long int verifyResult = SSL_get_verify_result(ssl);
    if(verifyResult != X509_V_OK &&
    	verifyResult != X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT)
    {
    	cerr << "ERROR: Certificate verification failed\n";
    	return false;
    }

    return true;
}

void Feed::setupCertificates(SSL_CTX **ctx)
{
	// Check if certification parameteres were used
	bool certFileUsed = (this->program->flags.count('c') != 0);
	bool certFolderUsed = (this->program->flags.count('C') != 0);

	// Set default location when certificates weren't used
	if(!certFileUsed && !certFolderUsed)
	{
		SSL_CTX_set_default_verify_paths(*ctx);
		return;
	}

	// Loop through used parameters
	for(auto const& flag: this->program->flags)
	{
		// Process parameter -c
		if(flag.first == 'c')
		{
			if(!SSL_CTX_load_verify_locations(*ctx, flag.second.c_str(), NULL))
				cerr << "ERROR: File specified in -c parameter failed to load\n";
		}
		// Process parameter -C
		else if(flag.first == 'C')
		{
			if(!SSL_CTX_load_verify_locations(*ctx, NULL, flag.second.c_str()))
				cerr << "ERROR: Location specified in -C parameter failed to load\n";
		}
	}

}

bool Feed::sendRequest(BIO **bio)
{
	// Prepare HTTP request
	string httpRequest("GET /"+this->path+" HTTP/1.0\r\n"
					"HOST: "+this->host+"\r\n"
					"User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36\r\n"
					"Connection: close\r\n\r\n");
    const char *httpRequestC = httpRequest.c_str();

    // Send HTTP request
    if(BIO_write(*bio, httpRequestC, strlen(httpRequestC)) <= 0)
    {
            if(!BIO_should_retry(*bio))
            {
            	cerr << "ERROR: Cannot send request\n";
            	return false;
            }
        	
            cerr << "ERROR: Cannot send request - Trying again\n";
    }
    
    return true;
}


string Feed::readResponse(BIO **bio)
{
	string response;

	// Setup buffer
	char buffer[65535];	
	memset(buffer, '\0', sizeof(buffer));
    int len;
    
    // Read incoming traffic
    while(true)
    {
        len = BIO_read(*bio, buffer, sizeof(buffer)-1);
        
        // Check end of transmission
        if(len <= 0)
        	break;

        // Add null byte to the end of the message
        buffer[len] = 0;

        // Append to response
        response += buffer;
    }

    return response;
}


bool Feed::checkHeader(string content)
{
	// Read response code
	string responseCode(content.substr(9, 3)); 

	// Check response code
	if(responseCode != "200")
	{
		cerr << "ERROR: Unexpected HTTP response code\n";
	    return false;			
	}

	return true;
}


string Feed::discardHeader(string content)
{
	size_t pos;

	// Search for official header ending
	pos = content.find("\r\n\r\n");
	if(pos != string::npos)
		pos += 4;
	else
	{
		// Search for non-official header ending
		pos = content.find("\n\n");
		if(pos != string::npos)
			pos += 2;
		else
		{
			// Header not found
			cerr << "ERROR: Couldn't find header in response\n";
			throw false;
			return "";
		}
	}   

	// Return response without header
	return content.substr(pos);
}

void Feed::parse(string content, unordered_multimap<int, string> flags)
{
	// Create parser for feed
	Parser parser(content);

	// Print feed
	cout << parser.toString(flags);
}