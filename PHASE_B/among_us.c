/***************************************************
 *                                                 *
 * file: among_us.c                                *
 *                                                 *
 * @Author  Skerdi Basha                           *
 * @Version 1-12-2020                              *
 * @email   sbash@csd.uoc.gr                       *
 *                                                 *
 * @brief   Implementation of among_us.h           *
 *                                                 *
 ***************************************************
 */

#include "among_us.h"

// get as a parameter a task.

int isLeaf_task(struct Task *task){
    if(task->lc == NULL && task->rc == NULL)return 1;
    return 0;
}

int has_one_child(struct Task *task){
    if((task->lc != NULL && task->rc == NULL) || (task->lc != NULL && task->rc != NULL))return 1;
    return 0;
}

int has_two_children(struct Task *task){
    if(task->lc != NULL && task->rc != NULL)return 1;
    return 0;
}
// get as a parameter a player.

int isLeaf_player(struct Player *player){
    if(player->lc == NULL && player->rc == NULL)return 1;
    return 0;
}

int has_one_child_player(struct Player *player){
    if((player->lc != NULL && player->rc == NULL) || (player->lc != NULL && player->rc != NULL))return 1;
    return 0;
}

int has_two_children_player(struct Player *player){
    if(player->lc != NULL && player->rc != NULL)return 1;
    return 0;
}

unsigned int primes_g[100] = {  5,   7,   11,  13,  17,  19,  23,  29,  31,  37,
                                41,  43,  47,  53,  59,  61,  67,  71,  73,  79,
                                83,  89,  97,  101, 103, 107, 109, 113, 127, 131,
                                137, 139, 149, 151, 157, 163, 167, 173, 179, 181,
                                191, 193, 197, 199, 211, 223, 227, 229, 233, 239,
                                241, 251, 257, 263, 269, 271, 277, 281, 283, 293,
                                307, 311, 313, 317, 331, 337, 347, 349, 353, 359,
                                367, 373, 379, 383, 389, 397, 401, 409, 419, 421,
                                431, 433, 439, 443, 449, 457, 461, 463, 467, 479,
                                487, 491, 499, 503, 509, 521, 523, 541, 547, 557  };

/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
int initialize() {
    int i = 0;
    NumberOfPlayers = 0;
    if(max_tasks_g/100>=1)size_of_hash_table = max_tasks_g/10;
    else size_of_hash_table = 10;
    players_sentinel = (struct Player*)malloc(sizeof(struct Player));
    players_tree = (struct Player*)malloc(sizeof(struct Player));
    players_tree->lc = (struct Player *)malloc(sizeof(struct Player));
    players_tree->rc = (struct Player *)malloc(sizeof(struct Player));
    players_tree->tasks = (struct Task*)malloc(sizeof(struct Task));

    // player's tree
    players_tree->parrent = NULL;
    players_tree->lc = players_sentinel;
    players_tree->rc = players_sentinel;
    players_tree->tasks = NULL;

    //player's sentinel
    players_sentinel->parrent = players_tree;

    //general task's HT
    general_tasks_ht = (struct General_Tasks_HT*)malloc(sizeof(struct General_Tasks_HT));
    general_tasks_ht->count = 0;

    //Hash table malloc
    general_tasks_ht->tasks = (struct HT_Task**)malloc(size_of_hash_table * sizeof(struct HT_Task));

    //Hash table initialize
    for(;i<size_of_hash_table;i++){
        general_tasks_ht->tasks[i] = (struct HT_Task*)malloc(size_of_hash_table * sizeof(struct HT_Task));
        general_tasks_ht->tasks[i] = NULL;
    }

    //completed tasks initialize
    completed_tasks_pq = (struct Completed_Tasks_PQ*)malloc(sizeof(struct Completed_Tasks_PQ));
    completed_tasks_pq->tasks = (struct HT_Task*)malloc(max_tasks_g * sizeof(struct HT_Task));
    completed_tasks_pq->size = 0;

    //global variable
    parrent_task = NULL;
    playerwithpid = NULL;
    parrent_player = NULL;
    ejected_player_tasks = (struct Task*)malloc(sizeof(struct Task));
    ejected_player_tasks = NULL;
    number_of_crewmates = 0;
    number_of_aliens = 0;
    number_of_distributed_tasks = 0;
    max_evidence_player = 0;

    return 1;
}

