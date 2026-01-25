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



static void PrintTransactions(My402list *list);










static void PrintTransactions(My402list *list){
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

    char *time_str = ctime(&trans->timestamp);
    char date_buf[16]

    strncpy(date_buf, time_str, 4);
    strncpy(date_buf + 4, time_str + 4, 4);
    strncpy(date_buf + 8, time_str + 8, 3);
    strncpy(date_buf + 11, time_str + 20, 4);
    date_buf[15] = '\0';

    char amount_buf[16];
    FormatAmount

}