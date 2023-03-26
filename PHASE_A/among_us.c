/***************************************************
 *                                                 *
 * file: among_us.c                                *
 *                                                 *
 * @Author  Antonios Peris                         *
 * @Version 20-10-2020                             *
 * @email   csdp1196@csd.uoc.gr                    *
 *                                                 *
 * @brief   Implementation of among_us.h           *
 *                                                 *
 ***************************************************
 */

#include "among_us.h"

/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
int initialize() {
    struct Players *helper;
    struct Tasks *helpertask;
    struct Tasks *helpertask1;
    tasks_stack = (struct Head_Completed_Task_Stack *)malloc(sizeof(struct Head_Completed_Task_Stack));
    helpertask = (struct Tasks *)malloc(sizeof(struct Tasks));
    helpertask1 = (struct Tasks *)malloc(sizeof(struct Tasks));
    players_head = (struct Players *)malloc(sizeof(struct Players));
    helper = (struct Players *)malloc(sizeof(struct Players));
    tasks_head = (struct Head_GL *)malloc(sizeof(struct Head_GL));
    helpertask->next = NULL;
    helpertask1->next = NULL;
    tasks_head->head->next = NULL;
    helpertask->difficulty = 0;
    tasks_stack->head->next = NULL;
    helper->evidence = -1;
    helper->is_alien = -1;
    helper->pid = -1;
    total_players = 0;
    helper->next = players_head;
    helper->prev = players_head;
    players_head->next = helper;
    players_head->prev = helper;
    return 1;
}

int floorr(double number){
    return (number*10)/10;
}


int count_tasks(struct Tasks *player){
    struct Tasks *tasks = player;
    int count=0;
    while(tasks!=tasks->next->next){
        count++;
        tasks = tasks->next;
    }
    count++;
    return count;
}

int merge_tasks(struct Tasks *playerhead, struct Tasks *tasks) {
    int i=0,f=0,flag = 0;
    struct Tasks *pl;
    struct Tasks *task = tasks;
    struct Tasks *task1 = tasks;
    task1 = (struct Tasks *)malloc(sizeof(struct Tasks));
    task = (struct Tasks *)malloc(sizeof(struct Tasks));
    pl = playerhead;
    if(pl->difficulty == 3){
        while(task->next != task->next->next->next)task = task->next;
        task = task->next;
        pl->next = task->next;
        task->next = pl;
        pl->next->next = pl;
    }else{
        while(task1->difficulty != 0 && flag==0){
            if(task->difficulty >= pl->difficulty && i==0){
                pl->next = task;
                tasks = pl;
                flag = 1;
                f = 1;
            }else if(task->difficulty >= pl->difficulty && i!=0){
                pl->next = task1->next;
                task1->next = pl;
                flag = 1;
                f = 1;
            }
            i++;
            task = task->next;
            if(task->difficulty < pl->difficulty){
                task1 = task1->next;
            }
        }
    }
    free(task1);
    free(task);
    if(f)return 1;
    else return 0;
}

int deleteplayer(struct Players *player){
    struct Players *helper = players_head->next;
    struct Players *helper1 = players_head;
    while(helper!=players_head){
        if(helper == player){
            helper1->next = helper->next;
            helper->next->prev = helper1;
            return 1;
        }
        helper = helper->next;
        if(helper!=player)helper1 = helper1->next;
    }
    return 0;
}

struct Tasks *deletetask(struct Tasks *task,struct Players *helperplayer){
    struct Tasks *helper = helperplayer->tasks_head;
    struct Tasks *helper1 = helperplayer->tasks_head;

    while(helper!=helperplayer->tasks_sentinel){
        if(helper == task){
            helper1->next=helper->next;
            helper = helperplayer->tasks_sentinel;
        }
        helper=helper->next;
        if(helper != task)helper1 = helper1->next;
    }
    return task;
}


/**
 * @brief Register player
 *
 * @param pid The player's id
 *
 * @param is_alien The variable that decides if he is an alien or not
 * @return 1 on success
 *         0 on failure
 */
