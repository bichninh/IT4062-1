#include "run.h"

Command *makeCommand (char *input_str) {
    char opcode[OPCODE_LEN + 1];
    char argv1[DATA_LEN + 1];
    char argv2[DATA_LEN + 1];

    Command *cmd = (Command *) malloc (sizeof(Command));

    sscanf(input_str, "%s %s %s", opcode, argv1, argv2);

    if (strcmp(opcode, "USER") == 0) {
        cmd->opcode = USER;
        strncpy(cmd->id, argv1, DATA_LEN);
        return cmd;
    } 

    else if (strcmp(opcode, "PASS") == 0) {
        cmd->opcode = PASS;
        strncpy(cmd->pass, argv1, DATA_LEN);
        return cmd;
    } 

    else if (strcmp(opcode, "LOUT") == 0) {
        cmd->opcode = LOUT;
        strncpy(cmd->id, argv1, DATA_LEN);
        strncpy(cmd->pass, argv2, DATA_LEN);
        return cmd;
    } 
        else{
            return NULL;
    }
}


Message *runCommand (struct sockaddr_in *cliaddr, char *input_str) {
    Command *cmd = makeCommand(input_str);
	if (cmd == NULL) return;
    
    if(cmd->opcode == USER) return runUSER(cliaddr,cmd->id);
    else if(cmd->opcode == PASS) return runPASS(cliaddr,cmd->pass);
    else if(cmd->opcode == LOUT) return runLOUT(cliaddr,cmd->id,cmd->pass);
    else return NULL;
}

Message *runUSER (struct sockaddr_in *cliaddr, char *id) {
    Message *msg = (Message *) malloc(sizeof(Message));

    enum TypeUserStatus typeUser_status = typeUser(cliaddr, id);
    
    switch (typeUser_status) {
    case TU_SUCCESS:
        strcpy(msg->msg_code, FOUND_USER_CODE);
        strcpy(msg->str, "Valid user id! Type PASS password to login.");
        break;
    case TU_EXISTED_USER:
        strcpy(msg->msg_code, NOTFOUND_USER_CODE);
        strcpy(msg->str, "Invalid user id! Existed an user in session.");
        break;
    case TU_USER_BLOCKED:
        strcpy(msg->msg_code, USER_BLOCKED_CODE);
        strcpy(msg->str, "Invalid user id! User is blocked.");
        break;
    case TU_NOT_FOUND_USER:
        strcpy(msg->msg_code, NOTFOUND_USER_CODE);
        strcpy(msg->str, "Invalid user id! User not found.");
        break;
    default:
        strcpy(msg->msg_code, NOTFOUND_USER_CODE);
        strcpy(msg->str, "Invalid user id!");
    }

    return msg;
}

Message *runPASS (struct sockaddr_in *cliaddr, char *pass) {
    Message *msg = (Message *) malloc(sizeof(Message));

    enum TypePassStatus typePass_status = typePass(cliaddr, pass);

    switch (typePass_status) {
    case TP_SUCCESS:
        strcpy(msg->msg_code, MATCHED_PASS_CODE);
        strcpy(msg->str, "Login success! Type LOUT userID to log out.");
        break;
    case TP_NOT_IDENTIFIED_USER:
        strcpy(msg->msg_code, NOTMATCHED_PASS_CODE);
        strcpy(msg->str, "Login fail! User not existed!");
        break;
    case TP_WRONG_PASS:
        strcpy(msg->msg_code, NOTMATCHED_PASS_CODE);
        strcpy(msg->str, "Login fail! Password doesn't match!");
        break;
    case TP_EXISTED_USER:
        strcpy(msg->msg_code, NOTMATCHED_PASS_CODE);
        strcpy(msg->str, "Login fail! Existed an user in session.");
        break;
    case TP_USER_BLOCKED:
        strcpy(msg->msg_code, USER_BLOCKED_CODE);
        strcpy(msg->str, "Login fail > 5 times! User was blocked.");
        break;
    default:
        strcpy(msg->msg_code, NOTMATCHED_PASS_CODE);
        strcpy(msg->str, "Login fail!");
    }

    return msg;
}

Message *runLOUT (struct sockaddr_in *cliaddr, char *id, char *pass) {
    Message *msg = (Message *) malloc(sizeof(Message));
    
    enum LogOutStatus logOut_status = logOut(cliaddr, id, pass);

    switch (logOut_status) {
    case LO_SUCCESS: 
        strcpy(msg->msg_code, LOUT_SUCCESS_CODE);
        strcpy(msg->str, "Logout successful!");
        break;
    case LO_NOT_IDENTIFIED_USER: 
        strcpy(msg->msg_code, LOUT_FAIL_CODE);
        strcpy(msg->str, "Logout fail! User not existed!");
        break;
    case LO_WRONG_USER: 
        strcpy(msg->msg_code, LOUT_FAIL_CODE);
        strcpy(msg->str, "Logout fail! UserID doesn't match.");
        break;
    case LO_WRONG_PASS: 
        strcpy(msg->msg_code, LOUT_FAIL_CODE);
        strcpy(msg->str, "Logout fail! Password is wrong!");
        break;
    default:
        strcpy(msg->msg_code, LOUT_FAIL_CODE);
        strcpy(msg->str, "Logout fail!");
    }

    return msg;
}

int changeToFullMessage(Message *msg, char *str, int str_size) {
    bzero(str, str_size);
    if (msg != NULL) {
        strcpy(str, msg->msg_code);
        strcat(str, ": ");
        strncat(str, msg->str, str_size - strlen(msg->msg_code) - 2);
        free(msg);  
    }
     else {
        strcpy(str, "Invalid command!");
    }
    return (strlen(str) + 1);
}

