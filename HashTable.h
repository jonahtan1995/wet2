#ifndef WET2_HASHTABLE_H
#define WET2_HASHTABLE_H

#include <cmath>
#include "AVLtree.h" //later
#define A ((sqrt(5)-1)/2)


typedef enum {
    HASH_SUCCESS,
    HASH_ALLOCATION_ERROR,
    HASH_INVALID_INPUT,
    HASH_FAILURE,
} HashTableStatus;

template <typename T>
class HashTable{
public:
    int size;
    int num_elements;
    AVLtree<T,int>** table;

    HashTable();
    ~HashTable();
    int Hash(int key); //key = teamID or playerID
    HashTableStatus Insert(int key, const T& element);
    T* Search(int key);
    HashTableStatus Delete(int key);

    HashTableStatus ReSize(int newSize);

};

template <class T>
HashTable<T>::HashTable(): size(2), num_elements(0){
    try {
        table = new AVLtree<T, int> *[size];
    }
    catch  (std::bad_alloc &){
        throw;
    }
    for (int i = 0; i < size; i++){
        table[i] = nullptr;
    }
}

template <class T>
HashTable<T>::~HashTable(){
    for (int i = 0; i < size; i++){
        //if (table[i]){
            delete table[i]; //make sure this deletes what we want
        //}
    }
    delete[] table;
}

template <class T>
int HashTable<T>::Hash(int key){
    double frac = ((double)key)*A;
    double not_relevant = 0;
    return size*(modf(frac, &not_relevant)); //make sure modf function works here
}

template <class T>
HashTableStatus HashTable<T>::Insert(int key, const T& element){
    if (key<=0){
        return HASH_FAILURE;
    }
    int index = Hash(key);
    if (!table[index]){
        AVLtree<T,int>* newTree;
        try{
            newTree = new AVLtree<T,int>();
        }
        catch(std::bad_alloc&){
            return HASH_ALLOCATION_ERROR;
        }
        newTree->m_root = newTree->insertNode(nullptr, key, element);
        table[index] = newTree;
    }
    else{
        if (!table[index]->findNode(table[index]->m_root, key)) {
            table[index]->m_root= table[index]->insertNode(table[index]->m_root, key, element);
        }
        else{
            return HASH_FAILURE;
        }
    }
    num_elements++;
    if (num_elements > size/2){ // if the number of elements is more than half of the array, we double the size of the array
        return this->ReSize(size*2); //do we need to return the result from here?
    }
    return HASH_SUCCESS;
}

template <class T>
T* HashTable<T>::Search(int key){
    int index = Hash(key);
    if (table[index]){
        Node<T,int>* current = table[index]->findNode(table[index]->m_root, key);
        if (current){
            return &current->m_element;
        }
    }
    return nullptr;
}

template <class T>
HashTableStatus HashTable<T>::Delete(int key){
    int index = Hash(key);
    if (!table[index] || !table[index]->findNode(table[index]->m_root, key)){
        return HASH_FAILURE;
    }
    table[index]->m_root = table[index]->deleteNode(table[index]->m_root, key);
    num_elements--;
    if (num_elements < size/4){ //if the number of elements is less than 25% of the size of the array, we cut it in half
        return this->ReSize(size/2); //do we need to return the result from here?
    }
    return HASH_SUCCESS;
}

template <class T>
HashTableStatus HashTable<T>::ReSize(int newSize){
    int oldSize = size;
    size = newSize;
    AVLtree<T,int>** newTable;
    try {
        newTable = new AVLtree<T,int> *[size];
    }
    catch (std::bad_alloc &){
        return HASH_ALLOCATION_ERROR;
    }
    for (int i = 0; i< size; i++){ //is this in the time complexity? yes, if the size of the table is O of the num of elements
        newTable[i] = nullptr;
    }
    for(int i = 0; i < oldSize; i++){
        if (table[i]) {
            Node<T,int> **curr_tree = new Node<T,int> *[table[i]->num_elements];
            table[i]->inOrderVisitUnite(table[i]->m_root, curr_tree, 0);
            for (int j = 0; j < table[i]->num_elements; j++) {
                int newIndex = Hash(curr_tree[j]->m_key);
                if (!newTable[newIndex]){
                    AVLtree<T,int>* newTree = new AVLtree<T,int>();
                    newTree->m_root = newTree->insertNode(nullptr, curr_tree[j]->m_key, curr_tree[j]->m_element);
                    newTable[newIndex] = newTree;
                }
                else{
                    newTable[newIndex]->m_root = newTable[newIndex]->insertNode(newTable[newIndex]->m_root, curr_tree[j]->m_key, curr_tree[j]->m_element);
                }

                curr_tree[j]->m_right_son = nullptr;
                curr_tree[j]->m_left_son = nullptr;
                T element;
                curr_tree[j]->m_element = element;
                delete curr_tree[j];//make sure this doesnt delete the element!!
            }
            delete[] curr_tree;
            table[i]->m_root = nullptr;
            delete table[i];
        }
    }
    delete[] table;
    table = newTable;

    return HASH_SUCCESS;
}

#endif