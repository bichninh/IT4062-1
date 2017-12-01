#ifndef RUN_H
#define RUN_H

#include <stdio.h>
#include <string.h>
#include "data.h"
//we can use enum
#define FOUND_USER_CODE "00"         
#define NOTFOUND_USER_CODE "01"       
#define USER_BLOCKED_CODE "31"       
#define MATCHED_PASS_CODE "10"         
#define NOTMATCHED_PASS_CODE "11"     
#define LOUT_SUCCESS_CODE "20"        
#define LOUT_FAIL_CODE "21"       
//some MAX_LEN
#define OPCODE_LEN 4          
#define DATA_LEN 30         
#define MSG_CODE_LEN 2        
#define MSG_STR_LEN 1024       

enum Opcode {
    USER,
    PASS,
    LOUT
};

struct Command_ { //command
    enum Opcode opcode;
    char id[DATA_LEN + 1];
    char pass[DATA_LEN + 1];
};

typedef struct Command_ Command;

typedef struct Message_ { //Message
    char msg_code[MSG_CODE_LEN + 1];
    char str[MSG_STR_LEN + 1];
} Message;

//some func
Command *makeCommand (char *input_str);
Message *runCommand (int sock, char *input_str);
Message *runUSER (int sock, char *id);
Message *runPASS (int sock, char *pass);
Message *runLOUT (int sock, char *id, char *pass);
int changeToFullMessage(Message *msg, char *str, int str_size);

#endif