/**
 * @brief Register Player
 *
 * @param pid The player's id
 *
 * @param is_alien The variable that decides if he is an alien or not
 * @return 1 on success
 *         0 on failure
 */
int register_player(int pid, int is_alien) {
    struct Player *newplayer;
    newplayer = (struct Player *)malloc(sizeof(struct Player)); // create new player.
    newplayer->tasks = (struct Task *)malloc(sizeof(struct Task)); // create new player's tasks tree.
    newplayer->lc = (struct Player *)malloc(sizeof(struct Player)); // create new player's lc pointer.
    newplayer->rc = (struct Player *)malloc(sizeof(struct Player)); // create new player's rc pointer.
    newplayer->evidence = 0;
    newplayer->is_alien = is_alien;
    newplayer->pid = pid;
    newplayer->lc = players_sentinel;
    newplayer->rc = players_sentinel;
    newplayer->parrent = NULL;
    newplayer->tasks = NULL;
    if(newplayer->is_alien)number_of_aliens++;
    else number_of_crewmates++;
    if(NumberOfPlayers == 0){
        players_tree = newplayer;
        NumberOfPlayers++;
    }
    else{
        register_player_recursion(players_tree, newplayer);
        NumberOfPlayers++;
    }
    print_players();
    return 1;
}

void register_player_recursion(struct Player *playertree, struct Player *newplayer){

    if(newplayer->pid > playertree->pid){
        if(playertree->rc == players_sentinel){
            playertree->rc = newplayer;
            newplayer->parrent = playertree;
            return;
        }else register_player_recursion(playertree->rc, newplayer);
    }else if(newplayer->pid < playertree->pid){
        if(playertree->lc == players_sentinel){
            playertree->lc = newplayer;
            newplayer->parrent = playertree;
            return;
        }else register_player_recursion(playertree->lc, newplayer);
    }
    return;
}


/**
 * @brief Insert Task in the general task hash table
 *
 * @param tid The task id
 *
 * @param difficulty The difficulty of the task
 *
 * @return 1 on success
 *         0 on failure
 */
int insert_task(int tid, int difficulty) {
    struct HT_Task *new_ht_task = (struct HT_Task*)malloc(sizeof(struct HT_Task));
    new_ht_task->difficulty = difficulty;
    new_ht_task->tid = tid;
    new_ht_task->next = NULL;
    if(general_tasks_ht->tasks[tid % size_of_hash_table] == NULL){
        general_tasks_ht->tasks[tid % size_of_hash_table] = new_ht_task;
        general_tasks_ht->count++;
        print_T();
        return 1;
    }
    else{
        new_ht_task->next = general_tasks_ht->tasks[tid % size_of_hash_table]->next;
        general_tasks_ht->tasks[tid % size_of_hash_table]->next = new_ht_task;
        general_tasks_ht->count++;
        print_T();
        return 1;
    }
    return 0;
}

void print_T(){
    int i;
    struct HT_Task *tasks;
    for(i = 0; i < size_of_hash_table; i++){
        tasks = general_tasks_ht->tasks[i];
        printf("Chain%d:",i);
        while(tasks != NULL){
            if(tasks->next == NULL)printf("<%d,%d>",tasks->tid,tasks->difficulty);
            else printf("<%d,%d>,",tasks->tid,tasks->difficulty);
            tasks = tasks->next;
        }
        printf("\n");
    }
    return;
}

/**
 * @brief Distribute Tasks to the players
 * @return 1 on success
 *         0 on failure
 */
