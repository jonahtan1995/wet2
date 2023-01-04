#include "worldcup23a2.h"

//------------------------------------------------ private functions ------------------------------------

bool world_cup_t::teamExists(int teamId){
    Team* team = hash_teams->Search(teamId);
    return !(team == nullptr);
}

bool world_cup_t::playerExists(int playerId){
    return uf_groups->playerExists(playerId);
}

//------------------------------------------------ end of private functions ------------------------------------


world_cup_t::world_cup_t() : firstGroup(nullptr)
{
	try
    {
        tree_ability = new AVLtree<int,keyAbilityTeamId>();
    }
    catch (std::bad_alloc &)
    {
        throw;
    }
    try
    {
        hash_teams= new HashTable<Team> ();
    }
    catch (std::bad_alloc &)
    {
        delete tree_ability;
        throw;
    }
    try
    {
        uf_groups= new UnionFind();
    }
    catch (std::bad_alloc &)
    {
        delete tree_ability;
        delete hash_teams;
        throw;
    }
}

world_cup_t::~world_cup_t()
{
	delete tree_ability;
    delete hash_teams;
    delete uf_groups;
    //TODO: delete linked list of groups
}

StatusType world_cup_t::add_team(int teamId)
{
	if(teamId<=0)
    {
        return StatusType::INVALID_INPUT;
    }
    if(this->hash_teams->Search(teamId)!= nullptr)
    {
        return StatusType::FAILURE;
    }
    Group* newGroup;
    try{
        newGroup = new Group(teamId);
    }
    catch(std::bad_alloc&){
        return StatusType::ALLOCATION_ERROR;
    }
    if (!firstGroup){
        firstGroup = newGroup;
    }
    else{
        newGroup->next = firstGroup;
        firstGroup->prev = newGroup;
        firstGroup = newGroup;
    }
    newGroup->totalSpirit = newGroup->totalSpirit.neutral();
    //insert into hash of teams
    Team team;
    team.group = newGroup;
    HashTableStatus status = this->hash_teams->Insert(teamId,team);
    if (status == HASH_ALLOCATION_ERROR){
        return StatusType::ALLOCATION_ERROR;
    }
    //insert into ability tree
    keyAbilityTeamId key(teamId);
    try{
        this->tree_ability->m_root = this->tree_ability->insertNode(this->tree_ability->m_root,key,0);
    }
    catch (std::bad_alloc &){
        return StatusType::ALLOCATION_ERROR;
    }
	return StatusType::SUCCESS;
}

StatusType world_cup_t::remove_team(int teamId)
{
    if (teamId <= 0){
        return StatusType::INVALID_INPUT;
    }
	Team* team = hash_teams->Search(teamId);
    if (team){
        if (team->group->num_players != 0){
            team->group->active = false;
            keyAbilityTeamId key(teamId, team->group->totalAbility);
            tree_ability->m_root = tree_ability->deleteNode(tree_ability->m_root, key);
        }
        else{
            delete team->group;
            keyAbilityTeamId key(teamId, 0);
            tree_ability->m_root = tree_ability->deleteNode(tree_ability->m_root, key);
        }

        HashTableStatus status = hash_teams->Delete(teamId);
        if (status == HASH_SUCCESS){
            return StatusType::SUCCESS;
        }
        else if (status == HASH_ALLOCATION_ERROR){
            return StatusType::ALLOCATION_ERROR;
        }
    }
	return StatusType::FAILURE;
}

StatusType world_cup_t::add_player(int playerId, int teamId,
                                   const permutation_t &spirit, int gamesPlayed,
                                   int ability, int cards, bool goalKeeper)
{
    if (playerId <= 0 || teamId <= 0 || !spirit.isvalid() || gamesPlayed < 0 || cards < 0){
        return StatusType::INVALID_INPUT;
    }


    if (uf_groups->playerExists(playerId) || !teamExists(teamId)){
        return StatusType::FAILURE;
    }
    Team* team = hash_teams->Search(teamId);
    Group* group = team->group;
    //create new player
    Player* newPlayer = new Player(playerId, teamId, spirit,(gamesPlayed - group->gamesPlayed),ability,cards,goalKeeper);

    //update group because new player has been added
    group->totalSpirit = group->totalSpirit * spirit;
    if (group->num_players == 0){
        group->root = newPlayer;
        newPlayer->my_team = group;
    }
    else{
        newPlayer->parent = group->root;
        newPlayer->spirit = group->root->spirit.inv() * group->totalSpirit;
        newPlayer->gamesPlayed -= group->root->partialGamesPlayed;
    }
    group->youngest = newPlayer;
    group->num_players++;
    if (!group->hasGoalie){
        group->hasGoalie = goalKeeper;
    }
    //update ability tree:
    //remove from ability tree
    keyAbilityTeamId oldKey(teamId, group->totalAbility);
    tree_ability->m_root = tree_ability->deleteNode(tree_ability->m_root, oldKey);
    group->totalAbility+= ability;
    //insert back into ability tree
    keyAbilityTeamId newKey(teamId, group->totalAbility);
    tree_ability->m_root = tree_ability->insertNode(tree_ability->m_root, newKey,0);

    //insert into hash table of players
    PlayerHash* playerHash = new PlayerHash(newPlayer); //when do we delete this? maybe a problem. also need to add try catch
    HashTableStatus status = uf_groups->players->Insert(playerId, *playerHash);
    if (status == HASH_ALLOCATION_ERROR){
        return StatusType::ALLOCATION_ERROR;
    }
    else if (status == HASH_FAILURE){
        return StatusType::FAILURE;
    }
    playerHash->my_player = nullptr;
    delete playerHash;
	return StatusType::SUCCESS;
}


