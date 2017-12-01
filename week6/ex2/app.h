#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <string.h>
#include "user.h"
#include "session.h"

#define FOUND_USER_CODE "00"         
#define NOTFOUND_USER_CODE "01"       
#define USER_BLOCKED_CODE "31"       
#define MATCHED_PASS_CODE "10"         
#define NOTMATCHED_PASS_CODE "11"     
#define LOUT_SUCCESS_CODE "20"        
#define LOUT_FAIL_CODE "21"       

#define ID_LEN 30
#define PASS_LEN 30
#define OPCODE_LEN 4          // do dai lon nhat cua ma lenh
#define DATA_LEN 30         // max (ID_LEN, PASS_LEN)
#define MSG_CODE_LEN 2         // do dai lon nhat cua output code
#define OP_STR_LEN 1024       // do dai lon nhat cua output string

enum Opcode {
    USER,
    PASS,
    LOUT
};

struct Command_ {
    enum Opcode opcode;
    char id[ID_LEN + 1];
    char pass[PASS_LEN + 1];
};

typedef struct Command_ Command;

typedef struct Output_ {
    char msg_code[MSG_CODE_LEN + 1];
    char str[OP_STR_LEN + 1];
} Output;

int convertToMessage (Output *op, char *str, int str_size);
Output *runCommand (struct sockaddr_in *cliaddr, char *input_str);
Output *runUSER (struct sockaddr_in *cliaddr, char *id);
Output *runPASS (struct sockaddr_in *cliaddr, char *pass);
Output *runLOUT (struct sockaddr_in *cliaddr, char *id, char *pass);
Command *makeCommand (char *input_str);

#endif