int distribute_tasks() {
    int i = 0, counter = 1;
    struct HT_Task *GL_task;
    for(;i<general_tasks_ht->count;i++){    //while the Hush table isn't finished.
        GL_task = general_tasks_ht->tasks[i];
        while(GL_task != NULL){    //while the chain isn't null.       O(number of tasks).
            number_of_distributed_tasks++;
            find_player_and_give_task(players_tree, counter%NumberOfPlayers, 1, GL_task->tid, GL_task->difficulty); //gives the task to players.
            GL_task = GL_task->next;
            counter++;
        }
    }
    print_double_tree();
    return 1;
}

void find_player_and_give_task(struct Player *player,int which_player,int counter,int tid,int difficulty){
    struct Task *newtask;
    newtask = (struct Task*)malloc(sizeof(struct Task));
    newtask->lc = NULL;
    newtask->rc = NULL;
    newtask->tid = tid;
    newtask->difficulty = difficulty;
    newtask->lcnt = 0;
    if(player == players_sentinel)return;
    if(counter == which_player){    //till the chosen player isn't alien.
        if(player->is_alien){
            find_player_and_give_task(player, which_player+1, counter, tid, difficulty);
            return;
        }
    }
    if(counter == which_player){
        if(player->tasks == NULL){  //if the player has no tasks.
        player->tasks = newtask;
        }else give_task(player->tasks, tid, difficulty);
        return;
    }
    find_player_and_give_task(player->lc,which_player,counter+1,tid,difficulty);
    find_player_and_give_task(player->rc,which_player,counter+1,tid,difficulty);
    free(newtask);  //it will be free if it isn't used;
}

void give_task(struct Task *player_task,int tid,int difficulty){   //give the tasks to players.
    struct Task *newtask;
    newtask = (struct Task*)malloc(sizeof(struct Task));
    newtask->lc = NULL;
    newtask->rc = NULL;
    newtask->tid = tid;
    newtask->difficulty = difficulty;
    newtask->lcnt = 0;
    if(tid > player_task->tid){  // decide to to go right
        if(player_task->rc == NULL){
            player_task->rc = newtask;
            return;
        }else give_task(player_task->rc,tid,difficulty);
    }else if(tid < player_task->tid){
        if(player_task->lc == NULL){
            player_task->lc = newtask;
            return;
        }else {
            give_task(player_task->lc,tid,difficulty);
            player_task->lcnt++;
        }
    }
    free(newtask);  //it will be free if it isn't used;
    return;
}

/**
 * @brief Implement Task
 *
 * @param pid The player's id
 *
 * @param tid The task's tid
 *
 * @return 1 on success
 *         0 on failure
 */
int implement_task(int pid, int tid) {
    struct Task *parent_task = NULL;
    implement_player_task(players_tree,parent_task,pid,tid);
    return 1;
}

void implement_player_task(struct Player *playertree,struct Task *parent_task,int pid,int tid){
    if(playertree == players_sentinel)return;
    if(playertree->pid == pid){         //if the player is found.
        parent_task = playertree->tasks;
        if(playertree->tasks == NULL){  //if there are no tasks.
            printf("The player with pid %d has no tasks!",pid);
        }else{
            delete_task(playertree->tasks,parent_task,playertree->tasks,tid);
        }
        return;
    }
    implement_player_task(playertree->lc,parent_task,pid,tid);
    implement_player_task(playertree->rc,parent_task,pid,tid);
}


