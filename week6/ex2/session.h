#ifndef SESSION_H
#define SESSION_H

#include <netinet/in.h>
#include "user.h"

#define MAX_SESSION 100
#define MAX_LOGIN_FAILED 5

enum SessionStatus {
    NOT_IDENTIFIED_USER,    // Chua xac dinh duoc user
    NOT_AUTHENTICATED,      // Da xac dinh user, mat khau chua dung
    AUTHENTICATED,          // Xac thuc thanh cong
    USER_BLOCKED            // Tai khoan bi khoa
};

typedef struct Session_ {
    User *user;
    enum SessionStatus status;
    struct sockaddr_in *cliaddr;
    int login_failed_count;
} Session;

Session session[MAX_SESSION];
extern int session_count;


enum FunctionStatus {
    FN_SUCCESS,
    FN_ERROR
};

enum LoginUserStatus {
    LU_SUCCESS,                     // Xac dinh user thanh cong
    LU_EXISTED_ONE_USER,            // Da co user trong session
    LU_USER_BLOCKED,                // User nay da bi block
    LU_NOT_FOUND                    // Khong ton tai user nay
};

enum LoginPassStatus {
    LP_SUCCESS,                     // Dang nhap thanh cong
    LP_NOT_IDENTIFIED_USER,         // Chua xac dinh user
    LP_WRONG_PASS,                  // Sai password
    LP_EXISTED_ONE_USER,            // Da co user trong session
    LP_USER_BLOCKED                 // Block user
};

enum LogoutStatus {
    LO_SUCCESS,                     // Dang xuat thanh cong
    LO_NOT_IDENTIFIED_USER,         // Chua xac dinh user
    LO_WRONG_USER,                  // Sai user
    LO_WRONG_PASS                   // Sai password
};

enum LogoutStatus logout (struct sockaddr_in *cliaddr, char *id, char *pass);
enum LoginPassStatus loginPass (struct sockaddr_in *cliaddr, char *pass);
enum LoginUserStatus loginUser (struct sockaddr_in *cliaddr, char *id);
enum SessionStatus sessionStatus (struct sockaddr_in *cliaddr);
_Bool removeSession (struct sockaddr_in *cliaddr);
_Bool addSession (User *user, enum SessionStatus status, struct sockaddr_in *cliaddr, int login_failed_count);
_Bool newSession (struct sockaddr_in *cliaddr);
Session *findBySessionCliaddr (struct sockaddr_in *cliaddr);
int indexOfSession (struct sockaddr_in *cliaddr);
void copySession (Session *src, Session *dest);
_Bool isFullNoSession ();
_Bool isEqualSockAddrIn (struct sockaddr_in *addr1, struct sockaddr_in *addr2);

#endif
