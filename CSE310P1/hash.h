#ifndef HASH_H
#define HASH_H

#include <iostream>
#include <string>
#include <cmath>
// You are free to use additional libraries as long as it's not PROHIBITED per instruction.

using namespace std;

//Node structure for chaining linked list
struct Node {
    string token;
    Node* next;
    Node(string t);
};

class HashTable {
    private:
        int numberSlots; //Number of slots in the hash table
        Node** table; //Array for linked list heads
        int hash_function(const string& token); //hash function

    public:
        HashTable(int k);
        ~HashTable();
        void insert(const string& token);
        void printFirst5Slots();
        void printSlotLengths();
        double computeStdDevelopment();
};

#endif // HASH_H
