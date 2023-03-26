/***************************************************************
 *                                                             *
 * file: main.c                                                *
 *                                                             *
 * @Author  Antonios Peris                         		   	   *
 * @Version 20-10-2020                             			   *
 * @email   csdp1196@csd.uoc.gr                                *
 *                                                             *
 * @brief   Main function for the needs of cs-240 project 2020 *
 *                                                             *
 ***************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "among_us.c"

#include "among_us.h"

#define BUFFER_SIZE 1024 /* Maximum length of a line in input file */

/* Uncomment the following line to enable debugging prints
 * or comment to disable it */
#define DEBUG
#ifdef DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#else /* DEBUG */
#define DPRINT(...)
#endif /* DEBUG */

/**
 * @brief The main function
 *
 * @param argc Number of arguments
 * @param argv Argument vector
 *
 * @return 0 on success
 *         1 on failure
 */
int main(int argc, char **argv)
{
	FILE *fin = NULL;
	char buff[BUFFER_SIZE], event='\0';

	/* Check command buff arguments */
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <input_file> <>\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* Open input file */
	if ((fin = fopen(argv[1], "r")) == NULL)
	{
		fprintf(stderr, "\n Could not open file: %s\n", argv[1]);
		perror("Opening test file\n");
		return EXIT_FAILURE;
	}

	/* Initializations */
	initialize();

	/* Read input file buff-by-buff and handle the events */
	while (fgets(buff, BUFFER_SIZE, fin))
	{

		DPRINT("\n>>> Event: %s", buff);

		switch (buff[0])
		{

		/* Comment */
		case '#':
			break;

		/* Register player
			 * P <pid><is_alien> */
		case 'P':
		{
			int pid;
			int is_alien;
			sscanf(buff, "%c %d %d", &event, &pid, &is_alien);
			DPRINT("%c %d %d\n", event, pid, is_alien);

			if (register_player(pid, is_alien))
			{
				DPRINT("P %d %d succeeded\n", pid, is_alien);
			}
			else
			{
				fprintf(stderr, "P %d %d failed\n", pid, is_alien);
			}

			break;
		}

		/* Insert task
			 * T <tid><difficulty>  */
		case 'T':
		{
			int tid, difficulty;

			sscanf(buff, "%c %d %d", &event, &tid, &difficulty);
			DPRINT("%c %d %d\n", event, tid, difficulty);

			if (insert_task(tid, difficulty))
			{
				DPRINT("%c %d %d succeded\n", event, tid, difficulty);
			}
			else
			{
				fprintf(stderr, "%c %d %d failed\n", event, tid, difficulty);
			}

			break;
		}

		/* Distribute Tasks
			 * D */
		case 'D':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (distribute_tasks())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}

		/* Implement Task
			 * I <pid> <difficulty> */
		case 'I':
		{
			int pid,difficulty;

			sscanf(buff, "%c %d %d", &event, &pid,&difficulty);
			DPRINT("%c %d %d \n", event, pid,difficulty);

			if (implement_task(pid,difficulty))
			{
				DPRINT("%c %d %d succeeded\n", event, pid,difficulty);
			}
			else
			{
				fprintf(stderr, "%c %d %d failed\n", event, pid,difficulty);
			}

			break;
		}

		/* Eject Player
			 * E <pid>*/
		case 'E':
		{
			int pid;

			sscanf(buff, "%c %d", &event, &pid);
			DPRINT("%c %d\n", event, pid);

			if (eject_player(pid))
			{
				DPRINT("%c %d succeeded\n", event, pid);
			}
			else
			{
				fprintf(stderr, "%c %d failed\n", event, pid);
			}

			break;
		}

			/* Witness Ejection
                 * W <pid> <pid_a> <number_of_witnesses>
                 */
		case 'W':
		{
			int pid ,pid_a ,number_of_witnesses;

			sscanf(buff, "%c %d %d %d", &event, &pid ,&pid_a ,&number_of_witnesses);
			DPRINT("%c %d %d %d\n", event, pid ,pid_a ,number_of_witnesses);

			if (witness_eject(pid ,pid_a ,number_of_witnesses))
			{
				DPRINT("%c %d %d %d succeded\n", event, pid ,pid_a ,number_of_witnesses);
			}
			else
			{
				fprintf(stderr, "%c %d %d %d failed\n", event, pid ,pid_a ,number_of_witnesses);
			}

			break;
		}

		/* Sabbotage
			 * S <number_of_tasks><pid> */
		case 'S':
		{
			int pid,number_of_tasks;

			sscanf(buff, "%c %d %d\n", &event, &number_of_tasks,&pid);
			DPRINT("%c %d %d\n", event, number_of_tasks,pid);

			if (sabbotage(pid,number_of_tasks))
			{
				DPRINT("%c %d %d succeeded\n", event,  number_of_tasks,pid);
			}
			else
			{
				fprintf(stderr, "%c %d %d failed\n", event,  number_of_tasks,pid);
			}

			break;
		}

		/* Vote
			 * V <pid> <vote_evidence> */
		case 'V':
		{
			int pid, vote_evidence;

			sscanf(buff, "%c %d %d\n", &event, &pid, &vote_evidence);
			DPRINT("%c %d %d\n", event, pid,vote_evidence);

			if (vote(pid,vote_evidence))
			{
				DPRINT("%c %d %d succeeded\n", event,  pid,vote_evidence);
			}
			else
			{
				fprintf(stderr, "%c %d %d failed\n", event,  pid,vote_evidence);
			}

			break;
		}

		/* Give Away Work
			 * G */
		case 'G':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (give_work())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}

		/* Terminate
			 * F */
		case 'F':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (terminate())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}
		/* Print Players
			 * X */
		case 'X':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (print_players())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}
		/* Print Tasks
			 * Y */
		case 'Y':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (print_tasks())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}
		/* Print Stack
			 * Z */
		case 'Z':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (print_stack())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}
		/* Print Players & Tasks list
			 * F */
		case 'U':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if (print_double_list())
			{
				DPRINT("%c succeeded\n", event);
			}
			else
			{
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}
		/* Empty line */
		case '\n':
			break;

		/* Ignore everything else */
		default:
			DPRINT("Ignoring buff: %s \n", buff);

			break;
		}
	}

	free_all();
	return (EXIT_SUCCESS);
}
