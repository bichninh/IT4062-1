#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>

#define MAX_USER 100
#define DATA_LEN 30

#define MAX_SESSION 100
#define MAX_LOGIN_FAILED 5

enum UserStatus { 
    BLOCKED,
    ACTIVE
};

typedef struct User_ {//user
    char id[DATA_LEN + 1];
    char pass[DATA_LEN + 1];
    enum UserStatus status;
} User;

User user[MAX_USER];
extern int user_count;

enum Session_Status {
    NOT_IDENTIFIED_USER,    
    NOT_AUTHENTICATED,      
    AUTHENTICATED,          
    USER_BLOCKED            
};

typedef struct Session_ {//session
    User *user;
    enum Session_Status status;
    struct sockaddr_in *cliaddr;
    int login_failed_count;
} Session;

Session session[MAX_SESSION];
extern int session_count;

//use to handle and send suitable mess to client
enum TypeUserStatus { //Status of system after client enter USER userid
    TU_SUCCESS,                     //found userID in file and sess has no user before
    TU_EXISTED_USER,            //existed an user in sess before
    TU_USER_BLOCKED,                //user blocked
    TU_NOT_FOUND_USER                    //not found userID in file
};

enum TypePassStatus {//after enter PASS pass
    TP_SUCCESS,                     //type suitable pass of userid
    TP_NOT_IDENTIFIED_USER,         //sess has no user
    TP_WRONG_PASS,                  //wrong pass
    TP_EXISTED_USER,            //sess has an user
    TP_USER_BLOCKED         //user is blocked
};

enum LogOutStatus {//after enter LOUT userid and pass
    LO_SUCCESS,                     //true user and its pass
    LO_NOT_IDENTIFIED_USER,         //
    LO_WRONG_USER,                  
    LO_WRONG_PASS
};

//some func()
User *findUserByID (char *id);
void readUserFromFile (char *file_name);
void writeUserToFIle (char *file_name);
int checkFullUser ();

enum TypeUserStatus typeUser (struct sockaddr_in *cliaddr, char *id);
enum TypePassStatus typePass (struct sockaddr_in *cliaddr, char *pass);
enum LogOutStatus logOut (struct sockaddr_in *cliaddr, char *id, char *pass);
int removeSession (struct sockaddr_in *cliaddr);
int addSession (User *user, enum Session_Status status, struct sockaddr_in *cliaddr, int login_failed_count);
int createSession (struct sockaddr_in *cliaddr);
Session *findSessionByCliaddr (struct sockaddr_in *cliaddr);
void duplicateSession (Session *ss1, Session *ss2);
int checkFullSession ();

#endif
