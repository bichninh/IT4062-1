#ifndef USER_H
#define USER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_USER 100
#define USER_ID_LEN 30
#define PASS_LEN 30

enum UserStatus { 
    BLOCKED,
    ACTIVE
};

typedef struct User_ {
    char id[USER_ID_LEN + 1];
    char pass[PASS_LEN + 1];
    enum UserStatus status;
} User;

User user[MAX_USER];
extern int user_count;



User *findByUserId (char *id);
int indexOfUser (char *id);
void writeUser (char *file_name);
void readUser (char *file_name);
_Bool isFullNoUser ();
FILE *openFile (char *file_name, char *mode);

#endif
