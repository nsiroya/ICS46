#include "HashMap.hpp"

namespace 
{
    // default hash function 
    unsigned int defaultHash(const std::string& key)
    {
        unsigned int seed = 19;
        unsigned int hash = 0;
        for(int i = 0; i < key.length(); i++)
        {
            hash = (hash*seed) + key.at(i);
        }
        return hash;
    }
}

// Constructor that uses the default hash function
HashMap::HashMap()
    : hashFunction{defaultHash}, totalBucketCount{INITIAL_BUCKET_COUNT}
{   
    bucketList = new Node*[totalBucketCount];
    initializeBucketList(totalBucketCount);
}

// Constructor that uses another hash function
HashMap::HashMap(HashFunction hashFunction) 
    : totalBucketCount{INITIAL_BUCKET_COUNT}
{
    this->hashFunction = hashFunction;
    bucketList = new Node*[totalBucketCount];
    initializeBucketList(totalBucketCount);
}

// Copy constructor
HashMap::HashMap(const HashMap& hm) 
    :hashFunction{hm.hashFunction}, totalBucketCount{hm.totalBucketCount}, totalPairCount{hm.totalPairCount}
{
    bucketList = new Node*[totalBucketCount];
    initializeBucketList(totalBucketCount);
    for(int i = 0; i < totalBucketCount; i++)
    {
        Node* curr_node = hm.bucketList[i];
        if (curr_node == nullptr) {
            bucketList[i] = nullptr;
        } else {    
            bucketList[i] = new Node{curr_node->key, curr_node->value, nullptr};
            Node* target = bucketList[i];
            curr_node = curr_node->next;
        
            while(curr_node != nullptr) {
                target->next = new Node{curr_node->key, curr_node->value, nullptr};
                curr_node = curr_node->next;
                target = target->next;
            }
        }
    }
}


// Destructor
HashMap::~HashMap()
{   
    deleteBucketList();
}

// Assignment operator
HashMap& HashMap::operator=(const HashMap& hm)
{
    if (this != &hm)
    {
        totalBucketCount = hm.totalBucketCount;
        hashFunction = hm.hashFunction;
        totalPairCount = hm.totalPairCount;

        deleteBucketList();

        bucketList = new Node*[totalBucketCount];
        initializeBucketList(totalBucketCount);

        for(int i = 0; i < totalBucketCount; i++)
        {
            Node* curr_node = hm.bucketList[i];
            if (curr_node != nullptr) {
                bucketList[i] = new Node{curr_node->key, curr_node->value, nullptr};
                Node* target = bucketList[i];
                curr_node = curr_node->next;
        
                while(curr_node != nullptr) {
                    target->next = new Node{curr_node->key, curr_node->value, nullptr};
                    curr_node = curr_node->next;
                    target = target->next;
                }
            }
        }       
    }
    return *this;
}

// Adds a key and a value to bucketList, rehashes the array if needed
void HashMap::add(const std::string& key, const std::string& value) {

    if (contains(key) == true) {
        return;
    }

    addNode(key, value);
    totalPairCount += 1;

    if (loadFactor() > rehashPoint) {
        reHash();
    }
}

bool HashMap::remove(const std::string& key) 
{
    if (contains(key) == true)
    {
        int keyIndex = applyHash(key);
        Node* previous_node = bucketList[keyIndex];
        Node* current_node = bucketList[keyIndex]->next; // need to check if previous_node-> works

        // if the first node has the key
        if (bucketList[keyIndex]->key == key) { 
            delete previous_node;
            bucketList[keyIndex] = current_node;
        } else {
            while(current_node->key != key) {
                previous_node = current_node;
                current_node = current_node->next; // i think?? maybe previous_node->next instead?
            }
            previous_node->next = current_node->next; // skips the current node
            delete current_node; 
        }
        totalPairCount -= 1;
        return true;
    } 
    return false;
}

