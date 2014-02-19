#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FIND_EXEC "/usr/bin/find"
#define BSIZE 256

int main( int argc, char ** argv )
{
  pid_t pid_1, pid_2;
  int pipefd[2];


  if( pipe(pipefd) == -1 ) {
    perror("pipe");
  }
  
  if( argc < 2 )
    {
      printf("need 1 argument\n");
      return 0;
    }

  pid_1 = fork();
  if( pid_1 < 0 ) {
      perror("fork #1");
  }
  else if( !pid_1 ) {
      /* child 1 */
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);
    close(pipefd[0]);
    execl( FIND_EXEC, "find", argv[1], "-name", "*.[ch]", (char *) NULL );
    exit(0);
  }

  pid_2 = fork();
  if( pid_2 < 0 ) {
    perror("fork #2");
  }
  else if( !pid_2 ) {
    size_t rsize;
    char buf[BSIZE];
    dup2(pipefd[0], STDIN_FILENO );
    close( pipefd[0] );
    close(pipefd[1]);
    while( (rsize = read( STDIN_FILENO, buf, BSIZE )) > 0 ) {
      printf("%s", buf);
    }
    exit(0);
  }

  close(pipefd[0]);
  close(pipefd[1]);
  


  wait();
  return 0;
}
