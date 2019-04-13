int authenticate(char* credentials);
int checkPassword(char* password, char* currentPasswordHash);
void genHash(char* password, unsigned long length, char* hashString);
void setCredentials(config_t* Config);

