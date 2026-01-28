/*
 *
 *Implementation of warmup1.c from the warmup1 assignment
 *
 *Usage: ./warmup1 sort FILENAME || ./warmup1 sort STDIN
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

#define MAX_LINE_LENGTH 1024
#define MAX_DESC_LENGTH 24
#define MAX_AMOUNT 10000000

typedef struct tagTxn {
    char type;              
    time_t ts;              
    int cents; 
    char desc[MAX_DESC_LENGTH + 1];
    int line_num;        
} Txn;


static void Usage(void);
static void ProcessFile(FILE *fp, const char *filename);
static int ParseLine(char *line, Txn *trans, int line_num, const char *filename);
static void PrintTransactions(My402List *list);
static void SortedInsert(My402List *list, Txn *trans, const char *filename);
static void FreeTxns(My402List *list);

static void FormatDate(time_t ts, char *buf);
static void FormatAmount(int cents, int is_negative, char *buf);

int main(int argc, char *argv[]){
	FILE *fp = NULL;

	//must have 2 or 3 args
	if (argc < 2 || argc > 3){
		fprintf(stderr, "error: malformed command\n");
		Usage();
	}

	//first arg must be "sort"
	if (strcmp(argv[1], "sort") != 0){
		fprintf(stderr, "error: malformed command, '%s' is not a valid commandline option\n", argv[1]);
		Usage();
	}

	if (argc == 3){
		struct stat st;
		
		//check if file exists
		if (stat(argv[2], &st) != 0){
			fprintf(stderr, "error: cannot open file '%s' - %s\n", 
                    argv[2], strerror(errno));
            exit(1);
		}
		
		//check if input is directory
		if (S_ISDIR(st.st_mode)){
			fprintf(stderr, "error: '%s' is a directory, input must be a file or stdin\n", argv[2]);
            exit(1);
		}

		//Open file for reading
		fp = fopen(argv[2], "r");
		
		if (fp == NULL){
			fprintf(stderr, "error: cannot open file '%s' - %s\n", 
                    argv[2], strerror(errno));
            exit(1);
		}

		ProcessFile(fp, argv[2]);
		fclose(fp);
	} else {
		//No file, read from stdin
		ProcessFile(stdin, "stdin");
	}
	return 0;
}

static void Usage(void){
	fprintf(stderr, "usage: warmup1 sort [tfile]\n");
	exit(1);
}

static void ProcessFile(FILE *fp, const char *filename){
	My402List list;
	char line[MAX_LINE_LENGTH + 2]; //+2 to detect lines > 1024
	int line_num = 0;
	int txn_count = 0;

	if (!My402ListInit(&list)){
		fprintf(stderr, "error: failed to initialize list\n");
        exit(1);
	}

	//read file line by line
	while (fgets(line, sizeof(line), fp) != NULL){
		line_num++;
		size_t len = strlen(line);

		//Check for lines that are too long
		if (len > 0 && line[len - 1] != '\n'){
			if (len >= MAX_LINE_LENGTH){
				fprintf(stderr, "error: line %d in '%s' is too long\n", 
                        line_num, filename);
                FreeTxns(&list); 
                exit(1);
			}
		} else if (len > 0) {
			//remove training /n
			line[len - 1] = '\0';
			len--;
		}
		//skip empty lines
		if (len == 0){
			continue;
		}

		//allocate new Txn object on heap, needs to persist
		Txn *trans = (Txn *)malloc(sizeof(Txn));
		if (trans == NULL){
			fprintf(stderr, "error: malloc failed\n");
            FreeTxns(&list);
            exit(1);
		}

		//parse line into struct
		if (!ParseLine(line, trans, line_num, filename)){
			free(trans);
			FreeTxns(&list);
			exit(1);
		}

		SortedInsert(&list, trans, filename);
		txn_count++;
	}

	//Must have at least one transaction
	if (txn_count == 0){
		fprintf(stderr, "error: '%s' contains no valid transactions\n", filename);
        My402ListUnlinkAll(&list);
        exit(1);
	}

	PrintTransactions(&list);

	FreeTxns(&list);

}

static int ParseLine(char *line, Txn *trans, int line_num, const char *filename){
	char *fields[4];
	int field_count = 0;
	char *ptr = line;
	char *start = line;

	//Split line by tab character
	while (*ptr != '\0' && field_count < 4){
		if (*ptr == '\t'){
			*ptr = '\0';
			fields[field_count] = start;
			field_count++;
			start = ptr + 1;
		}
		ptr++;
	}

	//capture last field 
	if (field_count < 4){
		fields[field_count] = start;
		field_count++;
	}

	//Must have exactly 4 fields
	if (field_count != 4) {
        fprintf(stderr, "error: line %d in '%s' is not in the right format (incorrect number of fields)\n", 
            line_num, filename);
        return FALSE;
    }

    //Parse Transaction type (+ or -), must be one character
    if (strlen(fields[0]) != 1 || (fields[0][0] != '+' && fields[0][0] != '-')){
    	fprintf(stderr, "error: line %d in '%s' has invalid transaction type\n", 
                line_num, filename);
        return FALSE;
    }
    trans->type = fields[0][0];

    //Parse Timestamp
    char *ts_str = fields[1];
    size_t ts_len = strlen(ts_str);

    //must be 1 - 10 digits
    if (ts_len == 0 || ts_len >= 11){
    	fprintf(stderr, "error: line %d in '%s' has invalid timestamp\n", 
                line_num, filename);
        return FALSE;
    }

    //no leading zeros
    if (ts_str[0] == '0' && ts_len > 1){
        fprintf(stderr, "error: line %d in '%s' has timestamp with leading zero\n", 
                line_num, filename);
        return FALSE;
    }

    //all characters must be digits
    for (size_t i = 0; i < ts_len; i++){
        if (!isdigit((unsigned char)ts_str[i])){
            fprintf(stderr, "error: line %d in '%s' has non-numeric timestamp\n", 
                    line_num, filename);
            return FALSE;
        }
    }

    char *endptr;
    unsigned long ts_val = strtoul(ts_str, &endptr, 10);
    
    if (*endptr != '\0' || ts_val == 0){
        fprintf(stderr, "error: line %d in '%s' has invalid timestamp\n", 
                line_num, filename);
        return FALSE;
    }

    //Timestamp must be in the past
    time_t current_time = time(NULL); 
    if ((time_t)ts_val >= current_time){
        fprintf(stderr, "error: line %d in '%s' has timestamp in the future\n", 
                line_num, filename);
        return FALSE;
    }
    
    trans->ts = (time_t)ts_val;  

    //Parse Amount 
    char *amount_str = fields[2];
    size_t amt_len = strlen(amount_str);

    if (amt_len == 0){
    	fprintf(stderr, "error: line %d in '%s' has empty amount\n", 
                line_num, filename);
        return FALSE;
    }

    //Must have decimal point
    char *dec = strchr(amount_str, '.');
    if (dec == NULL){
    	fprintf(stderr, "error: line %d in '%s' has amount without decimal point\n", 
                line_num, filename);
        return FALSE;
    }

    //Exactly 2 digits after decimal 
    size_t dec_len = strlen(dec + 1);
    if (dec_len != 2 || !isdigit((unsigned char)dec[1]) || !isdigit((unsigned char)dec[2])){
    	fprintf(stderr, "error: line %d in '%s' has invalid decimal places in amount\n", 
                line_num, filename);
        return FALSE;
    }

    //Integer part must be 1-7 digits
    size_t int_len = dec - amount_str;
    if (int_len == 0 || int_len > 7){
    	fprintf(stderr, "error: line %d in '%s' has invalid integer part in amount\n", 
                line_num, filename);
        return FALSE;
    }

    //No leading zeroes except 0.XX
    if (int_len > 1 && amount_str[0] == '0'){
    	fprintf(stderr, "error: line %d in '%s' has amount with leading zero\n", 
                line_num, filename);
        return FALSE;
    }

    //All chars before decimal must be digits
    for (size_t i = 0; i < int_len; i++){
    	if (!isdigit((unsigned char)amount_str[i])){
    		fprintf(stderr, "error: line %d in '%s' has non-numeric amount\n", 
                    line_num, filename);
            return FALSE;
    	}
    }

    //Convert to cents: split at decimal, combine dollars*100 + cents, ascii to int
    *dec = '\0';
    int dollars = atoi(amount_str);
    int cents = atoi(dec + 1);
    trans->cents = dollars * 100 + cents;

    if (trans->cents <= 0){
    	fprintf(stderr, "error: line %d in '%s' has zero or negative amount\n", 
                line_num, filename);
        return FALSE;
    }

    //Parse Description 
    char *desc = fields[3];
    //Trim Whitespace
    while (*desc == ' '){
    	desc++;
    }

    //check if empty after trimming (not allowed)
    if (*desc == '\0'){
    	fprintf(stderr, "error: line %d in '%s' has empty description\n", 
                line_num, filename);
        return FALSE;
    }

    //Copy description & truncate
    strncpy(trans->desc, desc, MAX_DESC_LENGTH);
    trans->desc[MAX_DESC_LENGTH] = '\0';
    
    trans->line_num = line_num;

    return TRUE;
}

/*
 *Insert transaction into list, maintaining timestamp order so list is always sorted
 *Exits on duplicate
 *
 */