int register_player(int pid,int is_alien) {
    struct Players *newplayer;
    struct Tasks *task;
    newplayer = (struct Players *)malloc(sizeof(struct Players));
    task = (struct Tasks *)malloc(sizeof(struct Tasks));
    task->tid = -1;
    task->difficulty = 0;
    //task->next = NULL;
    //task->next = newplayer->tasks_head;
    newplayer->tasks_sentinel = task;
    newplayer->tasks_head = task;
    newplayer->tasks_sentinel->next = newplayer->tasks_head;
    newplayer->pid = pid;
    newplayer->is_alien = is_alien;
    newplayer->next = players_head;
    newplayer->prev = players_head;
    players_head = newplayer;
    newplayer = newplayer->next;
    newplayer->prev = players_head;
    print_players();
    return 1;
}

/**
 * @brief Insert task in the general task list
 *
 * @param tid The task id
 *
 * @param difficulty The difficulty of the task
 *
 * @return 1 on success
 *         0 on failure
 */
int insert_task(int tid,int difficulty) {
    int i=0,f=0;
    struct Tasks *newtask;
    struct Tasks *task;
    struct Tasks *task1;
    newtask = (struct Tasks *)malloc(sizeof(struct Tasks));
    task1 = (struct Tasks *)malloc(sizeof(struct Tasks));
    task = (struct Tasks *)malloc(sizeof(struct Tasks));
    task = tasks_head->head;
    task1 = tasks_head->head;
    newtask->tid = tid;
    newtask->difficulty = difficulty;
    if(task->next == NULL){
        newtask->next = NULL;
        tasks_head->head = newtask;
        print_tasks();
        return 1;
    }
    if(difficulty == 3){
        while(task->next != NULL)task = task->next;
        newtask->next = NULL;
        task->next = newtask;
        Total_Tasks++;
        print_tasks();
        return 1;
    }
    while(task1 != NULL){
        if(task->difficulty >= difficulty && i==0){
            newtask->next = task;
            tasks_head->head = newtask;
            //task = NULL;
            Total_Tasks++;
            f = 1;
        }else if(task->difficulty >= difficulty && i!=0){
            newtask->next = task1->next;
            task1->next = newtask;
           // task = NULL;
            Total_Tasks++;
            f = 1;
        }
        i++;
        task = task->next;
        if(task->difficulty < difficulty){
            task1 = task1->next;
        }
    }
    print_tasks();
    if(f)return 1;
    else return 0;
}

/**
 * @brief Distribute tasks to the players
 * @return 1 on success
 *         0 on failure
 */
int distribute_tasks() {
    int f = 0,i = 1;
    struct Players *player = players_head;
    struct Tasks *task = tasks_head->head;
    struct Tasks *taskcopy;
    while(task != NULL){
        while(player->is_alien==1){
            player = player->next;
            i++;
            if(i==total_players+1)player = players_head;
        }
        taskcopy = task;
        task = task->next;
        if(i <= 10){
            taskcopy->next = player->tasks_sentinel;
            player->tasks_head->next = taskcopy;
            player->tasks_sentinel->next = taskcopy;
            f = 1;
            i++;
        }else{
            taskcopy->next = player->tasks_sentinel;
            player->tasks_sentinel->next->next = taskcopy;
            player->tasks_sentinel->next = taskcopy;
            f = 1;
            i++;
        }
        player = player->next;
        if(i==total_players+1)player = players_head;

    }

    print_double_list();
    if(f)return 1;
    else return 0;
}

/**
 * @brief Implement Task
 *
 * @param pid The player's id
 *
 * @param difficulty The task's difficuly
 *
 * @return 1 on success
 *         0 on failure
 */
