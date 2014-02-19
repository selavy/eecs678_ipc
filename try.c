#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define FIND_EXEC "/usr/bin/find"
#define XARGS_EXEC "/usr/bin/xargs"
#define GREP_EXEC "/bin/grep"
#define SORT_EXEC "/usr/bin/sort"
#define BSIZE 256

int main( int argc, char ** argv )
{
  pid_t pid_1, pid_2, pid_3;
  int findpipe[2], xargspipe[2];
  int status;

  if( pipe(findpipe) == -1 ) {
    perror("find pipe");
  }
  if( pipe(xargspipe) == -1 ) {
    perror("xargs pipe");
  }


  
  if( argc < 2 )
    {
      printf("need 1 argument\n");
      return 0;
    }

  pid_1 = fork();
  if( pid_1 < 0 ) {
      perror("fork #1");
  } else if( !pid_1 ) {
    /* child 1 */
    dup2(findpipe[1], STDOUT_FILENO);
    close(findpipe[1]);
    close(findpipe[0]);
    close(xargspipe[0]);
    close(xargspipe[1]);
    if( execl( FIND_EXEC, "find", argv[1], "-name", "*.[ch]", (char *) NULL ) == -1 ) {
      printf("find $1 -name '*'.[ch] failed\n");
    }
    exit(0);
  }
  
  pid_2 = fork();
  if( pid_2 < 0 ) {
    perror("fork #2");
  } else if( !pid_2 ) {
    dup2(findpipe[0], STDIN_FILENO);
    close(findpipe[0]);
    close(findpipe[1]);
    dup2(xargspipe[1], STDOUT_FILENO);
    close(xargspipe[0]);
    close(xargspipe[1]);
    if( execl( XARGS_EXEC, "xargs", GREP_EXEC, "-c", argv[2], (char *) NULL ) == -1 ) {
      printf("xargs grep -c $2 failed\n");
    }
    exit(0);
  }

  pid_3 = fork();
  if( pid_3 < 0 ) {
    perror("fork #3");
  } else if( !pid_3 ) {
    close(findpipe[0]);
    close(findpipe[1]);
    dup2(xargspipe[0], STDIN_FILENO);
    close(xargspipe[0]);
    close(xargspipe[1]);
    if( execl( SORT_EXEC, "sort", "-t", ":", "+1.0", "-2.0", "--numeric", "--reverse", (char *) NULL ) == -1 ) {
      printf("sort -t : +1.0 -2.0 --numeric --reverse failed\n");
    }
    exit(0);
  }

  close(findpipe[0]);
  close(findpipe[1]);
  close(xargspipe[0]);
  close(xargspipe[1]);

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
  return 0;
}
