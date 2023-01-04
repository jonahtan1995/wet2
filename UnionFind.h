#ifndef WET2_UNIONFIND_H
#define WET2_UNIONFIND_H

#include "HashTable.h"
#include "Objects.h"

class UnionFind{
public:
    HashTable<PlayerHash>* players;
   // HashTable<Team>* teams;

    Group* Find(int playerId);
    void Union(Group* group1, Group* group2);
    bool playerExists(int playerId);

public:
    UnionFind();
    ~UnionFind();
    permutation_t getPartialSpirit(int playerId);
};

#endif //WET2_UNIONFIND_H