void delete_task(struct Task *playertask,struct Task *parent_task,struct Task *tasktree,int tid){
    int i;
    struct Task *second_player;
    if(playertask == NULL)return;
    if(playertask->tid == tid){
        //second_player = (struct Task*)malloc(sizeof(struct Task));
        if(isLeaf_task(playertask)){
            add_complited_task(playertask->tid,playertask->difficulty);
            playertask = NULL;
        }else if(has_one_child(playertask)){
            add_complited_task(playertask->tid,playertask->difficulty);
            if(parent_task == playertask){
                if(playertask->lc == NULL){
                    parent_task = playertask->rc;
                    playertask->rc = NULL;
                }else {
                    parent_task = playertask->lc;
                    playertask->lc = NULL;
                }
            }else{
                if(parent_task->lc == playertask){
                    if(playertask->lc == NULL)parent_task->lc = playertask->rc;
                    else parent_task->lc = playertask->lc;
                }else if(parent_task->rc == playertask){
                    if(playertask->lc == NULL)parent_task->rc = playertask->rc;
                    else parent_task->rc = playertask->lc;
                }
            }
        }else if(has_two_children(playertask)){
            i = task_number(tasktree,playertask,1);
            second_player = find_next_task(tasktree,i+1,1);
            add_complited_task(playertask->tid,playertask->difficulty);
            playertask->difficulty = second_player->difficulty;
            playertask->tid = second_player->tid;
            delete_task(tasktree,tasktree,tasktree,second_player->tid);
        }
        return;
    }
    if((playertask->lc != NULL && playertask->lc->tid == tid) || (playertask->rc != NULL && playertask->rc->tid == tid))parent_task = playertask;
    delete_task(playertask->lc,parent_task,tasktree,tid);
    delete_task(playertask->rc,parent_task,tasktree,tid);
}

int task_number(struct Task *tasktree,struct Task *task, int number){     //return the number of task according to InOrder.
    if(tasktree == task)return number;
    task_number(tasktree->lc,task,number);
    number++;
    task_number(tasktree->rc,task,number);
    return number;
}

struct Task *find_next_task(struct Task *tasktree,int which_task,int counter){   //find and return the task that is after which_task according to InOrder.
    if(tasktree == NULL){
        return parrent_task;
    }
    if(counter == which_task){
        parrent_task = tasktree;
        return parrent_task;
    }
    find_next_task(tasktree->lc,which_task,counter);
    counter++;
    find_next_task(tasktree->rc,which_task,counter);
    return parrent_task;
}


void add_complited_task(int tid,int difficulty){
    completed_tasks_pq->tasks[completed_tasks_pq->size].tid = tid;
    completed_tasks_pq->tasks[completed_tasks_pq->size].difficulty = difficulty;
    completed_tasks_pq->tasks[completed_tasks_pq->size].next = NULL;
    completed_tasks_pq->size++;
}

/**
 * @brief Eject Player
 *
 * @param pid_1 The ejected player's id
 *
 * @param pid_2 The crewmates id
 *
 * @return 1 on success
 *         0 on failure
 */
int eject_player(int pid_1, int pid_2) {
    int i;
    struct Player *ejected_player;
    ejected_player = player_with_pid(players_tree,pid_1);
    if(ejected_player == NULL){
        printf("Player with pid %d didn't found",pid_1);
        return 0;
    }
    i = player_number(players_tree,pid_2,1);
    if(ejected_player->is_alien)number_of_aliens--;
    else number_of_crewmates--;
    delete_player(ejected_player);
    merge_tasks(ejected_player_tasks,i);
    return 1;
}


void delete_player(struct Player *ejected_player){
    struct Player *second_player = NULL;
    int i;
    if(isLeaf_player(ejected_player)){
        if(ejected_player == ejected_player->parrent->lc)ejected_player->parrent->lc = NULL;
        else if(ejected_player == ejected_player->parrent->rc)ejected_player->parrent->rc = NULL;
    }
    if(has_one_child_player(ejected_player)){
        if(ejected_player->lc == NULL){
            if(ejected_player == ejected_player->parrent->lc){
                ejected_player->parrent->lc = ejected_player->rc;
                ejected_player->rc->parrent = ejected_player->parrent;
            }
            else if(ejected_player == ejected_player->parrent->rc){
                ejected_player->parrent->rc = ejected_player->rc;
                ejected_player->rc->parrent = ejected_player->parrent;
            }
        }else if(ejected_player->rc == NULL){
            if(ejected_player == ejected_player->parrent->lc){
                ejected_player->parrent->lc = ejected_player->lc;
                ejected_player->lc->parrent = ejected_player->parrent;
            }
            else if(ejected_player == ejected_player->parrent->rc){
                ejected_player->parrent->rc = ejected_player->lc;
                ejected_player->lc->parrent = ejected_player->parrent;
            }
        }
    }
    if(has_two_children_player(ejected_player)){
        i = player_number(players_tree,ejected_player->pid,1);
        second_player = find_next_player(players_tree,i+1,1);
        if(second_player == NULL)second_player = find_next_player(players_tree,i-1,1);
        ejected_player->evidence = second_player->evidence;
        ejected_player->is_alien = second_player->is_alien;
        ejected_player->pid = second_player->pid;
        ejected_player_tasks = ejected_player->tasks;
        ejected_player->tasks = second_player->tasks;
        delete_player(second_player);
    }
}