int implement_task(int pid, int difficulty) {
    struct Players *player = players_head;
    struct Tasks *task;
    struct Tasks *tmp;
    struct Tasks *dt;
    int flag = 0,f = 0;
    while(player->pid!=pid)player = player->next;
    task = player->tasks_head;
    while(task!=player->tasks_sentinel){
        if(task->difficulty == difficulty){
            flag = 1;
        }
        task = task->next;
    }
    task = player->tasks_head;
    tmp = player->tasks_head;
    if(flag){
        while(task!=player->tasks_sentinel && flag){
            task = task->next;
            if(task->difficulty == difficulty){
                    dt = task;
                    flag = 0;
                    f=1;
            }else tmp = tmp->next;
        }
    }else{
        while(task != player->tasks_sentinel->next && flag){
            task = task->next;
            if(task == player->tasks_sentinel->next){
                dt = task;
                f=1;
                flag = 0;
            }else tmp = tmp->next;
        }
    }
    tmp->next = dt->next;
    if(tmp->next == player->tasks_sentinel)player->tasks_sentinel->next = tmp;
    dt->next = tasks_stack->head;
    tasks_stack->head = dt;
    //print_stack();
    if(f)return 1;
    else return 0;
}

/**
 * @brief Eject Player
 *
 * @param pid The ejected player's id
 *
 * @return 1 on success
 *         0 on failure
 */
int eject_player(int pid) {
    struct Players *helppid = players_head->next;
    struct Players *helppid1;
    struct Players *helppidmintasks;
    struct Players *player;
    struct Tasks *helpertaskhead;
    struct Tasks *helpertaskhead1;
    int min = 100, counter;

    while(helppid!=players_head){
        if(helppid->pid == pid)helppid1 = helppid;
        helpertaskhead1 = helppid->tasks_head;
        counter = 0;
        while(helpertaskhead1!=helppid->tasks_sentinel)
        {
            counter++;
            helpertaskhead1 = helpertaskhead1->next;
        }
        if(helppid->is_alien == 0 && helppid!=players_head && counter < min){
            min = counter;
            helppidmintasks = helppid;
        }
        helppid = helppid->next;
    }

    helpertaskhead = helppidmintasks->tasks_head;
    deleteplayer(helppid1);
    player = helppid1;
    while(player->tasks_head!=player->tasks_sentinel){
        merge_tasks(player->tasks_head,helpertaskhead);
        player->tasks_head = player->tasks_head->next;
    }

    return 1;
}

/**
 * @brief Witness Eject Player
 *
 * @param pid_a The alien's pid
 *
 * @param pid The crewmate's pid
 *
 * @param number_of_witnesses The number of witnesses
 *
 * @return 1 on success
 *         0 on failure
 */
int witness_eject(int pid, int pid_a, int number_of_witnesses){
    struct Players *helppid1 = players_head->next;

    while(helppid1!=players_head){
        if(helppid1->pid == pid_a){
            helppid1->evidence += number_of_witnesses;
            helppid1=players_head;
        }
    }
    if(eject_player(pid))return 1;
    else return 0;
}


/**
 * @brief Sabbotage
 *
 * @param pid The player's id
 *
 * @param number_of_tasks The number of tasks to be popped
 *
 * @return 1 on success
 *         0 on failure
 */
int sabbotage(int pid, int number_of_tasks) {
    int firstplayer=0,i=0;
    struct Players *player;
    struct Players *helperplayer = players_head->next;
    struct Tasks *taskhead = tasks_stack->head;
    while(helperplayer!=players_head){
        if(helperplayer->pid == pid){
            player = helperplayer;
            if(helperplayer->next == players_head){
                printf("Player with pid %d didn't found\n" , pid);
                return 0;
            }
        }
        helperplayer=helperplayer->next;
    }
    firstplayer = floorr(number_of_tasks/2.0);
    while(i<firstplayer){
        player = player->prev;
        i++;
    }
    while(player->is_alien)player = player->next;
    i=0;
    while(i<number_of_tasks){
        while(player->is_alien || player == players_head)player = player->next;
        merge_tasks(taskhead, player->tasks_head);
        i++;
        taskhead = taskhead->next;
        player = player->next;
    }
    tasks_stack->head->next = taskhead;
    return 1;
}


/**
 * @brief Vote
 *
 * @param pid The player's id
 *
 * @param vote_evidence The vote's evidence
 *
 * @return 1 on success
 *         0 on failure
 */
