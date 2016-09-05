/*
The MIT License (MIT)

Copyright (c) 2016 https://github.com/nmrr

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <iostream>
#include <curl/curl.h>
#include <map>
#include <thread>
#include <vector>
#include <fstream>
#include "timer.h"
#include "query.h"
#include <unistd.h>

using namespace std;

void threadFunction(Query &req, map<string, uint32_t> &linkMap)
{
	req.getLink(linkMap);
}

struct structWiki
{
	string root;
	map<string, uint32_t> depth1MapLink;
	map<string, map<string, uint32_t>> depth2MapLink;
};

int main(int argc, char **argv)
{
	bool wikiArg = false;
	string wikiArgString = "";

	bool pageArg = false;
	string pageArgString = "";

	bool resolutionArg = false;
	string resolutionArgString = "";

	bool fileArg = false;
	string fileArgString = "";

	bool depthArg = false;
	uint32_t depthArgInt = 0;

	bool threadArg = false;
	uint32_t threadArgInt = 0;

	bool useragentArg = false;
	string useragentArgString = "";

	bool separatorArg = false;
	string separatorArgString = "";

	bool helpArg = false;

	for (int i=1; i<argc;)
	{
		string argument = string(argv[i]);

		if (argument == "-h" || argument == "--help")
		{
			helpArg = true;
			break;
		}
		else if (argc-i-1 >= 1)
		{
			if (argument == "-w" || argument == "--wiki")
			{
				wikiArgString = string(argv[i+1]);

				if (wikiArgString.length() == 0)
				{
					cout << "Wiki's URL is empty" << endl;
					exit(1);
				}

				wikiArg = true;
				i += 2;
			}
			else if (argument == "-p" || argument == "--page")
			{
				pageArgString = string(argv[i+1]);

				if (pageArgString.length() == 0)
				{
					cout << "Page's name is empty" << endl;
					exit(1);
				}

				pageArg = true;
				i += 2;
			}
			else if (argument == "-r" || argument == "--resolution")
			{
				resolutionArgString = string(argv[i+1]);

				if (resolutionArgString.length() == 0)
				{
					cout << "Resolution's string is empty" << endl;
					exit(1);
				}

				resolutionArg = true;
				i += 2;
			}
			else if (argument == "-f" || argument == "--file")
			{
				fileArgString = string(argv[i+1]);

				if (fileArgString.length() == 0)
				{
					cout << "Output file's name is empty" << endl;
					exit(1);
				}

				fileArg = true;
				i += 2;
			}
			else if (argument == "-d" || argument == "--depth")
			{
				try
				{
					depthArgInt = stoi(string(argv[i+1]));
					if (depthArgInt == 0 || depthArgInt > 4)
					{
						cout << "Depth out of range (1-4)" << endl;
						exit(1);
					}
				}
				catch(...)
				{
					cout << "Invalid depth, enter a number (1-4)" << endl;
					exit(1);
				}
				depthArg = true;
				i += 2;
			}
			else if (argument == "-t" || argument == "--thread")
			{
				try
				{
					threadArgInt = stoi(string(argv[i+1]));
					if (threadArgInt == 0 || threadArgInt > 100)
					{
						cout << "Number of threads out of range (1-100)" << endl;
						exit(1);
					}
				}
				catch(...)
				{
					cout << "Invalid number of threads, enter a number (1-100)" << endl;
					exit(1);
				}
				threadArg = true;
				i += 2;
			}
			else if (argument == "-u" || argument == "--useragent")
			{
				useragentArgString = string(argv[i+1]);

				if (useragentArgString.length() == 0)
				{
					cout << "User agent is empty" << endl;
					exit(1);
				}

				useragentArg = true;
				i += 2;
			}
			else if (argument == "-s" || argument == "--separator")
			{
				separatorArgString = string(argv[i+1]);

				if (separatorArgString.length() == 0)
				{
					cout << "Separator is empty" << endl;
					exit(1);
				}

				separatorArg = true;
				i += 2;
			}
			else
			{
				cout << "Invalid argument : " << argument << endl;
				exit(1);
			}
		}
		else
		{
			break;
		}
	}

	if (helpArg == true)
	{
		cout << "--------- wikiLinking ---------" << endl;
		cout << "Required arguments : " << endl;
		cout << "-w (--wiki) : URL of the wiki (ex : https://en.wikipedia.org/wiki/)" << endl;
		cout << "-p (--page) : Root page (ex : Paris)" << endl;
		cout << "-r (--resolution) : Resolution domain string, page:port:IP (ex : en.wikipedia.org:443:91.198.174.192)" << endl;
		cout << "-f (--file) : Output file, format is CSV (ex : paris.csv)" << endl;
		cout << endl;
		cout << "Optional arguments : " << endl;
		cout << "-d (--depth) : Maximum depth of the tree (1-4), default is 1" << endl;
		cout << "-t (--thread) : Number of threads (1-100), default is 5, please don't overload the target server !" << endl;
		cout << "-u (--useragent) : User agent used by the crawler" << endl;
		cout << "-s (--separator) : Field separator, default is ' ; '" << endl;

		exit(1);
	}

	if (wikiArg == false || pageArg == false || resolutionArg == false || fileArg == false)
	{
		cout << "No enough arguments, to obtain help : -h or --help" << endl;
		exit(1);
	}

	if (depthArg == false)
	{
		depthArgInt = 1;
	}

	if (threadArg == false)
	{
		threadArgInt = 5;
	}

	if (useragentArg == false)
	{
		useragentArgString = "wikiLinking - https://github.com/nmrr/wikiLinking";
	}

	if (separatorArg == false)
	{
		separatorArgString = " ; ";
	}
	
	cout << "--------- wikiLinking ---------" << endl;
	cout << "Wiki's URL : " << wikiArgString << endl;
	cout << "Page : " << pageArgString << endl;
	cout << "Domain resolution : " << resolutionArgString << endl;
	cout << "Output file : " << fileArgString << endl;
	cout << "Depth : " << depthArgInt << endl;
	cout << "Number of threads : " << threadArgInt << endl;
	cout << "User agent : " << useragentArgString << endl;
	cout << "Separator : '" << separatorArgString << "'" << endl;
	cout << endl;

	ofstream outputFile;
	outputFile.open (fileArgString, ios::trunc);

	if (outputFile.is_open())
	{
		Timer time(true);

		map<string, uint32_t> poolMap;
		
		map<string, uint32_t> depth1MapLink;
		Query rootLink(wikiArgString, pageArgString, resolutionArgString, useragentArgString);
		rootLink.getLink(depth1MapLink);

		if(rootLink.getSuccess() == true)
		{
			poolMap[pageArgString] = 0;
			outputFile << pageArgString << separatorArgString << endl;
			
			uint32_t numberQueries = 1;

			for (map<string,uint32_t>::iterator it = depth1MapLink.begin(); it != depth1MapLink.end();)
			{
				if (it->first != pageArgString)
				{
					poolMap[it->first] = 1;
					outputFile << pageArgString << separatorArgString << it->first << separatorArgString << endl;
					it++;
				}
				else
				{
					it = depth1MapLink.erase(it);
				}
			}

			if (depthArgInt >= 2)
			{
				thread * threadArray = new thread[threadArgInt];
				Query * queryArray = new Query[threadArgInt];
				map<string, uint32_t> * threadMap = new map<string, uint32_t>[threadArgInt];

				uint32_t threadCounter = 0;

				// Depth 2
				vector<structWiki> depth2Vector;
				depth2Vector.reserve(depth1MapLink.size());
				uint32_t counter = 0;
				for (map<string,uint32_t>::iterator it = depth1MapLink.begin(); it != depth1MapLink.end(); it++)
				{
					counter++;
					cout << '\r' << "Depth 2 : " << counter << "/" << depth1MapLink.size() << " " << flush;

					if (threadCounter == threadArgInt)
					{
						for (uint32_t i=0; i<threadCounter; i++)
						{
							threadArray[i].join();
							if (queryArray[i].getSuccess() == true)
							{
								for (map<string,uint32_t>::iterator it2 = threadMap[i].begin(); it2 != threadMap[i].end();)
								{
									map<string, uint32_t>::iterator search = poolMap.find(it2->first);
									if (search == poolMap.end())
									{
										poolMap[it2->first] = 2;
										outputFile << pageArgString << separatorArgString << queryArray[i].getAddress() << separatorArgString << it2->first << separatorArgString << endl;
										it2++;
									}
									else
									{
										it2 = threadMap[i].erase(it2);
									}
								}

								if (threadMap[i].size() > 0)
								{
									structWiki tmp;
									tmp.root = queryArray[i].getAddress();
									tmp.depth1MapLink = threadMap[i];
									depth2Vector.push_back(tmp);
								}
							}
							else
							{
								cerr << "Error : " << queryArray[i].getAddress() << " : " << queryArray[i].getFailure() << endl;
							}
						}

						threadCounter = 0;
					}

					queryArray[threadCounter].newQuery(wikiArgString, it->first, resolutionArgString, useragentArgString);
					threadArray[threadCounter] = thread(threadFunction, ref(queryArray[threadCounter]), ref(threadMap[threadCounter]));
					numberQueries++;
					threadCounter++;
				}

				for (uint32_t i=0; i<threadCounter; i++)
				{
					threadArray[i].join();
					if (queryArray[i].getSuccess() == true)
					{
						structWiki tmp;
						tmp.root = queryArray[i].getAddress();

						for (map<string,uint32_t>::iterator it2 = threadMap[i].begin(); it2 != threadMap[i].end();)
						{
							map<string, uint32_t>::iterator search = poolMap.find(it2->first);
							if (search == poolMap.end())
							{
								poolMap[it2->first] = 2;
								outputFile << pageArgString << separatorArgString << queryArray[i].getAddress()<< separatorArgString << it2->first << separatorArgString << endl;
								it2++;
							}
							else
							{
								it2 = threadMap[i].erase(it2);
							}
						}

						if (threadMap[i].size() > 0)
						{
							tmp.depth1MapLink = threadMap[i];
							depth2Vector.push_back(tmp);
						}
					}
					else
					{
						cerr << "Error : " << queryArray[i].getAddress() << " : " << queryArray[i].getFailure() << endl;
					}
				}

				if (depthArgInt >= 3)
				{
					// Depth 3
					counter = 0;
					for (vector<structWiki>::iterator it = depth2Vector.begin(); it != depth2Vector.end(); it++)
					{
						counter++;
						threadCounter = 0;
						uint32_t counter2 = 0;

						for (map<string,uint32_t>::iterator it2 = it->depth1MapLink.begin(); it2 != it->depth1MapLink.end(); it2++)
						{
							counter2++;
							cout << '\r' << "Depth 3 : " << counter << "/" << depth2Vector.size() << ", " << counter2 << "/" << it->depth1MapLink.size() << " " << flush;

							if (threadCounter == threadArgInt)
							{
								for (uint32_t i=0; i<threadCounter; i++)
								{
									threadArray[i].join();
									if (queryArray[i].getSuccess() == true)
									{
										map<string, uint32_t> depth3MapLink;

										for (map<string,uint32_t>::iterator it3 = threadMap[i].begin(); it3 != threadMap[i].end(); it3++)
										{
											map<string, uint32_t>::iterator search = poolMap.find(it3->first);
											if (search == poolMap.end())
											{
												depth3MapLink[it3->first] = 3;
												poolMap[it3->first] = 3;
												outputFile << pageArgString << separatorArgString << it->root << separatorArgString << queryArray[i].getAddress() << separatorArgString << it3->first << separatorArgString << endl;
											}
										}

										if (depth3MapLink.size() > 0)
										{
											it->depth2MapLink[queryArray[i].getAddress()] = depth3MapLink;
										}
									}
									else
									{
										cerr << "Error : " << queryArray[i].getAddress() << " : " << queryArray[i].getFailure() << endl;
									}
								}
								threadCounter = 0;
							}

							queryArray[threadCounter].newQuery(wikiArgString, it2->first, resolutionArgString, useragentArgString);
							threadArray[threadCounter] = thread(threadFunction, ref(queryArray[threadCounter]), ref(threadMap[threadCounter]));
							numberQueries++;
							threadCounter++;
						}

						for (uint32_t i=0; i<threadCounter; i++)
						{
							threadArray[i].join();
							if (queryArray[i].getSuccess() == true)
							{
								map<string, uint32_t> depth3MapLink;

								for (map<string,uint32_t>::iterator it3 = threadMap[i].begin(); it3 != threadMap[i].end(); it3++)
								{
									map<string, uint32_t>::iterator search = poolMap.find(it3->first);
									if (search == poolMap.end())
									{
										depth3MapLink[it3->first] = 3;
										poolMap[it3->first] = 3;
										outputFile << pageArgString << separatorArgString << it->root << separatorArgString << queryArray[i].getAddress() << separatorArgString << it3->first << separatorArgString << endl;
									}
								}

								if (depth3MapLink.size() > 0)
								{
									it->depth2MapLink[queryArray[i].getAddress()] = depth3MapLink;
								}
							}
							else
							{
								cerr << "Error : " << queryArray[i].getAddress() << " : " << queryArray[i].getFailure() << endl;
							}
						}
					}

					if (depthArgInt == 4)
					{
						//Depth 4
						counter = 0;
						uint32_t sizeDepth2 = depth2Vector.size();

						for(vector<structWiki>::iterator it = depth2Vector.begin(); it != depth2Vector.end();)
						{
							counter++;
							uint32_t counter2 = 0;
							for (map<string, map<string, uint32_t>>::iterator it2 = it->depth2MapLink.begin(); it2 != it->depth2MapLink.end(); it2++)
							{
								counter2++;
								uint32_t counter3 = 0;
								threadCounter = 0;

								for (map<string,uint32_t>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); it3++)
								{
									counter3++;
									cout << '\r' << "Depth 4 : " << counter << "/" << sizeDepth2 << ", " << counter2 << "/" << it->depth2MapLink.size() << ", " << counter3 << "/" << it2->second.size() << " " << flush;

									if (threadCounter == threadArgInt)
									{
										for (uint32_t i=0; i<threadCounter; i++)
										{
											threadArray[i].join();
											if (queryArray[i].getSuccess() == true)
											{
												for (map<string,uint32_t>::iterator it4 = threadMap[i].begin(); it4 != threadMap[i].end(); it4++)
												{
													map<string, uint32_t>::iterator search = poolMap.find(it4->first);
													if (search == poolMap.end())
													{
														poolMap[it4->first] = 4;
														outputFile << pageArgString << separatorArgString << it->root << separatorArgString << it2->first << separatorArgString << queryArray[i].getAddress() << separatorArgString << it4->first << separatorArgString << endl;
													}
												}
											}
											else
											{
												cerr << "Error : " << queryArray[i].getAddress() << " : " << queryArray[i].getFailure() << endl;
											}
										}
										threadCounter = 0;
									}

									queryArray[threadCounter].newQuery(wikiArgString, it3->first, resolutionArgString, useragentArgString);
									threadArray[threadCounter] = thread(threadFunction, ref(queryArray[threadCounter]), ref(threadMap[threadCounter]));
									numberQueries++;
									threadCounter++;
								}

								for (uint32_t i=0; i<threadCounter; i++)
								{
									threadArray[i].join();
									if (queryArray[i].getSuccess() == true)
									{
										for (map<string,uint32_t>::iterator it4 = threadMap[i].begin(); it4 != threadMap[i].end(); it4++)
										{
											map<string, uint32_t>::iterator search = poolMap.find(it4->first);
											if (search == poolMap.end())
											{
												poolMap[it4->first] = 4;
												outputFile << pageArgString << separatorArgString << it->root << separatorArgString << it2->first << separatorArgString << queryArray[i].getAddress() << separatorArgString << it4->first << separatorArgString << endl;
											}
										}
									}
									else
									{
										cerr << "Error : " << queryArray[i].getAddress() << " : " << queryArray[i].getFailure() << endl;
									}
								}
							}
							
							it = depth2Vector.erase(it);
						}
					}
				}

				delete [] threadArray;
				delete [] queryArray;
				delete [] threadMap;

				cout << endl << endl;
			}

			time.stop();
			cout << "Time : " << time.getSec() << " s" << endl;
			cout << "Number of queries : " << numberQueries << endl;
			cout << "Size of the pool : " << poolMap.size() << endl;
		}
		else
		{
			cerr << "Error : " << rootLink.getFailure() << endl;
		}
	}
	else
	{
		cerr << "Unable to open the file" << endl;
	}

	outputFile.close();

	return 0;
}
