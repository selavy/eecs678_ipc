#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define R_FILE "/proc/meminfo"
#define BSIZE 256 

int main()
{
  int status;
  pid_t pid_1, pid_2;

  /* XXX - need to declare pipes and make the pipe() system call before
   * forking any children
   */
  int pipefd[2];

  if( pipe( pipefd ) == -1 )
    {
      perror("pipe");
      exit(EXIT_FAILURE);
    }


  pid_1 = fork();
  if (pid_1 == 0) { 
    /* process a */ 

    int rfd;
    size_t rsize;
    char buf[BSIZE];

    close( pipefd[0] ); /* close read end */
    if ((rfd = open(R_FILE, O_RDONLY)) < 0) {
      fprintf(stderr, "\nError opening file: %s. ERROR#%d\n", R_FILE, errno);
      return EXIT_FAILURE;
    }

    /* read contents of file and write it out to a pipe */
    while ((rsize = read(rfd, buf, BSIZE)) > 0) {
      write(pipefd[1], buf, rsize);
    }

    close( pipefd[1] );
    close(rfd);
    return 0; 
  } 

  pid_2 = fork();
  if (pid_2 == 0) {
    /* process b */
    size_t rsize;
    char buf[BSIZE];

    close( pipefd[1] ); /* close write end */
    /* read from pipe and write out contents to the terminal */
    while ((rsize = read(pipefd[0], buf, BSIZE)) > 0) {
      write(STDOUT_FILENO, buf, rsize);
    }

    close( pipefd[0] );
    return 0;
  }

  close( pipefd[0] );
  close( pipefd[1] );
  /* shell process */
  if ((waitpid(pid_1, &status, 0)) == -1) {
    fprintf(stderr, "Process 1 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }        

  if ((waitpid(pid_2, &status, 0)) == -1) {
    fprintf(stderr, "Process 2 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }

  return 0;
}
