#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>

#define BSIZE 256

#define BASH_EXEC  "/bin/bash"
#define FIND_EXEC  "/bin/find"
#define XARGS_EXEC "/usr/bin/xargs"
#define GREP_EXEC  "/bin/grep"
#define SORT_EXEC  "/bin/sort"
#define HEAD_EXEC  "/usr/bin/head"

int main(int argc, char *argv[])
{
  int status, i;
  pid_t pid_1, pid_2, pid_3, pid_4;
  int pipefd[3][2];
  

  for( i = 0; i < 3; ++i )
    {
      if( pipe( pipefd[i] ) == - 1)
	{
	  perror("pipe");
	  exit(EXIT_FAILURE);
	}
    }

  if (argc != 4) {
    printf("usage: finder DIR STR NUM_FILES\n");
    exit(0);
  }

  pid_1 = fork();
  if (pid_1 == 0) {
    /* First Child */
    printf("child 1\n");
    dup2( pipefd[0][1], STDOUT_FILENO );
    for( i = 0; i < 3; ++i )
      {
	close( pipefd[i][0] );
	close( pipefd[i][1] );
      }
    printf("calling find\n");
    execl(FIND_EXEC, "find", argv[1], "-name", "\'*\'.[ch]", (char *) 0 );
    printf("finished find\n");
    printf("child 1 done\n");
    exit(0);
  }

  pid_2 = fork();
  if (pid_2 == 0) {
    /* Second Child */
    size_t rsize;
    char buf[BSIZE];

    printf("child 2\n");
    
    while((rsize = read(pipefd[0][0], buf, BSIZE)) > 0 ) {
      printf("read %s\n", buf );
    }

    for( i = 0; i < 3; ++i )
      {
	close( pipefd[i][0] );
	close( pipefd[i][1] );
      }

    printf("child 2 done\n");
    exit(0);
  }

  pid_3 = fork();
  if (pid_3 == 0) {
    /* Third Child */
    printf("child 3\n");
    for( i = 0; i < 3; ++i )
      {
	close( pipefd[i][0] );
	close( pipefd[i][1] );
      }
    printf("child 3 done\n");
    exit(0);
  }

  pid_4 = fork();
  if (pid_4 == 0) {
    /* Fourth Child */
    printf("child 4\n");
    for( i = 0; i < 3; ++i )
      {
	close( pipefd[i][0] );
	close( pipefd[i][1] );
      }
    printf("child 4 done\n");
    exit(0);
  }

  for( i = 0; i < 3; ++i )
    {
      close( pipefd[i][0] );
      close( pipefd[i][1] );
    }

  if ((waitpid(pid_1, &status, 0)) == -1) {
    fprintf(stderr, "Process 1 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }
  if ((waitpid(pid_2, &status, 0)) == -1) {
    fprintf(stderr, "Process 2 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }
  if ((waitpid(pid_3, &status, 0)) == -1) {
    fprintf(stderr, "Process 3 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }
  if ((waitpid(pid_4, &status, 0)) == -1) {
    fprintf(stderr, "Process 4 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }

  printf("parent done\n");
  return 0;
}
