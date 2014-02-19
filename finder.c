#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define FIND_EXEC "/usr/bin/find"
#define XARGS_EXEC "/usr/bin/xargs"
#define GREP_EXEC "/bin/grep"
#define SORT_EXEC "/usr/bin/sort"
#define HEAD_EXEC "/usr/bin/head"
#define BSIZE 256

int main( int argc, char ** argv )
{
  pid_t pid_1, pid_2, pid_3, pid_4;
  int findpipe[2], xargspipe[2], sortpipe[2];
  int status;

  if( pipe(findpipe) == -1 ) {
    perror("find pipe");
  }
  if( pipe(xargspipe) == -1 ) {
    perror("xargs pipe");
  }
  if( pipe(sortpipe) == -1 ) {
    perror("sort pipe");
  }
  
  if( argc < 4 )
    {
      printf("need 3 arguments\n");
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
    close(sortpipe[0]);
    close(sortpipe[1]);
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
    close(sortpipe[0]);
    close(sortpipe[1]);
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
    dup2(sortpipe[1], STDOUT_FILENO);
    close(sortpipe[0]);
    close(sortpipe[1]);
    if( execl( SORT_EXEC, "sort", "-t", ":", "+1.0", "-2.0", "--numeric", "--reverse", (char *) NULL ) == -1 ) {
      printf("sort -t : +1.0 -2.0 --numeric --reverse failed\n");
    }
    exit(0);
  }

  pid_4 = fork();
  if( pid_4 < 0 ) {
    perror("fork #4");
  } else if( !pid_4 ) {
    close(findpipe[0]);
    close(findpipe[1]);
    close(xargspipe[0]);
    close(xargspipe[1]);
    dup2(sortpipe[0], STDIN_FILENO);
    close(sortpipe[0]);
    close(sortpipe[1]);
    if( execl( HEAD_EXEC, "head", "--lines", argv[3], (char *) NULL ) == -1 ) {
      printf("head --lines=$3 failed");
    }
    exit(0);
  }

  close(findpipe[0]);
  close(findpipe[1]);
  close(xargspipe[0]);
  close(xargspipe[1]);
  close(sortpipe[0]);
  close(sortpipe[1]);

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
  return 0;
}