struct Player *player_with_pid(struct Player *playerstree,int pid){
    if(playerstree == NULL)return playerwithpid;
    if(playerstree->pid == pid){
        playerwithpid = playerstree;
        return playerwithpid;
    }
    player_with_pid(playerstree->lc,pid);
    player_with_pid(playerstree->rc,pid);
    return playerwithpid;
}

struct Player *find_next_player(struct Player *playertree,int which_task,int counter){   //find and return the task that is after which_task according to InOrder.
    if(playertree == NULL){
        printf("There is no next task\n");
        return parrent_player;
    }
    if(counter == which_task){
        parrent_player = playertree;
        return parrent_player;
    }
    find_next_player(playertree->lc,which_task,counter);
    counter++;
    find_next_player(playertree->rc,which_task,counter);
    return parrent_player;
}

int player_number(struct Player *playertree,int ejected_player, int number){     //return the number of player according to InOrder.
    if(playertree == players_sentinel)return number;
    if(playertree->pid == ejected_player)return number;
    player_number(playertree->lc,ejected_player,number);
    number++;
    player_number(playertree->rc,ejected_player,number);
    return number;
}

void merge_tasks(struct Task *tasktree, int i){
    if(tasktree == NULL)return;
    find_player_and_give_task(players_tree,i,1,tasktree->tid,tasktree->difficulty);
    merge_tasks(tasktree->lc,i);
    merge_tasks(tasktree->rc,i);
}

/**
 * @brief Witness Eject Player
 *
 * @param pid_1 The ejected player's id
 *
 * @param pid_2 The crewmate's pid
 *
 * @param pid_a The alien's pid
 *
 * @param number_of_witnesses The number of witnesses
 *
 * @return 1 on success
 *         0 on failure
 */
int witness_eject(int pid_1, int pid_2, int pid_a, int number_of_witnesses){
    eject_player(pid_1,pid_2);
    add_evidence(players_tree, pid_a, number_of_witnesses);
    return 1;
}

void add_evidence(struct Player *playertree, int pid, int number_of_witnesses){
    if(playertree == NULL)return;
    if(playertree->pid == pid){
        playertree->evidence += number_of_witnesses;
        return;
    }
    add_evidence(playertree->lc,pid,number_of_witnesses);
    add_evidence(playertree->rc,pid,number_of_witnesses);
}

/**
 * @brief Sabotage
 *
 * @param number_of_tasks The number of tasks to be sabotaged
 *
 * @param pid The player's id
 *
 * @return 1 on success
 *         0 on failure
 */
int sabotage(int number_of_tasks, int pid) {
    return 1;
}

/**
 * @brief Vote
 *
 * @param pid_1 The suspicious player's id
 *
 * @param pid_2 The crewmate's pid
 *
 * @param vote_evidence The vote's evidence
 *
 * @return 1 on success
 *         0 on failure
 */
int vote(int pid_1, int pid_2, int vote_evidence) {
    add_evidence(players_tree,pid_1,vote_evidence);
    find_max_evidence_player(players_tree);
    eject_player(max_evidence_player_pid,pid_2);
    return 1;
}

void find_max_evidence_player(struct Player *playerstree){
    if(playerstree == NULL)return;
    if(playerstree->evidence > max_evidence_player){
        max_evidence_player_pid = playerstree->pid;
        max_evidence_player = playerstree->evidence;
    }
    find_max_evidence_player(playerstree->lc);
    find_max_evidence_player(playerstree->rc);
}

/**
 * @brief Give Away Work
 *
 * @param pid_1 The existing crewmate's id
 *
 * @param pid_2 The new crewmate's pid
 *
 * @return 1 on success
 *         0 on failure
 */
