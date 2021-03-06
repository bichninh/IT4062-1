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

typedef struct User_ {
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

typedef struct Session_ {
    User *user;
    enum Session_Status status;
    int sock;
    int login_failed_count;
} Session;

Session session[MAX_SESSION];
extern int session_count;


enum TypeUserStatus { //Status of system after client enter USER userid
    TU_SUCCESS,                     
    TU_EXISTED_USER,            
    TU_USER_BLOCKED,                
    TU_NOT_FOUND_USER                    
};

enum TypePassStatus {//after enter PASS pass
    TP_SUCCESS,                     
    TP_NOT_IDENTIFIED_USER,         
    TP_WRONG_PASS,                  
    TP_EXISTED_USER,            
    TP_USER_BLOCKED         
};

enum LogOutStatus {//after enter LOUT userid and pass
    LO_SUCCESS,                     
    LO_NOT_IDENTIFIED_USER,         
    LO_WRONG_USER,                  
    LO_WRONG_PASS
};

User *findUserByID (char *id);
void readUserFromFile (char *file_name);
void writeUserToFile (char *file_name);
int checkFullUser ();


int addSession (User *user, enum Session_Status status,int sock, int login_failed_count);
int removeSession (int sock);
int createSession (int sock);
enum TypeUserStatus typeUser (int sock, char *id);
enum TypePassStatus typePass (int sock, char *pass);
enum LogOutStatus logOut (int sock, char *id, char *pass);
Session *findSessionBySock (int sock);
void duplicateSession (Session *ss1, Session *ss2);
int checkFullSession ();

#endif
