/*
 *
 *Implementation of warmup1.c from the warmup1 assignment
 *
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>

#include "my402list.h"
#include "cs402.h"
#include "my402list.h"

#define MAX_LINE_LENGTH 1024
#define MAX_DESC_LENGTH 24
#define MAX_AMOUNT 10000000

typedef struct tagTxn {
    char type;              
    time_t ts;              
    int cents; 
    char desc[MAX_DESC_LENGTH + 1];        
} Txn;



static void PrintTransactions(My402List *list);
static void SortedInsert(My402List *list, Txn *trans, const char *filename);

static void FormatDate(time_t ts, char *buf);
static void FormatAmount(int cents, int is_negative, char *buf);

int main(int argc, char *argv[]){
	FILE *fp = NULL;

	if (argc < 2 || argc > 3){
		Usage();
	}

	if (strcmp(argv[1], "sort") != 0){
		fprintf(stderr, "error: unknown command '%s'\n", argv[1]);
		Usage();
	}

	if (argc == 3){
		struct stat st;
		
		if (stat(argv[2], &st) != 0){
			fprintf(stderr, "error: cannot open file '%s' - %s\n", 
                    argv[2], strerror(errno));
            exit(1);
		}
		
		if (S_ISDIR(st.st_mode)){
			fprintf(stderr, "error: '%s' is a directory\n", argv[2]);
            exit(1);
		}

		fp = fopen(argv[2], "r");
		
		if (fp == NULL){
			fprintf(stderr, "error: cannot open file '%s' - %s\n", 
                    argv[2], strerror(errno));
            exit(1);
		}

		ProcessFile(fp, argv[2]);
		fclose(fp);
	} else {
		ProcessFile(stdin, "stdin");
	}
	return 0;
}

static void SortedInsert(My402List *list, Txn *trans, const char *filename){
	My402ListElem *elem;

	for (elem = My402ListFirst(list); elem != NULL; elem = My402ListNext(list,elem)){
		Txn *curr = (Txn *)(elem->obj);

		if (curr->ts == trans->ts){
			fprintf(stderr, "error: duplicate timestamp in '%s'\n", filename);
			free(trans);
			//Free all transactions before exiting
			exit(1);
		}

		if (curr->ts > trans->ts){
			if (!My402ListInsertBefore(list, trans, elem)){
				fprintf(stderr, "error: failed to insert transaction\n");
                free(trans);
                //Free all transactions before exiting
                exit(1);
			}
			return
		}
	}

	if (!My402ListAppend(list, trans)){
		fprintf(stderr, "error: failed to insert transaction\n");
        free(trans);
        //Free all transactions before exiting
        exit(1);
	}
}

static void FormatAmount(int cents, int is_negative, char *buf){
	int dollars = cents / 100;
	int remainder = cents % 100;

	if (cents >= MAX_AMOUNT * 100){
		if (is_negative){
			strcpy(buf, "(?,???,???" ".??" ")");
		} else {
			strcpy(buf, " ?,???,???" ".??" " ");
		}
		return;
	}

	char num_buf[20];
	sprintf(num_buf, "%d.%02d", dollars, cent_part);

	char format_buf[20];
	int dec_pos = (int)(strchr(num_buf, '.') - num_buf);
	int j = 0;

	for (int i = 0; i < dec_pos; i++){
		format_buf[j] = num_buf[i];
		j++;
		int remaining = dec_pos - i - 1;
		if (remaining > 0 && remaining % 3 == 0){
			format_buf[j] = ',';
			j++;
		}
	}

	strcpy(format_buf + j, num_buf + dec_pos);

	if (is_negative){
		sprintf(buf, "(%12s)", format_buf);
	} else {
		sprintf(buf, "%13s ", format_buf);
	}
}

/*
 *Format date thats passed in as a cstring
 *PRE: the buf is of size buf[16] to fit with output format
 *
 */
static void FormatDate(time_t ts, char *buf){
	char *time_str = ctime(&ts);

	strncpy(buf, time_str, 8);
	strncpy(buf + 8, time_str + 8, 3);
	strncpy(buf + 11, time_str + 20, 4);

	buf[15] = '\0';
}

static void PrintTransactions(My402List *list){
	My402ListElem *elem;
	long balance = 0;

	char *border = "+-----------------+--------------------------+----------------+----------------+";
	char *header = "|       Date      | Description              |         Amount |        Balance |";

	printf("%s\n", border);
    printf("%s\n", header);
    printf("%s\n", border);

    for (elem = My402ListFirst(list); elem != NULL; elem = My402ListNext(list, elem)){
    	Txn *trans = (Txn *)(elem->object);

    	if (trans->type == '+'){
    		balance += trans->cents;
    	} else {
    		balance -= trans->cents;
    	}
    }

    char date_buf[16];
    FormatDate(trans->ts, date_buf);

    char amount_buf[16];
    int is_negative = (trans->type == '-');
    FormatAmount(trans->cents, is_negative, amount_buf);


}