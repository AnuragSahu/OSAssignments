#include "types.h"
#include "stat.h"
#include "user.h"

//char buf[512];
char temp;
int
main(int argc, char *argv[])
{
  int i, j;

  if(argc <= 1){
    //wc(0, "");
    exit();
  }
  
  for(j = 1; j < argc; j++){
    i=0;
    while(argv[j][i]!=0){
      temp = argv[j][i];
      if(temp  >= 'A' && temp <= 'Z')
        argv[j][i] = argv[j][i] + 32;
      else if(temp >= 'a' && temp <= 'z')
        argv[j][i] = argv[j][i] - 32;
      i++;
		}
  }
  for(j = 1; j < argc ; j++){
    printf(1,"%s ",argv[j]);
  }
  printf(1,"\n");
  exit();
}