// Returns true if given key is in the HashMap, false otherwise
bool HashMap::contains(const std::string& key) const 
{
    int index = applyHash(key);
    Node* curr_bucket = bucketList[index];
    while (curr_bucket != nullptr)
    {
        if (curr_bucket->key == key) {
            return true;
        }
        curr_bucket = curr_bucket->next;
    }
    return false;
}

// Returns the value associated with the key, if the key is present in HashMap
std::string HashMap::value(const std::string& key) const 
{
    if (contains(key) == true)
    {
        int index = applyHash(key);
        Node* curr_bucket = bucketList[index];
        while (curr_bucket != nullptr) {
            if (curr_bucket->key == key) {
                return curr_bucket->value;
            }
            curr_bucket = curr_bucket->next;
        }
    }
    return "";
}

// Returns number of key/value pairs stored in the HashMap
unsigned int HashMap::size() const 
{
    return totalPairCount;
}

// Returns number of buckets currently allocated in the HashMap
unsigned int HashMap::bucketCount() const
{
    return totalBucketCount;
}

double HashMap::loadFactor() const
{
    return (double) size()/(double) totalBucketCount;
}

// Returns the number of the key/value pairs stored in the largest bucket
unsigned int HashMap::maxBucketSize() const
{
    unsigned int max = 0;
    for(int i = 0; i < totalBucketCount; i++)
    {
        if(max <= getNumNodes(bucketList[i]))
        {
            max = getNumNodes(bucketList[i]);
        }
    }
    return max;
}

// -----Private member functions-----
// Initializes the bucketList with nullptrs
void HashMap::initializeBucketList(int listlength)
{
    for(int i = 0; i < listlength; i++)
    {
        bucketList[i] = nullptr;
    }
}

// Adds a node to the end of the bucket
void HashMap::addNode(const std::string& key, const std::string& value)
{
    int newNodeIndex = applyHash(key);

    if (bucketList[newNodeIndex] == nullptr)
    {
        bucketList[newNodeIndex] = new Node{key, value, nullptr};
    } 
    else 
    {
        Node* current_bucket = bucketList[newNodeIndex];
        while (current_bucket->next != nullptr) {
            current_bucket = current_bucket->next;
        }
        current_bucket->next = new Node{key, value, nullptr};
    }

}

// Applies hash to a string and gets the index of the bucket list it should be in
int HashMap::applyHash(const std::string &key) const 
{
    int hashresult = hashFunction(key);
    int index = hashresult % totalBucketCount;
    return index;
}

// Rehashes the bucketList if the load factor is greater than .8
// Iterates through the old bucket list and adds each node to bucketList
void HashMap::reHash()
{
    int newBucketTotal = ((2 * totalBucketCount) + 1); // calculates the new bucket total
    Node** oldBucketList = bucketList;
    
    bucketList = new Node*[newBucketTotal];
    initializeBucketList(newBucketTotal);

    for(int i = 0; i < totalBucketCount; i++)
    {
        Node* temp = nullptr;
        Node* old_node = oldBucketList[i];
        while (old_node != nullptr) {
            addNode(old_node->key, old_node->value);
            temp = old_node;
            old_node = old_node->next;
            delete temp;
        } 
        delete old_node;
        
    }

    totalBucketCount = newBucketTotal;
    delete[] oldBucketList;
}

// Gets the number of nodes in a single bucket
int HashMap::getNumNodes(const Node* bucket) const 
{
    if (bucket == nullptr) {
        return 0;
    }
    return 1 + getNumNodes(bucket -> next);
}

// Deallocates the bucket/linked list 
void HashMap::deleteBucket(Node* node)
{
    Node* temp = node;
    while(node->next != nullptr) 
    {
        temp = node;
        node = node->next;
        delete temp;
    }
    delete node;
}

// Deallocates the entire bucketList 
void HashMap::deleteBucketList()
{
    for(int i = 0; i < totalBucketCount; i++) 
    {
        if(bucketList[i] != nullptr) {
            deleteBucket(bucketList[i]);
        } else {
            delete bucketList[i];
        } 
    }
    delete[] bucketList; 
}