int vote(int pid, int vote_evidence) {
    int f = 0,max = -1,pidh;
    struct Players *player = players_head->next;
    while(player != players_head){
        if(player->pid == pid){
            player->evidence+=vote_evidence;
            f = 1;
        }
        player = player->next;
    }
    if(!f){
        printf("Player with pid %d didn't found!", pid);
        return 0;
    }
    player = players_head->next;
    while(player != players_head){
        if(player->evidence>max){
            max = player->evidence;
            pidh = player->pid;
        }
        player = player->next;
    }
    eject_player(pidh);

    return 1;
}


/**
 * @brief Give Away Work
 *
 * @return 1 on success
 *         0 on failure
 */
int give_work() {
    struct Players *player_max;
    struct Players *player_min;
    struct Players *helperplayer = players_head->next;
    struct Tasks *helpertask;
    int max = -1,min = 100,i = 0,f=0;
    while(helperplayer != players_head){
        if(count_tasks(helperplayer->tasks_head) > max){
            max = count_tasks(helperplayer->tasks_head);
            player_max = helperplayer;
        }
        if(count_tasks(helperplayer->tasks_head) < min && !(helperplayer->is_alien)){
            min = count_tasks(helperplayer->tasks_head);
            player_min = helperplayer;
        }
        helperplayer = helperplayer->next;
    }
    //delete the task from the player_max and save it in the helpertask, then it put it in player_min.
    while(i < floorr(count_tasks(player_max->tasks_head)/2.0 )){
        helpertask = deletetask(player_max->tasks_head, player_max);
        merge_tasks(helpertask, player_min->tasks_head);
        f=1;
        i++;
    }
    if(f)return 1;
    else return 0;
}

/**
 * @brief Terminate
 *
 * @return 1 on success
 *         0 on failure
 */
int terminate() {
    struct Players *players = players_head->next;
    int counta=0,countc=0;
    while(players!=players_head){
        if(players->is_alien)counta++;
        else countc++;
    }
    if(counta > countc)printf("Aliens win.\n");
    if(Total_Tasks == count_tasks(tasks_stack->head) || counta == 0)printf("Crewmates win.\n");
    return 1;
}

/**
 * @brief Print Players
 *
 * @return 1 on success
 *         0 on failure
 */
int print_players() {
    struct Players *players = players_head->next;
    printf("players = ");
    while(players != players_head->prev){
        printf("<%d: %d>",players->pid, players->is_alien);
        players = players->next;
    }
    printf("\n");

    return 1;
}

/**
 * @brief Print Tasks
 *
 * @return 1 on success
 *         0 on failure
 */
int print_tasks() {
    struct Tasks *tasks = tasks_head->head;
    printf("List_Tasks = ");
    while(tasks != NULL){
        printf("<%d,%d>",tasks->tid,tasks->difficulty);
        tasks = tasks->next;
    }
    printf("\n");

    return 1;
}

/**
 * @brief Print Stack
 *
 * @return 1 on success
 *         0 on failure
 */
int print_stack() {
    struct Tasks *tasks = tasks_stack->head;
    printf("Stack_Tasks = ");
    while(tasks != NULL){
        printf("<%d,%d>",tasks->tid,tasks->difficulty);
        tasks = tasks->next;
    }
    printf("\n");

    return 1;
}

/**
 * @brief Print Players & Task List
 *
 * @return 1 on success
 *         0 on failure
 */
int print_double_list() {
    struct Players *players = players_head->next;
    struct Tasks *tasks;
    int i=1;
    while(players != players_head){
        tasks = players->tasks_head;
        printf("Player%d=",i);
        while(tasks != players->tasks_sentinel){
            printf("<%d,%d>,",tasks->tid,tasks->difficulty);
            tasks = tasks->next;
        }
        printf("\n");
        i++;
        players = players->next;
    }
    return 1;
}

/**
 * @brief Free resources
 *
 * @return 1 on success
 *         0 on failure
 */

int free_all(void) {
    return 1;
}

