#include "Wordset.hpp"
#include <string>
#include <cmath>
#include <iostream>

const int BASE_TO_USE = 41; 
const double LOAD_LIMIT = 0.27;



// returns s, as a number in the given base, mod the given modulus
int hashFunction(std::string s, int base, int mod)
{
	unsigned int total = 0;
	unsigned int power = s.size() - 1;
	for (unsigned i = 0; i < s.size(); i ++)
	{	
		total += ((( s[i] - 'a' ) % mod) * (int(pow(base % mod, power)) % mod)) % mod;
		power --;
	}
	return total % mod;
}


WordSet::WordSet()
	: array_length{SIZES[0]}, array_length_index{0}, unique_count{0}, filled_buckets{0}
{
	hash_array = new std::string[array_length];
}

double WordSet::load_factor()
{
	return double(filled_buckets) / double(array_length);
}

WordSet::~WordSet()
{
	delete[] hash_array;
}

void WordSet::insert(std::string s)
{
	unsigned int bucket_index = hashFunction(s, BASE_TO_USE, array_length);
	unsigned int bucketsProbed = 0;
	unsigned int i = 0;
	bool get_count = true;
	while ( bucketsProbed < array_length)
	{
		
		if ( hash_array[bucket_index].empty() )
		{
			hash_array[bucket_index] = s;
			filled_buckets += 1;
			break;
		}
		else if ( !hash_array[bucket_index].empty() && hash_array[bucket_index] == s)
		{
			i += 1;
			bucket_index = (hashFunction(s, BASE_TO_USE, array_length) + unsigned(pow(i,2))) % array_length;
			bucketsProbed += 1;
			get_count = false;
		}
		else if (!hash_array[bucket_index].empty() )		
		{
			i += 1;
			bucket_index = (hashFunction(s, BASE_TO_USE, array_length) + unsigned(pow(i,2))) % array_length;
			bucketsProbed += 1;
		}
	}
	if ( get_count )
	{
		unique_count += 1;
	}
	if (load_factor() > LOAD_LIMIT)
	{
		resize();
	}
}


bool WordSet::contains(std::string s) const
{
	unsigned int bucket_index = hashFunction(s, BASE_TO_USE, array_length);
	unsigned int bucketsProbed = 0;
	unsigned int i = 0;
	while ( bucketsProbed < array_length)
	{
		if (hash_array[bucket_index].empty())
		{
			return false;
		}
		if ( hash_array[bucket_index] == s)
		{
			return true;
		}
		else
		{
			i += 1;
			bucket_index = (hashFunction(s, BASE_TO_USE, array_length) + unsigned(pow(i,2))) % array_length;
			bucketsProbed += 1;
		}
	}
	return false;
}

// return how many distinct strings are in the set
int WordSet::getCount() const
{
	return unique_count;
}

// return how large the underlying array is.
int WordSet::getCapacity() const
{
	return array_length; 
}

void WordSet::resize()
{
	array_length_index += 1;
	int new_array_length = SIZES[array_length_index];
	std::string* new_array = new std::string[SIZES[array_length_index]];
	int buckets_added = 0;
	for (unsigned int a = 0; a < array_length; a++)
	{
		if (!hash_array[a].empty())
		{
			unsigned int bucket_index = hashFunction(hash_array[a], BASE_TO_USE, new_array_length);
			unsigned int bucketsProbed = 0;
			unsigned int i = 0;
			while ( bucketsProbed < new_array_length)
			{
				if ( new_array[bucket_index].empty() )
				{
					new_array[bucket_index] = hash_array[a];
					buckets_added += 1;
					break;
				}

				else if (!new_array[bucket_index].empty() )		
				{
					i += 1;
					bucket_index = (hashFunction(hash_array[a], BASE_TO_USE, new_array_length) + unsigned(pow(i,2))) % new_array_length;
					bucketsProbed += 1;
				}
			}
		}
	}
	delete[] hash_array;
	hash_array = new_array;
	array_length = new_array_length;
}