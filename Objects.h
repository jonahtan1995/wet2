#ifndef WET2_OBJECTS_H
#define WET2_OBJECTS_H

#include "wet2util.h"
class Player;

class Group{
public:
    int teamId;
    int points;
    int num_players;
    bool active;
    bool hasGoalie;
    Player* youngest;
    int totalAbility;
    Player* root;
    int gamesPlayed;
    permutation_t totalSpirit;
    Group* next;
    Group* prev;

    Group(int teamId): teamId(teamId), points(0),num_players(0), active(true), hasGoalie(false), youngest(nullptr), totalAbility(0), root(
            nullptr), gamesPlayed(0), next(nullptr), prev(nullptr){};
    ~Group()
    {
        youngest = nullptr;
        root = nullptr;
        next = nullptr;
        prev = nullptr;
    }
};
class Player{
public:

    int playerId;
    int teamId; //not sure if necessary
    permutation_t spirit;
    int gamesPlayed;
    int ability;
    int cards;
    bool goalKeeper;
    Player* parent;
    //permutation_t partialSpirit;
    int partialGamesPlayed;
    Group* my_team;

    Player(int playerId, int teamId, const permutation_t& spirit, int gamesPlayed, int ability, int cards, bool goalKeeper):
        playerId(playerId), teamId(teamId), spirit(spirit), gamesPlayed(gamesPlayed), ability(ability), cards(cards), goalKeeper(goalKeeper), parent(
            nullptr), partialGamesPlayed(0), my_team(nullptr) {}; //partialSpirit(spirit.neutral())
    ~Player(){
        parent = nullptr;
        delete parent;
        //delete my_team;
        my_team = nullptr;
    }
    int getNumGamesPlayed(){
        Player* player = this;
        int sumGames = player->gamesPlayed;
        while (player->parent){
            sumGames += player->partialGamesPlayed;
            player = player->parent;
        }
        return sumGames + player->partialGamesPlayed + player->my_team->gamesPlayed;
    }
};



class Team{
public:
    Group* group;

    Team(): group(nullptr) {};
    ~Team(){
        /*
        if (group->num_players == 0){
            delete group;
        }*/
        group = nullptr;
        delete group;
    }
};

class PlayerHash{
public:
    Player* my_player;

    PlayerHash(Player* my_player = nullptr): my_player(my_player) {};
    ~PlayerHash(){
        delete my_player;
    }
};

class keyAbilityTeamId{
public:
    int teamId;
    int ability;
    keyAbilityTeamId(int teamId, int ability = 0): teamId(teamId), ability(ability){}
    bool operator<(const keyAbilityTeamId& other) const{
        if (ability == other.ability){
            return teamId < other.teamId;
        }
        return ability < other.ability;
    }

    bool operator>(const keyAbilityTeamId& other) const{
        return other < *this;
    }
/*
    bool operator<(const int& other) const {
        return ability < other;
    }

    bool operator>(const int& other) const{
        return  !(*this < other) ;
    }
    */
    bool operator==(const keyAbilityTeamId& other) const{
        return this->ability == other.ability && this->teamId == other.teamId;
    }
    bool operator!=(const keyAbilityTeamId& other) const{
        return !(this->ability == other.ability && this->teamId == other.teamId);
    }
};


#endif //WET2_OBJECTS_H
