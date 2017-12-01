#include "user.h"

User *findByUserId (char *id) {
    for (int i = 0; i < user_count; i++) {
        if (!strcmp(user[i].id, id))
            return (user + i);
    }

    return NULL;
}

int indexOfUser (char *id) {
    for (int i = 0; i < user_count; i++) {
        if (!strcmp(user[i].id, id))
            return i;
    }

    return -1;
}

void writeUser (char *file_name) {
    FILE *f = openFile(file_name, "w");

    for (int i = 0; i < user_count; i++) {
        fprintf(f, "%s %s %d\n", user[i].id, user[i].pass, user[i].status);
    }

    fclose(f);
}

void readUser (char *file_name) {
    FILE *f = openFile(file_name, "r");

    while (!isFullNoUser()) {
        if (feof(f)) break; // Ket thuc file

        fscanf(f, "%s %s %d", user[user_count].id, user[user_count].pass, &(user[user_count].status));

        // Bo qua dong trong cuoi cung neu co
        if (user[user_count].id[0] == '\0' || user[user_count].pass[0] == '\0') break;
        
        user_count++;
    }

    fclose(f);
}

_Bool isFullNoUser () {
    if (user_count > MAX_USER) {
        printf("Number of users is full!\n");
        return 1;
    }

    return 0;
}

FILE *openFile (char *file_name, char *mode) {
    FILE *f = fopen(file_name, mode);

    if (f == NULL) {
        printf("ERROR: Can't open file %s!\n", file_name);
        exit(EXIT_FAILURE);
    }

    return f;
}
