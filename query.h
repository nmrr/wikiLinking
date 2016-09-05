/*
The MIT License (MIT)

Copyright (c) 2016 https://github.com/nmrr

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __QUERY_H
#define __QUERY_H

#include <curl/curl.h>
#include <map>

using namespace std;

class Query
{
	
private :

	string wikiURL;
	string article;
	unsigned int timeout;
	string error;
	bool success;
	string resolution;
	string useragent;

public :

	Query()
	{

	}

	Query(string wikiURL, string article, string resolution, string useragent, unsigned int timeout=30)
	{
		newQuery(wikiURL, article, resolution, useragent, timeout);
	}

	~Query()
	{

	}

	void newQuery(string wikiURL, string article, string resolution, string useragent, unsigned int timeout=30);
	string getAddress();
	bool getSuccess();
	string getFailure();
	void getLink(map<string, uint32_t> &linkMap, uint32_t attemptMaximum=5);

private :

	static size_t curlCallbackString(void *contents, size_t size, size_t nmemb, void *userp);
	bool curlGET(string url, string &output, string &error, string resolution, string useragent, unsigned int timeout);
	char hexa(char letter);
	string decodeURL(string URL);
	
};

#endif

