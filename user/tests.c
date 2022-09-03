#include "kernel/types.h"
#include "user.h"

int stdout = 1;

void niceTest()
{
    printf(stdout, "running nice test\n");
    nice(-1, -1);
    nice(1, prng());
    printf(stdout, "Changed Priority\n");
    nice(-1, -1);
}

void prngTest() {
  printf(stdout, "running prng test\n");
  int i;
  int min = -1;
  int max = -1;
  int sum = 0;
  for (i = 0; i < 20; i++)
  {
    int num = prng();
    printf(stdout, "Num: %d\n",  num);
    if(min == -1){min = num;}
    if(max == -1){ max = num;}
    if(min > num){min = num;}
    if(max < num)
    {max = num;}
    sum += num;
  }
  int mean = sum/ 100;
  printf(stdout, "\n");
  printf(stdout, "Min: %d\n",   min);
  printf(stdout, "Max: %d\n",   max);
  printf(stdout, "Mean: %d\n",  mean);
}

void schedulerTest1(){
  int k, n;
  int x, z;

  n = 7;
  int pid = 0;
  int limit = 9;
  for (k = 0; k < n; k++) {
    pid = fork();
    if (pid < 0) {
      printf(1, "%d failed in fork!\n", getpid());
    } else if (pid > 0) {
      printf(1, "Parent %d creating child %d\n", getpid(), pid);
    } else {
      int id = getpid();
      for (z = 0; z < limit; z += 1)
      {
        x = x + 3.14 * 89.64; // Useless calculation to consume CPU Time
        printf(1, "%d/%d completed in %d\n", z+1, limit, id);  
      }
      exit();
    }
  }
}

void schedulerTest2(){
  int k, n;
  int x, z;

  n = 1;
  int pid = 0;
  int limit = 9;
  for (k = 0; k < n; k++) {
    pid = fork();
    if (pid < 0) {
      printf(1, "%d failed in fork!\n", getpid());
    } else if (pid > 0) {
      printf(1, "Parent %d creating child %d\n", getpid(), pid);
    } else {
      int id = getpid();
      for (z = 0; z < limit; z += 1)
      {
        printf(1, "%d/%d completed in %d\n", z+1, limit, id);  
        x = x + 3.14 * 89.64; // Useless calculation to consume CPU Time
      }
      exit();
    }
  }
}

void schedulerTest3(){
  int k, n;
  int x, z;

  n = 7;
  int pid = 0;
  int limit = 2;
  for (k = 0; k < n; k++) {
    pid = fork();
    if (pid < 0) {
      printf(1, "%d failed in fork!\n", getpid());
    } else if (pid > 0) {
      printf(1, "Parent %d creating child %d\n", getpid(), pid);
    } else {
      int id = getpid();
      for (z = 0; z < limit; z += 1)
      {
        printf(1, "%d/%d completed in %d\n", z+1, limit, id);  
        x = x + 3.14 * 89.64; // Useless calculation to consume CPU Time
      }
      exit();
    }
  }
}

void schedulerTest4(){
  int k, n;
  int x, z;

  n = 7;
  int pid = 0;
  int limit = 9;
  for (k = 0; k < n; k++) {
    pid = fork();
    if (pid < 0) {
      printf(1, "%d failed in fork!\n", getpid());
    } else if (pid > 0) {
      printf(1, "Parent %d creating child %d\n", getpid(), pid);  
      wait();
    } else {
      int id = getpid();
      for (z = 0; z < limit; z += 1)
      {
        x = x + 3.14 * 89.64; // Useless calculation to consume CPU Time
        printf(1, "%d/%d completed in %d\n", z+1, limit, id);  
      }
      exit();
    }
  }
}

void schedulerTest5(){
  int k, n;
  int x, z;

  n = 1;
  int pid = 0;
  int limit = 1;
  for (k = 0; k < n; k++) {
    pid = fork();
    if (pid < 0) {
      printf(1, "%d failed in fork!\n", getpid());
    } else if (pid > 0) {
      printf(1, "Parent %d creating child %d\n", getpid(), pid);  
    } else {
      int id = getpid();
      for (z = 0; z < limit; z += 1)
      {
        printf(1, "%d/%d completed in %d\n", z+1, limit, id);  
        x = x + 3.14 * 89.64; // Useless calculation to consume CPU Time
      }
      exit();
    }
  }
}

void schedulerTest6()
{
  int k, n;
  int x, z;

  n = 7;
  int pid = 0;
  int limit = 9;
  for (k = 0; k < n; k++) {
    pid = fork();
    if (pid < 0) {
      printf(1, "%d failed in fork!\n", getpid());
    } else if (pid > 0) {
      printf(1, "Parent %d creating child %d\n", getpid(), pid);  
    } else {
      int id = getpid();
      for (z = 0; z < limit; z += 1)
      {
        if(id % 2 == 0)
        {
            //printf(1, " Changing priority of %d dynamically\n", id);
            nice(id, prng());
            // nice(-1, -1);
        }          
        printf(1, "%d/%d completed in %d\n", z+1, limit, id);  
        x = x + 3.14 * 89.64; // Useless calculation to consume CPU Time
      }
      exit();
    }
  }
}

// 1, 4, 6
int main(int argc, char *argv[]) {
    printf(stdout, "tests starting\n");
    if(strcmp(argv[1], "nice") == 0)
    {
        niceTest();
    }
    else if(strcmp(argv[1], "prng") == 0)
    {
        prngTest();
    }
    else if(strcmp(argv[1], "lottery") == 0)
    {
        int x = argv[2][0] -'0';
        switch(x){
            case 1:
              printf(stdout, "running lottery test without parent waiting\n");
              schedulerTest1();
              break;
            case 4:
              printf(stdout, "running lottery test with parent waiting\n");
              schedulerTest4();
              break;
            case 6:
            default:
              printf(stdout, "running lottery test with dynamically changing policy \n");
              schedulerTest6();
              break;
        }
    }
    else {
        printf(stdout, "invalid test\n");
    }
}