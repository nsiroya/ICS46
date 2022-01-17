#include "proj3.hpp"
#include "Wordset.hpp"
#include <iostream>
#include <set>
#include <sstream>
#include <map>
#include <stack>
#include <queue>

// You should not need to change this function.
void loadWordsIntoTable(WordSet & words, std::istream & in)
{
	std::string line, word;
	std::stringstream ss;

	while(	getline(in, line) )
	{
		ss.clear();
		ss << line;
		while( ss >> word )
		{
			words.insert( word );
		}
	}

}


// You probably want to change this function.
std::string convert(std::string s1, std::string s2, const WordSet & words)
{
	std::map<std::string, std::string> word_map;
	std::queue<std::string> frontierQueue;
	frontierQueue.push(s1);
	while ( !frontierQueue.empty() )
	{

		std::string currentV = frontierQueue.front();
		frontierQueue.pop();

		for ( unsigned int a = 0; a < currentV.size(); a++ )
		{

			if (currentV.at(a) != s2.at(a))
			{
				std::string newV = currentV;
				newV.at(a) = 'a';				
				for (unsigned int i = 0; i<25; i++)
				{
					if (word_map.count(newV)==0 && words.contains(newV))
					{	
						word_map[newV] = currentV;
						frontierQueue.push(newV);
					}
					else
					{
						newV.at(a) += 1;
					}
				}
			}
		}
	}     
	std::string return_string;
	return_string += (" --> " + s2 );
	std::string x = word_map[s2];
	while (x != s1)
	{
		return_string = (" --> " + x) + return_string;
		x = word_map[x];
	}
	return_string = s1 + return_string;
	return return_string;  
}
