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

#include "my402list.h"
#include "cs402.h"
#include "my402list.h"

#define MAX_LINE 2048
#define TEN_MILLION_CENTS (10000000LL * 100LL)

typedef struct tagTxn {
    char type;              
    time_t ts;              
    long long cents; 
    char desc[1024];        
} Txn;



