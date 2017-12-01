struct Session{
	msg_type type;
	struct User user;
	struct sockaddr_in cliaddr;
};
struct User{
	char id[30];
	char pass[30];
	int status;
};
enum msg_type{
	FOUND_USER = 00,
	NOTFOUND_USER = 01,
	MATCH_PASS = 10,
	NOTMATCH_PASS = 11,
	BLOCKED = 31,
	LOGOUT_SUCCESS = 20,
	LOGOUT_FAIL = 21
};
