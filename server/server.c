// Cwk2: server.c - multi-threaded server using readn() and writen()

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "rdwrn.h"
#include <sys/utsname.h>
#include <time.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>

static void handler(int sig, siginfo_t *siginfo, void *context)
{
    printf("PID: %ld, UID: %ld\n",
	   (long) siginfo->si_pid, (long) siginfo->si_uid);
}

// thread function
void *client_handler(void *);

typedef struct {
//strt ip
 int fd;

//end ip

    int id_number;
    int age;
    float salary;
}employee;


void get_and_send_employee(int, employee *);
void firstoption(int);
void secondoption(int);
void thirdoption(int);
void fourthoption(int);




// you shouldn't need to change main() in the server except the port number
int main(void)
{
//signal start

    struct sigaction act;

    memset(&act, '\0', sizeof(act));

    // this is a pointer to a function
    act.sa_sigaction = &handler;

    // the SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler
    act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGTERM, &act, NULL) == -1) {
	perror("sigaction");
	exit(EXIT_FAILURE);
    }

//signal ends
    int listenfd = 0, connfd = 0;

    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    socklen_t socksize = sizeof(struct sockaddr_in);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(50001);

    bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (listen(listenfd, 10) == -1) {
	perror("Failed to listen");
	exit(EXIT_FAILURE);
    }
    // end socket setup

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    while (1) {
	printf("Waiting for a client to connect...\n");
	connfd =
	    accept(listenfd, (struct sockaddr *) &client_addr, &socksize);
	printf("Connection accepted...\n");

	pthread_t sniffer_thread;
        // third parameter is a pointer to the thread function, fourth is its actual parameter
	if (pthread_create
	    (&sniffer_thread, NULL, client_handler,
	     (void *) &connfd) < 0) {
	    perror("could not create thread");
	    exit(EXIT_FAILURE);
	}
	//Now join the thread , so that we dont terminate before the thread
	//pthread_join( sniffer_thread , NULL);
	printf("Handler assigned\n");
    }


    // never reached...
    // ** should include a signal handler to clean up
   // exit(EXIT_SUCCESS);
} // end main()

// thread function - one instance of each for each connected client
// this is where the do-while loop will go
void *client_handler(void *socket_desc)
{
    //Get the socket descriptor
    int connfd = *(int *) socket_desc;

    
    firstoption(connfd);
    secondoption(connfd);
    thirdoption(connfd);
    fourthoption(connfd);
    
   // stat_file(connfd,char *file);
   //display_time(connfd)//time
   // display_uname(connfd);

    employee *employee1;
    employee1 = (employee *) malloc(sizeof(employee));

    int i;
    for (i = 0; i < 5; i++) {
	printf("(Counter: %d)\n", i);
	get_and_send_employee(connfd, employee1);
	printf("\n");
    }

    free(employee1);

    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    printf("Thread %lu exiting\n", (unsigned long) pthread_self());

    // always clean up sockets gracefully
    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    return 0;
}  // end client_handler()
//menu first option starts here
void firstoption(int socket)
{

  
     char id_string[] = "";

    size_t n = strlen(id_string) + 1;
    writen(socket, (unsigned char *) &n, sizeof(size_t));	
    writen(socket, (unsigned char *) id_string, n);	  

 // socket= socket(AF_INET, SOCK_DGRAM, 0);
//ip address strts here
 struct ifreq ifr;
/* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want an IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    strcat(ifr.ifr_name, id_string);

    ioctl(socket, SIOCGIFADDR, &ifr);

    writen(socket, (unsigned char *)&ifr.ifr_addr.sa_family,n);



}
//menu fisrt option ends here


//menu time starts here
void secondoption(int socket)
{

    char date[]="";
    time_t t;    // always look up the manual to see the error conditions
    //  here "man 2 time"
   size_t n = strlen(date) + 1;
   writen(socket, (unsigned char *)&t,n);
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

   // printf("%s\n", asctime(tm));

  

}
//menu time ends here

//menu uname starts here
void thirdoption(int socket)
{    

   char sysname[]="";
   size_t n = strlen(sysname) + 1;
    writen(socket, (unsigned char *)&sysname,n);


struct utsname uts;

    if (uname(&uts) == -1) {
	perror("uname error");
	exit(EXIT_FAILURE);
    }

//uname ends here

}
//menu uname ends here



void fourthoption(int socket)
{


   char file[]="";
   //namelist;
   size_t n = strlen(file) + 1;
   writen(socket, (unsigned char *)&file,n);


    struct stat sb;
   

    if (stat(file, &sb) == -1) {
	perror("stat");
	exit(EXIT_FAILURE);
    }

    printf("File type:                ");

    switch (sb.st_mode & S_IFMT) {
    case S_IFBLK:
	printf("block device\n");
	break;
    case S_IFCHR:
	printf("character device\n");
	break;
    case S_IFDIR:
	printf("directory\n");
	break;
    case S_IFIFO:
	printf("FIFO/pipe\n");
	break;
    case S_IFLNK:
	printf("symlink\n");
	break;
    case S_IFREG:
	printf("regular file\n");
	break;
    case S_IFSOCK:
	printf("socket\n");
	break;
    default:
	printf("unknown?\n");
	break;
    }

    printf("I-node number:            %ld\n", (long) sb.st_ino);

    printf("Mode:                     %lo (octal)\n",
	   (unsigned long) sb.st_mode);

    printf("Link count:               %ld\n", (long) sb.st_nlink);
    printf("Ownership:                UID=%ld   GID=%ld\n",
	   (long) sb.st_uid, (long) sb.st_gid);

    printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
    printf("File size:                %lld bytes\n",
	   (long long) sb.st_size);
    printf("Blocks allocated:         %lld\n", (long long) sb.st_blocks);

    printf("Last status change:       %s", ctime(&sb.st_ctime));
    printf("Last file access:         %s", ctime(&sb.st_atime));
    printf("Last file modification:   %s", ctime(&sb.st_mtime));

    printf("\n");

}



//signal handler ends here
void get_and_send_employee(int socket, employee * e)
{
    size_t payload_length;

    size_t n =
	readn(socket, (unsigned char *) &payload_length, sizeof(size_t));
    printf("payload_length is: %zu (%zu bytes)\n", payload_length, n);
    n = readn(socket, (unsigned char *) e, payload_length);

    printf("Age is %d\n", e->age);
    printf("id is %d\n", e->id_number);
    printf("Salary is %6.2f\n", e->salary);
    printf("(%zu bytes)\n", n);

    // make arbitrary changes to the struct & then send it back
    e->age++;
    e->salary += 1.0;

    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
    writen(socket, (unsigned char *) e, payload_length);
}  // end get_and_send_employee()
