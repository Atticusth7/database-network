#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "msg.h"

#define BUF 256

void Usage(char *progname);

int LookupName(char *name,
                unsigned short port,
                struct sockaddr_storage *ret_addr,
                size_t *ret_addrlen);

int Connect(const struct sockaddr_storage *addr,
             const size_t addrlen,
             int *ret_fd);

int 
main(int argc, char **argv) {
  if (argc != 3) {
    Usage(argv[0]);
  }

  unsigned short port = 0;
  if (sscanf(argv[2], "%hu", &port) != 1) {
    Usage(argv[0]);
  }

  // Get an appropriate sockaddr structure.
  struct sockaddr_storage addr;
  size_t addrlen;
  if (!LookupName(argv[1], port, &addr, &addrlen)) {
    Usage(argv[0]);
  }

  // Connect to the remote host.
  int socket_fd;
  printf("%d", socket_fd);
  if (!Connect(&addr, addrlen, &socket_fd)) {
    Usage(argv[0]);
  }

  // Read something from the remote host.
  // Will only read BUF-1 characters at most.
  int choice;
  char readbuf[128];
  
  printf("%d", socket_fd);

  // Write something to the remote host.
 while(1){
 printf("Enter your choice (1 to put, 2 to get, 3 to delete, 0 to quit): ");
 choice = 0;
 scanf("%d",&choice ); 
 struct msg m1;
 if(choice == 0)
 {
  break;
 }
 else if (choice == 1)
 {
   char name1[128];
   char filler[1];
   uint32_t tid;
   m1.type = 1;
   printf("Enter the name: ");
   fgets(filler, 8, stdin);
   fgets(name1, 128, stdin);
   if(strcmp( name1, "\n") == 0)
   {
   printf("put failed \n");
   close(socket_fd);
   return EXIT_FAILURE;
   }
   name1[strlen(name1) - 1] = 0;
   strncpy(m1.rd.name, name1,sizeof(name1));
   printf("Enter the id: ");
   scanf("%u",&tid);
   m1.rd.id = tid; 
  }
  else if(choice == 2)
  {
  uint32_t tid;
  m1.type = 2;
  printf("Enter the id: ");
  scanf("%u",&tid);
  m1.rd.id = tid;
  }
  else if(choice == 3)
  { 
  uint32_t tid;
  m1.type = 3;
  printf("Enter the id: ");
  scanf("%u",&tid);
  m1.rd.id = tid;
  }
  while (1) 
  {
    int wres = write(socket_fd, &m1, sizeof(struct msg));
    if (wres == 0) {
     printf("socket closed prematurely \n");
      close(socket_fd);
      return EXIT_FAILURE;
    }
    if (wres == -1) {
      if (errno == EINTR)
        continue;
      printf("socket write failure \n");
      close(socket_fd);
      return EXIT_FAILURE;
    }
    break;
    }
    while(1)
    {
    int	res = read(socket_fd, readbuf, BUF -1);
	if (res == 0) {
	 printf("socket closed prematurely \n");
	 close(socket_fd);
	 return EXIT_FAILURE;
	}
	if (res == -1) {
	   if (errno == EINTR)
	    continue;
	   printf("socket read failure \n");
	 close(socket_fd);	
	 return EXIT_FAILURE;
	}
	printf("%s\n", readbuf);
	break;
   } 
}
close(socket_fd);
return EXIT_SUCCESS;
}

void 
Usage(char *progname) {
  printf("usage: %s  hostname port \n", progname);
  exit(EXIT_FAILURE);
}

int 
LookupName(char *name,
                unsigned short port,
                struct sockaddr_storage *ret_addr,
                size_t *ret_addrlen) {
  struct addrinfo hints, *results;
  int retval;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // Do the lookup by invoking getaddrinfo().
  if ((retval = getaddrinfo(name, NULL, &hints, &results)) != 0) {
    printf( "getaddrinfo failed: %s", gai_strerror(retval));
    return 0;
  }

  // Set the port in the first result.
  if (results->ai_family == AF_INET) {
    struct sockaddr_in *v4addr =
            (struct sockaddr_in *) (results->ai_addr);
    v4addr->sin_port = htons(port);
  } else if (results->ai_family == AF_INET6) {
    struct sockaddr_in6 *v6addr =
            (struct sockaddr_in6 *)(results->ai_addr);
    v6addr->sin6_port = htons(port);
  } else {
    printf("getaddrinfo failed to provide an IPv4 or IPv6 address \n");
    freeaddrinfo(results);
    return 0;
  }

  // Return the first result.
  assert(results != NULL);
  memcpy(ret_addr, results->ai_addr, results->ai_addrlen);
  *ret_addrlen = results->ai_addrlen;

  // Clean up.
  freeaddrinfo(results);
  return 1;
}

int 
Connect(const struct sockaddr_storage *addr,
             const size_t addrlen,
             int *ret_fd) {
  // Create the socket.
  int socket_fd = socket(addr->ss_family, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    printf("socket() failed: %s", strerror(errno));
   return 0;
  }

  // Connect the socket to the remote host.
  int res = connect(socket_fd,
                    (const struct sockaddr *)(addr),
                    addrlen);
  if (res == -1) {
    printf("connect() failed: %s", strerror(errno));
    return 0;
  }

  *ret_fd = socket_fd;
  return 1;
}
