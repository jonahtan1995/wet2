#include "UnionFind.h"

UnionFind::UnionFind(){
    try{
        players = new HashTable<PlayerHash>();
    }
    catch (std::bad_alloc &){
        throw;
    }
    /*
    try{
        teams = new HashTable<Team>();
    }
    catch (std::bad_alloc &){
        delete players;
        throw;
    }
     */
}

UnionFind::~UnionFind(){
    /*
    for (int i = 0; i < players.size; i++){
        if (players[i]){
            Node<PlayerHash>** currTree = new Node<PlayerHash>*[players[i]->num_elements];
            players[i]->inOrderVisitUnite(players[i]->m_root, currTree,0);
            for (int j = 0; j < players[i]->num_elements; j++){
                delete currTree[j]->m_element;
                currTree[j]->m_right_son = nullptr;
                currTree[j]->m_left_son = nullptr;
                delete currTree[j];
            }
            delete[] currTree;
            delete players[i]
        }
    }
     */
    delete players; //does this delete content of the pointers each player is pointing at?
    //delete teams;
}

Group* UnionFind::Find(int playerId){
    if (!playerExists(playerId)){
        return nullptr;
    }
    PlayerHash* playerHash = players->Search(playerId);
    Player* player = playerHash->my_player;
    Player* temp = player;
    int totalGames = 0;
    permutation_t totalSpirit;//check if this is the way to start totalSpirit
    totalSpirit = totalSpirit.neutral();
    while (temp->parent){
        totalGames += temp->partialGamesPlayed;
        totalSpirit = temp->spirit * totalSpirit;
        temp = temp->parent;
    }
    Player* root = temp;
    temp = player;
    while (temp->parent){
        int gamesToSubtract = temp->partialGamesPlayed;
        permutation_t spiritToDivide = temp->spirit;

        temp->partialGamesPlayed = totalGames;
        temp->spirit = totalSpirit;

        Player* tempParent = temp->parent;
        temp->parent = root;
        temp = tempParent;

        totalGames -= gamesToSubtract;
        totalSpirit = totalSpirit * spiritToDivide.inv();
    }
    return root->my_team;
}

void UnionFind::Union(Group* group1, Group* group2){
    //Group* group1 = teams->Search(teamId1)->group;
    //Group* group2 = teams->Search(teamId2)->group;
    Player* root1 = group1->root;
    Player* root2 = group2->root;
    if (group1->num_players == 0){
        group1->youngest = group2->youngest;
        group1->num_players += group2->num_players;
        group1->hasGoalie = group2->hasGoalie;
        group1->totalAbility += group2->totalAbility;
        group1->gamesPlayed += group2->gamesPlayed;
        group1->points += group2->points;
        group1->totalSpirit = group1->totalSpirit * group2->totalSpirit;
        group1->root = root2;
        root2->my_team = group1;
        delete group2;
        return;
    }
    root2->partialGamesPlayed -= group1->gamesPlayed;
    root1->partialGamesPlayed -= group2->gamesPlayed;
    if (group1->num_players >= group2->num_players){
        //add players of group 2 into group 1
        root2->parent = root1;
        root2->my_team = nullptr;
        root2->partialGamesPlayed -= root1->partialGamesPlayed;
        if (root1->playerId != group1->youngest->playerId){
            root2->spirit = root1->spirit.inv() * getPartialSpirit(group1->youngest->playerId) * root2->spirit;
        }
    }
    else{
        //add players of group1 into group 2
        root1->parent = root2;
        root2->my_team = group1;
        root1->my_team = nullptr;
        root1->partialGamesPlayed -= root2->partialGamesPlayed;
        root1->spirit = (root2->spirit).inv() * root1->spirit ;
        group1->root = root2;
    }



    group1->youngest = group2->youngest;
    group1->num_players += group2->num_players;
    if (!group1->hasGoalie){
        group1->hasGoalie = group2->hasGoalie;
    }
    group1->totalAbility += group2->totalAbility;
    group1->gamesPlayed += group2->gamesPlayed;
    group1->points += group2->points;
    group1->totalSpirit = group1->totalSpirit * group2->totalSpirit;
    delete group2;
}

permutation_t UnionFind::getPartialSpirit(int playerId){
    permutation_t res;
    if (!playerExists(playerId)){
        return res;
    }
    PlayerHash* playerHash = players->Search(playerId);
    Player* player = playerHash->my_player;
    res = res.neutral();
    while (player){
        res = player->spirit * res;
        player = player->parent;
    }
    return res;
}

bool UnionFind::playerExists(int playerId){
    PlayerHash* playerHash = players->Search(playerId);
    return playerHash != nullptr;
}

