/* Example Echo Server taken from github
 * Written by Mathias Buus (mafintosh)
 * No Plagiarism is intended by the use of this simple TCP echo server, 
 * it is simply a starting point & reference for a larger project
 */

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

#define BUFFER_SIZE 1024
#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }

int main () {

  //Open for lab1 machines
  int port_number = 2112;

  //File descriptors for server and client for multi-threading.
  int server_fd, client_fd, err;
  
  struct sockaddr_in server, client;
  
  char buf[BUFFER_SIZE];

  //Use socket type AF_INET (for IPv4)
  //Use socket type SOCK_Stream (for TCP)
  //Third argument is 0 in order to use IP.
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  
  //Check for failure
  if (server_fd < 0) 
  	cout << "Could not create socket\n";

  server.sin_family = AF_INET; // Server uses IPv4
  server.sin_port = htons(port_number);  //Convert port number to byte order used by network
  server.sin_addr.s_addr = htonl(INADDR_ANY); //Accept any IP address

  int opt_val = 1;
  //Set options. Could be useful. Check these out.
  setsockopt(	server_fd,  	//applying choices to server
  		SOL_SOCKET, 	//set socket level options
  		SO_REUSEADDR, 	//Bind will reuse local addresses
  		&opt_val, 	//Returns an int indicating the options chosen.
  		sizeof opt_val);//In order to save the int correctly

  //Binds the address in 'sockaddr' to the file descriptor for server
  if ( bind(server_fd, (struct sockaddr *) &server, sizeof(server)) < 0)
  	cout << "Could not bind socket\n";

  //Tells server to start listening
  if ( listen(server_fd, 128) )
  	cout << "Server failed to listen socket\n";

  printf("Server is listening on %d\n", port_number);

  while (1) {
  
    //Size of client struct
    socklen_t client_len = sizeof(client);
    
    //Assign file descriptor to accepted client.
    client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);

    if (client_fd < 0) 
    	cout << "Could not establish new connection\n";

    while (1) {
    
      //Recieves message, stores it in buffer.
      int read = recv(client_fd, buf, BUFFER_SIZE, 0);

      if (!read) break; // done reading
      if (read < 0)
      	cout << "Receive from client failed\n";

	//Send the buffer contents back. 
      if (send(client_fd, buf, read, 0) < 0) 
      	cout << "Send to client failed\n";
    }
  }

  return 0;
}