int give_work(int pid_1, int pid_2) {
    return 1;
}

/**
 * @brief Terminate
 *
 * @return 1 on success
 *         0 on failure
 */
int terminate() {
    if(number_of_aliens > number_of_crewmates){
        printf("Aliens win.\n");
        return 1;
    }else if(number_of_aliens == 0 || number_of_distributed_tasks == completed_tasks_pq->size){
        printf("Crewmates win.\n");
        return 1;
    }
    return 0;
}

/**
 * @brief Print Players
 *
 * @return 1 on success
 *         0 on failure
 */
int print_players() {
    printf("Players=");
    print_pl(players_tree);
    printf("\n");
    return 1;
}
void print_pl(struct Player *PlayersTree){
    if(PlayersTree == players_sentinel)return;
    printf("<%d:%d>",PlayersTree->pid,PlayersTree->is_alien);
    print_pl(PlayersTree->lc);
    print_pl(PlayersTree->rc);
}

/**
 * @brief Print Tasks
 *
 * @return 1 on success
 *         0 on failure
 */
int print_tasks() {
    int i;
    struct HT_Task *tasks;
    for(i = 0; i < size_of_hash_table; i++){
        tasks = general_tasks_ht->tasks[i];
        printf("Index%d:",i);
        while(tasks != NULL){
            if(tasks->next == NULL)printf("<%d,%d>",tasks->tid,tasks->difficulty);
            else printf("<%d,%d>,",tasks->tid,tasks->difficulty);
            tasks = tasks->next;
        }
        printf("\n");
    }
    return 1;
}


/**
 * @brief Print Priority Queue
 *
 * @return 1 on success
 *         0 on failure
 */
int print_pq() {
    int i;
    printf("Completed Tasks = ");
    for(i = 0;i < completed_tasks_pq->size;i++){
        printf("<%d,%d>",completed_tasks_pq->tasks[i].tid,completed_tasks_pq->tasks[i].difficulty);
    }
    printf("\n");
    return 1;
}

/**
 * @brief Print Players & Task tree
 *
 * @return 1 on success
 *         0 on failure
 */
int print_double_tree() {
    print_player_tasks(players_tree, 0);
    return 1;
}
void print_t(struct Task *tasktree){    //used to print the tasks of the player in function print_player_tasks.
    if(tasktree == NULL)return;
    printf("<%d:%d>",tasktree->tid,tasktree->difficulty);
    print_t(tasktree->lc);
    print_t(tasktree->rc);
}


void print_player_tasks(struct Player *players,int counter){
    if(players == players_sentinel)return;
    counter++;
    printf("Player%d:",counter);
    print_t(players->tasks);
    printf("\n");
    print_player_tasks(players->lc,counter);
    counter++;
    print_player_tasks(players->rc,counter);
}


/**
 * @brief Free resources
 *
 * @return 1 on success
 *         0 on failure
 */

int free_all(void) {
    int i;
    free_players(players_tree);
    for(i = 0;i<size_of_hash_table;i++)free_hash_table(general_tasks_ht->tasks[i]);
    free(completed_tasks_pq->tasks);
    free(parrent_task);
    free(parrent_player);
    free(playerwithpid);
    free(ejected_player_tasks);
    return 1;
}

void free_players(struct Player *playerstree){
    if(playerstree == players_sentinel)return;
    free_tasks(playerstree->tasks);
    free_players(playerstree->lc);
    free_players(playerstree->rc);
    free(playerstree);
}

void free_tasks(struct Task *tasktree){
    int i;
    if(tasktree == NULL)return;
    free_tasks(tasktree->lc);
    free_tasks(tasktree->rc);
    free(tasktree);
    for(i = 0;i<size_of_hash_table;i++)free_hash_table(general_tasks_ht->tasks[i]);
}

void free_hash_table(struct HT_Task *general_tasks){
    if(general_tasks == NULL)return;
    free_hash_table(general_tasks->next);
    free(general_tasks);
}

