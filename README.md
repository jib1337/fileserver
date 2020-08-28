# TCP File Server

File Server and Client applications, intended for use by a single user over multiple devices.
Warning: Data is unencrypted in transmission. Passwords are stored securely by the server.

-------------------------------------------------------------------------------------------------------------------------------------------------

## Installing dependancies
In order to compile the binaries, the OpenSSL libraries must be installed on the system. To do this, simply input the following command:
```
sudo apt-get install libssl-dev
```

## Server
1. Build by running the makefile.
2. Run the program - choose a username and password.
3. Review settings and configuration, then choose to run the server when satisfied.

## Client
1. Build by running the makefile.
2. Review configuration - remember to update the username in the config with the chosen server username.
3. When satisfied, connect to the server and authenticate with the server's password.
4. Follow the prompts to upload and download files.
  
