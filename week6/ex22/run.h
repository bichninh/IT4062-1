#ifndef RUN_H
#define RUN_H

#include <stdio.h>
#include <string.h>
#include "data.h"

#define FOUND_USER_CODE "00"         
#define NOTFOUND_USER_CODE "01"       
#define USER_BLOCKED_CODE "31"       
#define MATCHED_PASS_CODE "10"         
#define NOTMATCHED_PASS_CODE "11"     
#define LOUT_SUCCESS_CODE "20"        
#define LOUT_FAIL_CODE "21"       

#define OPCODE_LEN 4          
#define DATA_LEN 30         
#define MSG_CODE_LEN 2        
#define MSG_STR_LEN 1024       

enum Opcode {
    USER,
    PASS,
    LOUT
};

struct Command_ {
    enum Opcode opcode;
    char id[DATA_LEN + 1];
    char pass[DATA_LEN + 1];
};

typedef struct Command_ Command;

typedef struct Message_ {
    char msg_code[MSG_CODE_LEN + 1];
    char str[MSG_STR_LEN + 1];
} Message;

int changeToFullMessage(Message *msg, char *str, int str_size);
Message *runCommand (struct sockaddr_in *cliaddr, char *input_str);
Message *runUSER (struct sockaddr_in *cliaddr, char *id);
Message *runPASS (struct sockaddr_in *cliaddr, char *pass);
Message *runLOUT (struct sockaddr_in *cliaddr, char *id, char *pass);
Command *makeCommand (char *input_str);

#endif