static void SortedInsert(My402List *list, Txn *trans, const char *filename){
	My402ListElem *elem;

	for (elem = My402ListFirst(list); elem != NULL; elem = My402ListNext(list,elem)){
		Txn *curr = (Txn *)(elem->obj);

		if (curr->ts == trans->ts){
			fprintf(stderr, "error: line %d in '%s' has duplicate timestamp\n", trans->line_num, filename);
			free(trans);
			FreeTxns(list);
			exit(1);
		}

		if (curr->ts > trans->ts){
			if (!My402ListInsertBefore(list, trans, elem)){
				fprintf(stderr, "error: failed to insert transaction\n");
                free(trans);
                FreeTxns(list);
                exit(1);
			}
			return;
		}
	}

	//Append largest timestamp at end
	if (!My402ListAppend(list, trans)){
		fprintf(stderr, "error: failed to insert transaction\n");
        free(trans);
        FreeTxns(list);
        exit(1);
	}
}

static void FormatAmount(int cents, int is_negative, char *buf){
	int dollars = cents / 100;
	int remainder = cents % 100;

	//Overflow balance display
	if (cents >= MAX_AMOUNT * 100){
		if (is_negative){
			strcpy(buf, "(?,???,???" ".??" ")");
		} else {
			strcpy(buf, " ?,???,???" ".??" " ");
		}
		return;
	}

	//Format dollars.cents 
	char num_buf[20];
	sprintf(num_buf, "%d.%02d", dollars, remainder);

	//Format output to have a comma
	char format_buf[20];
	int dec_pos = (int)(strchr(num_buf, '.') - num_buf);
	int j = 0;

	for (int i = 0; i < dec_pos; i++){
		format_buf[j] = num_buf[i];
		j++;

		//Insert comma every 3 digits
		int remaining = dec_pos - i - 1;
		if (remaining > 0 && remaining % 3 == 0){
			format_buf[j] = ',';
			j++;
		}
	}

	//Copy decimal part
	strcpy(format_buf + j, num_buf + dec_pos);

	//Format with width and paranthesis for neg/pos
	if (is_negative){
		sprintf(buf, "(%12s)", format_buf);
	} else {
		sprintf(buf, "%13s ", format_buf);
	}
}

