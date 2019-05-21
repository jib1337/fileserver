/* File server application | Jack Nelson | CSP2308
 * networking.h
 * networking.c header file */

void connectionSignalShutdown();
void threadExit();
void serverStart(config_t* Config);
void* connectionHandler(void* data);