output_t<int> world_cup_t::play_match(int teamId1, int teamId2)
{
	if(teamId1<=0||teamId2<=0||teamId1==teamId2)
    {
        return StatusType::INVALID_INPUT;
    }
    Team* team1 = this-> hash_teams->Search(teamId1);
    Team* team2 = this-> hash_teams->Search(teamId2);



    if(team1==nullptr||team2== nullptr)
    {
        return StatusType::FAILURE;
    }
    Group* group1 = team1->group;
    Group* group2 = team2->group;
    if(!group1->hasGoalie || !group2->hasGoalie)
    {
        return StatusType::FAILURE;
    }

    group1->gamesPlayed+=1;
    group2->gamesPlayed+=1;

    int winNum1 = group1->totalAbility + group1->points;
    int winNum2 = group2->totalAbility + group2->points;
    // diffrent winNum values
    if(winNum1!=winNum2)
    {
        if(winNum1>winNum2)
        {
            group1->points+=3;
            return 1;
        }
        else
        {
            group2->points+=3;
            return 3;
        }
    }
    // equal winNum values
    else{
    if(group1->totalSpirit.strength()>group2->totalSpirit.strength())
    {
        group1->points+=3;
        return 2;

    }
    else if(group1->totalSpirit.strength()<group2->totalSpirit.strength())
    {
        group2->points+=3;
        return 4;

    }
    //this is draw
    else
    {
        group1->points+=1;
        group2->points+=1;
        return 0;
     }
    }

}

output_t<int> world_cup_t::num_played_games_for_player(int playerId)
{
    if (playerId <=0){
        return StatusType::INVALID_INPUT;
    }
    if (playerExists(playerId)){
        PlayerHash* playerHash = uf_groups->players->Search(playerId);
        Player* player = playerHash->my_player;
        return player->getNumGamesPlayed();
    }
    return StatusType::FAILURE;
}
//to review
StatusType world_cup_t::add_player_cards(int playerId, int cards)
{
    if(playerId<=0||cards<0)
    {
        return StatusType::INVALID_INPUT;
    }
    if (playerExists(playerId)){
        if (!uf_groups->Find(playerId)->active){
            return StatusType::FAILURE;
        }
        PlayerHash* playerHash = uf_groups->players->Search(playerId);
        Player* player = playerHash->my_player;
        player->cards += cards;
        return StatusType::SUCCESS;
    }
    return StatusType::FAILURE;

}

output_t<int> world_cup_t::get_player_cards(int playerId)
{
    if(playerId<=0)
    {
        return StatusType::INVALID_INPUT;
    }
    if (playerExists(playerId)){
        PlayerHash* playerHash = uf_groups->players->Search(playerId);
        Player* player = playerHash->my_player;
        return player->cards;
    }

    return StatusType::FAILURE;
}

output_t<int> world_cup_t::get_team_points(int teamId)
{
    if (teamId <=0){
        return StatusType::INVALID_INPUT;
    }
    if (teamExists(teamId)){
        Team* team = hash_teams->Search(teamId);
        return team->group->points;
    }
	return StatusType::FAILURE;
}

output_t<int> world_cup_t::get_ith_pointless_ability(int i)
{
	if (i<0 || i >= hash_teams->num_elements){
        return StatusType::FAILURE;
    }
    Node<int,keyAbilityTeamId>* current = tree_ability->select(tree_ability->m_root, i+1);
    return current->m_key.teamId;
}

output_t<permutation_t> world_cup_t::get_partial_spirit(int playerId)
{
	if(playerId<=0)
    {
        return StatusType::INVALID_INPUT;
    }
    if ((!playerExists(playerId)) || (!uf_groups->Find(playerId)->active)) {
        return StatusType::FAILURE;
    }
    return uf_groups->getPartialSpirit(playerId);

}

StatusType world_cup_t::buy_team(int teamId1, int teamId2)
{
	if (teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2){
        return StatusType::INVALID_INPUT;
    }
    if (!teamExists(teamId1) || !teamExists(teamId2)){
        return StatusType::FAILURE;
    }
    Team* team1 = hash_teams->Search(teamId1);
    Team* team2 = hash_teams->Search(teamId2);
    Group* group1 = team1->group;
    Group* group2 = team2->group;
    bool removed = (group2->num_players == 0);
    remove_team(teamId2); //return results from  here
    if (!removed){
        //remove group1 from ability tree
        keyAbilityTeamId oldKey(teamId1, group1->totalAbility);
        tree_ability->m_root = tree_ability->deleteNode(tree_ability->m_root, oldKey);
        uf_groups->Union(group1, group2);
        //insert group1 to ability tree
        keyAbilityTeamId newKey(teamId1, group1->totalAbility);
        tree_ability->m_root = tree_ability->insertNode(tree_ability->m_root, newKey,0);
    }
    return StatusType::SUCCESS;
}



