/*
The MIT License (MIT)

Copyright (c) 2016 https://github.com/nmrr

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "query.h"

void Query::newQuery(string wikiURL, string article, string resolution, string useragent, unsigned int timeout)
{
	this->wikiURL = wikiURL;
	this->article = article;
	this->resolution = resolution;
	this->useragent = useragent;
	this-> timeout = timeout;
}

string Query::getAddress()
{
	return article;
}

bool Query::getSuccess()
{
	return success;
}

string Query::getFailure()
{
	return error;
}

void Query::getLink(map<string, uint32_t> &linkMap, uint32_t attemptMaximum)
{
	for (uint32_t attempt=0; attempt<attemptMaximum; attempt++)
	{
		linkMap.clear();
		string output;
		output.reserve(1024*50);
		if (curlGET(this->wikiURL + this->article, output, this->error, this->resolution, this->useragent, this->timeout) == true)
		{
			// Find the beginning of the article
			size_t articleBeginning = output.find("<div id=\"content\" class=\"mw-body\" role=\"main\">");
			if (articleBeginning != string::npos)
			{
				// Find the ending of the article
				size_t articleEnding = output.find("<div id=\"footer\" role=\"contentinfo\">", articleBeginning);
				if (articleEnding != string::npos)
				{
					// Copy the content of the article
					string articleContent = "";
					articleContent.reserve(articleEnding-articleBeginning);
					for (size_t i=articleBeginning; i<articleEnding; i++)
					{
						articleContent += output[i];
					}

					// Search links to the other pages
					size_t position = 0;
					while(1)
					{
						const string search = "href=\"/wiki/";
						size_t found = articleContent.find(search, position);
						if (found != string::npos)
						{
							found += search.length();

							string link = "";
							link.reserve(64);
							for (size_t i = found; i < articleContent.length(); i++)
							{
								if (articleContent[i] != '"')
								{
									link += articleContent[i];
									found++;
								}
								else
								{
									break;
								}
							}

							position = found+1;
							link = decodeURL(link);

							if (link.length() > 0)
							{
								// In the english Wikipedia, there is a redirection to the main page after the beginning
								if (link != "Main_Page")
								{
									// Check if the link is an internal Wikipedia's page like discussion or help
									bool check = true;
									for (size_t i = 0; i < link.length(); i++)
									{
										if (link[i] == ':')
										{
											check = false;
											break;
										}
									}

									if (check == true)
									{
										string linkWithoutAnchor = "";
										linkWithoutAnchor.reserve(link.length());

										// Remove the anchor of the link
										for (size_t i = 0; i < link.length(); i++)
										{
											if (link[i] != '#')
											{
												linkWithoutAnchor += link[i];
											}
											else
											{
												break;
											}
										}

										if (linkWithoutAnchor.length() > 0)
										{
											// Convert % to %25 and / to %27
											for (uint32_t position = 0; position < linkWithoutAnchor.length();)
											{
												if (linkWithoutAnchor[position] == '%')
												{
													linkWithoutAnchor.insert(position+1, "25");
													position += 3;
												}
												else if (linkWithoutAnchor[position] == '\'')
												{
													linkWithoutAnchor[position] = '%';
													linkWithoutAnchor.insert(position+1, "27");
													position += 3;
												}
												else
												{
													position++;
												}
											}

											// Add the link to the map
											linkMap[linkWithoutAnchor] = 0;
										}
									}
								}
							}
						}
						else
						{
							break;
						}
					}

					success = true;
				}
				else
				{
					error = "missing ending";
					success = false;
				}
			}
			else
			{
				error = "missing beginning";
				success = false;
			}

		}
		else
		{
			success = false;
		}

		if (success == true) break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t Query::curlCallbackString(void *contents, size_t size, size_t nmemb, void *userp)
{
	((string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

bool Query::curlGET(string url, string &output, string &error, string resolution, string useragent, unsigned int timeout)
{
	CURL *curl;
	CURLcode res;
	output = "";
	error = "";
	curl = curl_easy_init();
	bool returnValue = false;

	struct curl_slist *host = NULL;
	host = curl_slist_append(NULL, resolution.c_str());

	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_RESOLVE, host);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, useragent.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallbackString);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
		{
			error = curl_easy_strerror(res);
		}
		else
		{
			returnValue = true;
		}
	}
	else
	{
		error = "curl_easy_init";
	}

	curl_easy_cleanup(curl);
	curl_slist_free_all(host);
	return returnValue;
}

char Query::hexa(char letter)
{
	if (letter >= 48 && letter <= 57) // 0-9
	{
		return letter - 48;
	}
	else if (letter >= 65 && letter <= 70) // A-F
	{
		return letter - 65 + 10;
	}
	else if (letter >= 97 && letter <= 102) // a-f
	{
		return letter - 97 + 10;
	}
	else
	{
		return -1;
	}
}

string Query::decodeURL(string URL)
{
	string output;
	output.reserve(URL.length());

	for (uint32_t i=0; i<URL.length();)
	{
		bool success = false;
		if (URL[i] == '%')
		{
			if (URL.length() - i >= 3)
			{
				char a = hexa(URL[i+1]);
				char b = hexa(URL[i+2]);

				if (a != -1 && b != -1)
				{
					a <<= 4;
					a |= b;
					output += a;
					i += 3;
					success = true;
				}
			}
		}

		if (success == false)
		{
			output += URL[i];
			i++;
		}
	}

	return output;
}
