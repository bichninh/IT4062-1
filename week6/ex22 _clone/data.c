#include "data.h"

User *findUserByID (char *id) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user[i].id, id) == 0)
            return (user + i);
    }
    return NULL;
}


void readUserFromFile (char *file_name) {
    FILE *f = fopen(file_name, "r");
    if (f == NULL) {
        printf("ERROR: Can't open file!");
        return;
    }

    while (checkFullUser() == 0) {
        if (feof(f)) break;
        fscanf(f, "%s %s %d", user[user_count].id, user[user_count].pass, &(user[user_count].status));
        user_count++;
    }
    fclose(f);
}


void writeUserToFile (char *file_name) {
    FILE *f = fopen(file_name, "w");
     if (f == NULL) {
        printf("ERROR: Can't open file!");
        return;
    }

    for (int i = 0; i < user_count; i++) {
        fprintf(f, "%s %s %d\n", user[i].id, user[i].pass, user[i].status);
    }
    fclose(f);
}


int checkFullUser () {
    if (user_count > MAX_USER) {
        printf("Full users!\n");
        return 1;
    }
    return 0;
}


enum TypeUserStatus typeUser (int sock, char *id) {
    Session *sess = findSessionBySock(sock);
    if (sess == NULL || sess->status == USER_BLOCKED) return -1;
    else if (sess->status == NOT_AUTHENTICATED || sess->status == AUTHENTICATED) 
        return TU_EXISTED_USER;

    User *user = findUserByID(id);
    if (user == NULL) return TU_NOT_FOUND_USER;
    else if (user->status == BLOCKED) return TU_USER_BLOCKED;
    else {
        sess->user = user;
        sess->status = NOT_AUTHENTICATED;
        return TU_SUCCESS;
    }
}


enum TypePassStatus typePass (int sock, char *pass) {
    Session *sess = findSessionBySock(sock);
    if (sess == NULL) return -1;
    else if (sess->status == NOT_IDENTIFIED_USER) return TP_NOT_IDENTIFIED_USER;
    else if (sess->status == AUTHENTICATED) return TP_EXISTED_USER;

    if (strcmp(sess->user->pass, pass) == 0) {   
        sess->status = AUTHENTICATED;
        sess->login_failed_count = 0;
        return TP_SUCCESS;
    } 
    else {    
        sess->login_failed_count++;
        if (sess->login_failed_count >= MAX_LOGIN_FAILED) {    //blocked
            sess->user->status = BLOCKED;
            sess->user = NULL;
            sess->status = NOT_IDENTIFIED_USER;
            sess->login_failed_count = 0;
            return TP_USER_BLOCKED;
        } 
        else {
            return TP_WRONG_PASS;
        }
    }
}


enum LogOutStatus logOut (int sock, char *id, char *pass) {
    Session *sess = findSessionBySock(sock);
    if (sess == NULL) return -1;
    else if (sess->status == NOT_IDENTIFIED_USER) return LO_NOT_IDENTIFIED_USER;

    User *user = findUserByID(id);
  
    if (user == NULL || user != sess->user) return LO_WRONG_USER;

    if (sess->status == NOT_AUTHENTICATED || strcmp(user->pass, pass) == 0) {      
        sess->user = NULL;
        sess->status = NOT_IDENTIFIED_USER;
        sess->login_failed_count = 0;
        return LO_SUCCESS;
    } 
    else{  
        return LO_WRONG_PASS;
    }
}


int createSession (int sock) {
    if (checkFullSession() != 0) return 0;
    
    session[session_count].user = NULL;
    session[session_count].status = NOT_IDENTIFIED_USER;
    session[session_count].sock = sock;
    session[session_count].login_failed_count = 0;
    session_count++;
    
    return 1;
}


int addSession (User *user, enum Session_Status status,int sock, int login_failed_count) {
    if (checkFullSession() != 0) return 0;
    
    session[session_count].user = user;
    session[session_count].status = status;
    session[session_count].sock = sock;
    session[session_count].login_failed_count = login_failed_count;
    session_count++;

    return 1;
}


int removeSession (int sock) {
    int check = findSessionBySock(sock);

    if (check == -1) return 0;

    for (int i = check; i < session_count - 1; i++) {
        duplicateSession(session + i, session + i + 1);
    }
    session_count--;

    return 1;
}



Session *findSessionBySock (int sock) {
    for (int i = 0; i < session_count; i++) {
        if(session[i].sock == sock)
            return (session + i);
    }
    return NULL;
}


void duplicateSession (Session *ss1, Session *ss2) {
    ss2->user = ss1->user;
    ss2->status = ss1->status;
    ss2->sock = ss1->sock;
    ss2->login_failed_count = ss1->login_failed_count;
}


int checkFullSession() {
    if (session_count > MAX_SESSION) {
        printf("Sessions is full!\n");
        return 1;
    }
    return 0;
}

