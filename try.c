#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FIND_EXEC "/bin/find"

int main( int argc, char ** argv )
{
  pid_t pid;
  if( argc < 2 )
    return;

  pid = fork();
  if( pid < 0 )
    {
      perror("fork");
    }
  else if( !pid )
    {
      printf("calling find\n");
      execl( FIND_EXEC, "find", argv[1], "-name", "\'*\'.[ch]", (char *) NULL );
      exit(0);
    }
  else
    {
      wait();
    }

  return 0;
}
