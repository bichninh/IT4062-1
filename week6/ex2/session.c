#include "session.h"

enum LoginUserStatus loginUser (struct sockaddr_in *cliaddr, char *id) {
    Session *ss = findBySessionCliaddr(cliaddr);
    if (ss == NULL || ss->status == USER_BLOCKED) return -1;
    else if (ss->status == NOT_AUTHENTICATED || ss->status == AUTHENTICATED) return LU_EXISTED_ONE_USER;

    User *us = findByUserId(id);
    if (us == NULL) return LU_NOT_FOUND;
    else if (us->status == BLOCKED) return LU_USER_BLOCKED;
    else {
        ss->user = us;
        ss->status = NOT_AUTHENTICATED;

        return LU_SUCCESS;
    }
}


enum LoginPassStatus loginPass (struct sockaddr_in *cliaddr, char *pass) {
    Session *ss = findBySessionCliaddr(cliaddr);
    if (ss == NULL) return -1;
    else if (ss->status == NOT_IDENTIFIED_USER) return LP_NOT_IDENTIFIED_USER;
    else if (ss->status == AUTHENTICATED) return LP_EXISTED_ONE_USER;

    // Da dang ki nguoi dung
    if (!strcmp(ss->user->pass, pass)) {    // Login success
        ss->status = AUTHENTICATED;
        ss->login_failed_count = 0;

        return LP_SUCCESS;
    } else {    // Login fail
        ss->login_failed_count++;

        if (ss->login_failed_count >= MAX_LOGIN_FAILED) {    // Login fail too much
            // block user
            ss->user->status = BLOCKED;

            // remove user from session
            ss->user = NULL;
            ss->status = NOT_IDENTIFIED_USER;
            ss->login_failed_count = 0;

            return LP_USER_BLOCKED;
        } else {
            return LP_WRONG_PASS;
        }
    }
}



enum LogoutStatus logout (struct sockaddr_in *cliaddr, char *id, char *pass) {
    Session *ss = findBySessionCliaddr(cliaddr);
    if (ss == NULL) return -1;
    else if (ss->status == NOT_IDENTIFIED_USER) return LO_NOT_IDENTIFIED_USER;

    User *us = findByUserId(id);
    // id khong ton tai hoac sai nguoi dung
    if (us == NULL || us != ss->user) return LO_WRONG_USER;

    // Dung nguoi dung da dang nhap
    if (ss->status == NOT_AUTHENTICATED     // Da ton tai user nhung chua dang nhap
        || !strcmp(us->pass, pass)) {       // Dung password -> Dang xuat thanh cong
        ss->user = NULL;
        ss->status = NOT_IDENTIFIED_USER;
        ss->login_failed_count = 0;
        
        return LO_SUCCESS;
    } else {    // Sai password
        return LO_WRONG_PASS;
    }
}



enum SessionStatus sessionStatus (struct sockaddr_in *cliaddr) {
    Session *ss = findBySessionCliaddr(cliaddr);
    if (ss == NULL) return -1;

    return ss->status;
}

_Bool removeSession (struct sockaddr_in *cliaddr) {
    int target = findBySessionCliaddr(cliaddr);

    if (target == -1) return FN_ERROR;

    for (int i = target; i < session_count - 1; i++) {
        copySession(session + i, session + i + 1);
    }
    session_count--;

    return FN_SUCCESS;
}

_Bool addSession (User *user, enum SessionStatus status, struct sockaddr_in *cliaddr, int login_failed_count) {
    if (isFullNoSession()) return FN_ERROR;
    
    session[session_count].user = user;
    session[session_count].status = status;
    session[session_count].cliaddr = cliaddr;
    session[session_count].login_failed_count = login_failed_count;

    session_count++;

    return FN_SUCCESS;
}

_Bool newSession (struct sockaddr_in *cliaddr) {
    if (isFullNoSession()) return FN_ERROR;
    
    session[session_count].user = NULL;
    session[session_count].status = NOT_IDENTIFIED_USER;
    session[session_count].cliaddr = cliaddr;
    session[session_count].login_failed_count = 0;
    
    session_count++;
    
    return FN_SUCCESS;
}

Session *findBySessionCliaddr (struct sockaddr_in *cliaddr) {
    for (int i = 0; i < session_count; i++) {
        if (isEqualSockAddrIn(session[i].cliaddr, cliaddr))
            return (session + i);
    }

    return NULL;
}

int indexOfSession (struct sockaddr_in *cliaddr) {
    for (int i = 0; i < session_count; i++) {
        if (isEqualSockAddrIn(session[i].cliaddr, cliaddr))
            return i;
    }

    return -1;
}

void copySession (Session *src, Session *dest) {
    dest->user = src->user;
    dest->status = src->status;
    dest->cliaddr = src->cliaddr;
    dest->login_failed_count = src->login_failed_count;
}

_Bool isFullNoSession () {
    if (session_count > MAX_SESSION) {
        printf("Number of sessions is full!\n");
        return 1;
    }

    return 0;
}

_Bool isEqualSockAddrIn (struct sockaddr_in *addr1, struct sockaddr_in *addr2) {
    return (addr1->sin_addr.s_addr == addr2->sin_addr.s_addr && addr1->sin_port == addr2->sin_port);
}
