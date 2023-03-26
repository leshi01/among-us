/**************************************************
 *                                                *
 * file: among_us.h                               *
 *                                                *
 *                                                *
 * @Author  Skerdi Basha                          *
 * @Version 1-12-2020                             *
 * @email   sbash@csd.uoc.gr                      *
 *                                                *
 * @brief   Header file for the needs of CS-240   *
 * project 2020, with the structures and function *
 * prototypes                                     *
 *                                                *
 **************************************************
 */

#ifndef __AMONG_US_H_
#define __AMONG_US_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * Structure defining a node of the players double linked tree
 * tree
 */
struct Player
{
    int pid;                        /*Player's identifier*/
    int is_alien;                   /*Alien flag*/
    int evidence;                   /*Amount of evidence*/
    struct Player *parrent;         /*Pointer to the parent node*/
    struct Player *lc;              /*Pointer to the left child node*/
    struct Player *rc;              /*Pointer to the right child node*/
    struct Task *tasks;             /*Pointer to the head of player's task tree*/
};

/**
 * Structure defining a node of the tasks sorted tree
 */
struct Task
{
    int tid;                        /*Task's identifier*/
    int difficulty;                 /*Task's difficulty*/
    int lcnt;                       /*Node's left child's node counter*/
    struct Task *lc;                /*Pointer to the left child node*/
    struct Task *rc;                /*Pointer to the right child node*/
};

struct HT_Task
{
    int tid;                        /*Task's identifier*/
    int difficulty;                 /*Task's difficulty*/
    struct HT_Task *next;           /*Pointer to the next node*/
};

struct General_Tasks_HT
{
    int count;                        /*Count of tasks*/
    struct HT_Task **tasks;           /*General tasks hash table*/
};

struct Completed_Tasks_PQ
{
    int size;                       /*Count of completed tasks*/
    struct HT_Task *tasks;         /*Completed tasks priority queue*/
};

unsigned int max_tasks_g; /* Max number of tasks */
unsigned int max_tid_g;   /* Max task tid */
unsigned int size_of_hash_table;

/* Primes for your universal hashing implementation */
extern unsigned int primes_g[100];

int NumberOfPlayers;

/* Global variable, pointer to the head of the players tree */
struct Player *players_tree;

/* Global variable, pointer to the head of the tasks tree */
struct General_Tasks_HT *general_tasks_ht;

/* Global variable, pointer to the top of the completed task's priority queue */
struct Completed_Tasks_PQ *completed_tasks_pq;

struct Player *players_sentinel;

void print_pl(struct Player *PlayersTree);

void print_T();

void print_player_tasks(struct Player *players,int counter);

void print_t(struct Task *tasktree);

void find_player_and_give_task(struct Player *player,int which_player,int counter,int tid,int difficulty);

void give_task(struct Task *player,int tid,int difficulty);

void register_player_recursion(struct Player *playertree, struct Player *newplayer);

void implement_player_task(struct Player *playertree,struct Task *parent_task,int pid,int tid);

void delete_task(struct Task *playertask,struct Task *parent_task,struct Task *tasktree,int tid);

int isLeaf_task(struct Task *task);

int has_one_child(struct Task *task);

int has_two_children(struct Task *task);

int task_number(struct Task *tasktree,struct Task *task, int number);

struct Task *find_next_task(struct Task *tasktree,int which_task,int counter);

struct Player *find_next_player(struct Player *playertree,int which_task,int counter);

int player_number(struct Player *playertree,int ejected_player, int number);

void add_complited_task(int tid,int difficulty);

struct Player *player_with_pid(struct Player *playerstree,int pid);

int isLeaf_player(struct Player *player);

int has_one_child_player(struct Player *player);

int has_two_children_player(struct Player *player);

void delete_player(struct Player *ejected_player);

void merge_tasks(struct Task *ejected_player_tasks,int i);

void add_evidence(struct Player *playertree, int pid, int number_of_witnesses);

void find_max_evidence_player(struct Player *players_tree);

void free_players(struct Player *playerstree);

void free_tasks(struct Task *tasktree);

void free_hash_table(struct HT_Task *general_tasks);


//global variable
struct Task *parrent_task;
struct Player *parrent_player;
struct Player *playerwithpid;
struct Task *ejected_player_tasks;
int number_of_crewmates;
int number_of_aliens;
int number_of_distributed_tasks;
int max_evidence_player;
int max_evidence_player_pid;


/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
int initialize();

/**
 * @brief Register Player
 *
 * @param pid The player's id
 *
 * @param is_alien The variable that decides if he is an alien or not
 * @return 1 on success
 *         0 on failure
 */
int register_player(int pid, int is_alien);

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
int insert_task(int tid, int difficulty);

/**
 * @brief Distribute Tasks to the players
 * @return 1 on success
 *         0 on failure
 */
int distribute_tasks();

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
int implement_task(int pid, int tid);

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
int eject_player(int pid_1, int pid_2);

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
int witness_eject(int pid_1, int pid_2, int pid_a, int number_of_witnesses);

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
int sabotage(int number_of_tasks, int pid);

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
int vote(int pid_1, int pid_2, int vote_evidence);

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
int give_work(int pid_1, int pid_2);

/**
 * @brief Terminate
 *
 * @return 1 on success
 *         0 on failure
 */
int terminate();

/**
 * @brief Print Players
 *
 * @return 1 on success
 *         0 on failure
 */
int print_players();

/**
 * @brief Print Tasks
 *
 * @return 1 on success
 *         0 on failure
 */
int print_tasks();

/**
 * @brief Print Priority Queue
 *
 * @return 1 on success
 *         0 on failure
 */
int print_pq();

/**
 * @brief Print Players & Task tree
 *
 * @return 1 on success
 *         0 on failure
 */
int print_double_tree();

/**
 * @brief Free resources
 *
 * @return 1 on success
 *         0 on failure
 */

int free_all(void);

#endif /* __AMONG_US_H_ */