/*
 *Format date thats passed in as a cstring
 *PRE: the buf is of size buf[16] to fit with output format
 *ctime() formate is "Wed Jan 19 21:25:02 1998"
 *Function extracts day + mon (0-7), date (8-10), year (20-23) #'s are cstring array index'
 * add null terminator at the end
 *
 */
static void FormatDate(time_t ts, char *buf){
	char *time_str = ctime(&ts);

	strncpy(buf, time_str, 8);
	strncpy(buf + 8, time_str + 8, 3);
	strncpy(buf + 11, time_str + 20, 4);

	buf[15] = '\0';
}
/*
 * Print all Txns as formatted table
 * Each line follows spec formatting
 *
 */
static void PrintTransactions(My402List *list){
	My402ListElem *elem;
	long balance = 0;

	char *border = "+-----------------+--------------------------+----------------+----------------+";
	char *header = "|       Date      | Description              |         Amount |        Balance |";

	printf("%s\n", border);
    printf("%s\n", header);
    printf("%s\n", border);

    //Print each transaction row
    for (elem = My402ListFirst(list); elem != NULL; elem = My402ListNext(list, elem)){
    	Txn *trans = (Txn *)(elem->obj);

    	//Update running balance
    	if (trans->type == '+'){
    		balance += trans->cents;
    	} else {
    		balance -= trans->cents;
    	}

    	//Format Date
    	char date_buf[16];
    	FormatDate(trans->ts, date_buf);

    	//Format amount with 1/0 to track if withrawal or deposit
    	char amount_buf[16];
    	int is_negative = (trans->type == '-');
    	FormatAmount(trans->cents, is_negative, amount_buf);

    	//Format balance with 1/0 to track negative or positive
    	char balance_buf[16];
    	long abs_balance;
    	int is_negative_balance;

    	if (balance < 0){
    		abs_balance = -balance;
    		is_negative_balance = TRUE;
    	} else {
    		abs_balance = balance;
    		is_negative_balance = FALSE;
    	}
    	FormatAmount((int)abs_balance, is_negative_balance, balance_buf);

    	printf("| %s | %-24s | %14s | %14s |\n", date_buf, trans->desc, amount_buf, balance_buf);
    }

    printf("%s\n", border);
}

/*
 *Because we allocate memore for all Txns, if we run into an error or need to clear list,
 *we also need to free memory for Txn objects
 *
 *
 */
static void FreeTxns(My402List *list){
	My402ListElem *elem;

	for (elem = My402ListFirst(list); elem != NULL; elem = My402ListNext(list, elem)){
		free(elem->obj);
	}

	My402ListUnlinkAll(list);
}