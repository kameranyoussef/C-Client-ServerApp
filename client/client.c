// Cwk2: client.c - message length headers with variable sized payloads
//  also use of readn() and writen() implemented in separate code module

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "rdwrn.h"
#include <time.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/utsname.h>
#define INPUTSIZ 10
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>


// menu starts here
void firstoption(socket)
{
    //printf("First option chosen...\n");
 char id_string[32]="Kameran Youssef";
     
    size_t k;

struct ifreq ifr;
/* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want an IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

   

    readn(socket, (unsigned char *) &k, sizeof(size_t));	
    readn(socket, (unsigned char *) id_string, k);
  //  readn(socket, (unsigned char *) name_string, k);
   // readn(socket, (unsigned char *) id_string,k);
    readn(socket, (unsigned char *) &ifr.ifr_addr.sa_family,k);

    //strcat(name_string,id_string);
   // printf("strcat(name_string,id_string\n):%s\n", name_string);


  
   printf("id String: %s\n", id_string);
   printf("IP Address:""%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
   printf("Received: %zu bytes\n\n", k);
}

void secondoption(socket)
{

    //printf("Second option chosen...\n");
time_t t;    // always look up the manual to see the error conditions
    //  here "man 2 time"
    //size_t n = strlen(date) + 1;

  size_t k=0;
    readn(socket, (unsigned char *)&t,k);
    if ((t = time(NULL)) == -1) {
	perror("time error");
	exit(EXIT_FAILURE);
    }

    // localtime() is in standard library so error conditions are
    //  here "man 3 localtime"
    struct tm *tm;
    if ((tm = localtime(&t)) == NULL) {
	perror("localtime error");
	exit(EXIT_FAILURE);
    }    

    printf("%s\n", asctime(tm));
  //  printf("Received: %zu bytes\n\n", k);

}

void thirdoption(socket)
{
    //printf("Third option chosen...\n");
     char sysname[]="";
      size_t k=0;
readn(socket, (unsigned char *)&sysname,k);
struct utsname uts;

    if (uname(&uts) == -1) {
	perror("uname error");
	exit(EXIT_FAILURE);
    }


    printf("Node name:    %s\n", uts.nodename);
    printf("System name:  %s\n", uts.sysname);
    printf("Release:      %s\n", uts.release);
    printf("Version:      %s\n", uts.version);
    printf("Machine:      %s\n", uts.machine);
}
 void fourthoption(socket)
{

      char files [100];
    size_t payload_length = sizeof(files);   
    
    readn(socket, (unsigned char *) &payload_length, sizeof(size_t));	
    readn(socket, (unsigned char *) files, payload_length);

   
    printf("%s\n", files);
}


void displaymenu()
{
    printf("0. Menu\n");
    printf("1. IP Address & Name \n");
    printf("2. Time & Date\n");
    printf("3. Uanme\n");
    printf("4. Files\n");
    printf("5. Exit\n");
    
}


//menu subset ends 



int main(void)
{
    // *** this code down to the next "// ***" does not need to be changed except the port number
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("Error - could not create socket");
	exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;

    // IP address and port of server we want to connect to
    serv_addr.sin_port = htons(50001);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // try to connect...
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)  {
	perror("Error - connect failed");
	exit(1);
    } else
       printf("Connected to server...\n");

    // ***
    // your own application code will go here and replace what is below... 
    // i.e. your menu etc.
char input;
    char name[INPUTSIZ];

    displaymenu();

    do {
	printf("option> ");
	fgets(name, INPUTSIZ, stdin);	// get the value from input
	name[strcspn(name, "\n")] = 0;
	input = name[0];
	if (strlen(name) > 1)
	    input = 'x';	// set invalid if input more 1 char

	switch (input) {
	case '0':
	    displaymenu();
	    break;
	case '1':
	    firstoption();
	    break;
	case '2':
	    secondoption();
	    break;
	case '3':
	    thirdoption();
	    break;
        case '4':
	    fourthoption();
	    break;
	case '5':
	    printf("Exit!\n");
	    exit(0);
	    break;
	default:
	    printf("Invalid choice - 0 displays options...!\n");
	    break;
	}
    } while (input != '4');

// *** make sure sockets are cleaned up

    close(sockfd);

    exit(EXIT_SUCCESS);
} // end main()
