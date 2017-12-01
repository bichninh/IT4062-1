#include "app.h"

Command *makeCommand (char *input_str) {
    char opcode[OPCODE_LEN + 1];
    char argv1[DATA_LEN + 1];
    char argv2[PASS_LEN + 1];

    Command *cmd = (Command *) malloc (sizeof(Command));

    sscanf(input_str, "%s %s %s", opcode, argv1, argv2);

    if (strcmp(opcode, "USER") == 0) {
        cmd->opcode = USER;
        strncpy(cmd->id, argv1, ID_LEN);
        return cmd;
    } 

    else if (strcmp(opcode, "PASS") == 0) {
        cmd->opcode = PASS;
        strncpy(cmd->pass, argv1, PASS_LEN);
        return cmd;
    } 

    else if (strcmp(opcode, "LOUT") == 0) {
        cmd->opcode = LOUT;
        strncpy(cmd->id, argv1, ID_LEN);
        strncpy(cmd->pass, argv2, PASS_LEN);
        return cmd;
    } 
        else{
            return NULL;
    }
}


Output *runCommand (struct sockaddr_in *cliaddr, char *input_str) {
    Command *cmd = makeCommand(input_str);
	if (cmd == NULL) return;

	switch (cmd->opcode) {
    case USER:
        return runUSER(cliaddr, cmd->id);
    case PASS:
        return runPASS(cliaddr, cmd->pass);
    case LOUT:
        return runLOUT(cliaddr, cmd->id, cmd->pass);
    default:
        return NULL;
    }
}

Output *runUSER (struct sockaddr_in *cliaddr, char *id) {
    Output *op = (Output *) malloc(sizeof(Output));

    enum LoginUserStatus login_user_status = loginUser(cliaddr, id);
    
    switch (login_user_status) {
    case LU_SUCCESS:
        strcpy(op->msg_code, FOUND_USER_CODE);
        strcpy(op->str, "Add user successful! Use 'PASS password' to login.");
        break;
    case LU_EXISTED_ONE_USER:
        strcpy(op->msg_code, NOTFOUND_USER_CODE);
        strcpy(op->str, "Add user fail! Existed an user in session.");
        break;
    case LU_USER_BLOCKED:
        strcpy(op->msg_code, USER_BLOCKED_CODE);
        strcpy(op->str, "Add user fail! This user is blocked.");
        break;
    case LU_NOT_FOUND:
        strcpy(op->msg_code, NOTFOUND_USER_CODE);
        strcpy(op->str, "Add user fail! User not found.");
        break;
    default:
        strcpy(op->msg_code, NOTFOUND_USER_CODE);
        strcpy(op->str, "Add user fail! Something is wrong.");
    }

    return op;
}

Output *runPASS (struct sockaddr_in *cliaddr, char *pass) {
    Output *op = (Output *) malloc(sizeof(Output));

    enum LoginPassStatus login_pass_status = loginPass(cliaddr, pass);

    switch (login_pass_status) {
    case LP_SUCCESS:
        strcpy(op->msg_code, MATCHED_PASS_CODE);
        strcpy(op->str, "Login successful! Use 'LOUT userid' to logout.");
        break;
    case LP_NOT_IDENTIFIED_USER:
        strcpy(op->msg_code, NOTMATCHED_PASS_CODE);
        strcpy(op->str, "Login fail! Don't exist any user in session, use 'USER useid' to add an user.");
        break;
    case LP_WRONG_PASS:
        strcpy(op->msg_code, NOTMATCHED_PASS_CODE);
        strcpy(op->str, "Login fail! Password doesn't match, try again.");
        break;
    case LP_EXISTED_ONE_USER:
        strcpy(op->msg_code, NOTMATCHED_PASS_CODE);
        strcpy(op->str, "Login fail! Existed an user in session.");
        break;
    case LP_USER_BLOCKED:
        strcpy(op->msg_code, USER_BLOCKED_CODE);
        strcpy(op->str, "Login fail too much! User was blocked and removed from session. Use 'USER userid' to add an other user.");
        break;
    default:
        strcpy(op->msg_code, NOTMATCHED_PASS_CODE);
        strcpy(op->str, "Add user fail! Something is wrong.");
    }

    return op;
}

Output *runLOUT (struct sockaddr_in *cliaddr, char *id, char *pass) {
    Output *op = (Output *) malloc(sizeof(Output));
    
    enum LogoutStatus logout_status = logout(cliaddr, id, pass);

    switch (logout_status) {
    case LO_SUCCESS: 
        strcpy(op->msg_code, LOUT_SUCCESS_CODE);
        strcpy(op->str, "Logout successful! Use 'USER userid' to add an user.");
        break;
    case LO_NOT_IDENTIFIED_USER: 
        strcpy(op->msg_code, LOUT_FAIL_CODE);
        strcpy(op->str, "Logout fail! Don't exist any user in session, use 'USER userid' to add an user.");
        break;
    case LO_WRONG_USER: 
        strcpy(op->msg_code, LOUT_FAIL_CODE);
        strcpy(op->str, "Logout fail! UserID doesn't match, try again.");
        break;
    case LO_WRONG_PASS: 
        strcpy(op->msg_code, LOUT_FAIL_CODE);
        strcpy(op->str, "Logout fail! Wrong password, try again.");
        // Wrong password -> BLOCK???
        break;
    default:
        strcpy(op->msg_code, LOUT_FAIL_CODE);
        strcpy(op->str, "Add user fail! Something is wrong.");
    }

    return op;
}

int convertToMessage (Output *op, char *str, int str_size) {
    bzero(str, str_size);
    if (op != NULL) {
        strcpy(str, op->msg_code);
        strcat(str, ": ");
        strncat(str, op->str, str_size - strlen(op->msg_code) - 2);
        free(op);  
    }
     else {
        strcpy(str, "41: Invalid command!");
    }
    return (strlen(str) + 1);
}

