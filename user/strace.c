#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "ush.c"
#include "kernel/strace.h"



/* 

2 args:strace on 
strace off
strace dump

>2:strace run <command>
strace -f <command>
strace -s <command>
strace -e <system-call-name> <command>

strace -s -e <system-call-name> <command>
strace -f -e <system-call-name> <command>
strace -o <filename> run <command> 
strace -o <filename> -f <command> 
strace -o <filename> -s <command> 

strace -o <filename> -e <system-call-name> <command> 
strace -o <filename> -s -e <system-call-name> <command>
strace -o <filename> -f -e <system-call-name> <command>

*/

int strCmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char *strcat(char *dest, const char *src)
{
    int i,j;
    for (i = 0; dest[i] != '\0'; i++)
        ;
    for (j = 0; src[j] != '\0'; j++)
        dest[i+j] = src[j];
    dest[i+j] = '\0';
    return dest;
}

// int strcmpe(const char * str1,
//    const char * str2, int j, int k, int isCaseInsensitive) {
//    if (j != k) {
//       return 0;
//    } else {
//       int i;
//       if (isCaseInsensitive == 0) {
//          for (i = 0; i < j; i++) {
//             if (str1[i] != str2[i]) {
//                return 0;
//             }
//          }
//       } else {
//          for (i = 0; i < j; i++) {
//             if (str1[i] != str2[i]) {
//                if (str1[i] >= A && str1[i] <= Z) {
//                   if (str2[i] >= a && str2[i] <= z) {
//                      if (str1[i] != str2[i] - 32) {
//                         return 0;
//                      }
//                   } else {
//                      return 0;
//                   }
//                } else if (str1[i] >= a && str1[i] <= z) {
//                   if (str2[i] >= A && str2[i] <= Z) {
//                      if (str2[i] != str1[i] - 32) {
//                         return 0;
//                      }
//                   } else {
//                      return 0;
//                   }
//                } else {
//                   return 0;
//                }
//             }
//          }
//       }
//    }
//    return 1;
// }

void getSubStr(char *str, int start, int end, char *buffer) {
    int length = end - start;
    int i;
    for (i = 0; i < length; i++) {
        buffer[i] = *(str+i+start);
    }
}

int main(int argc, char *argv[]) {
    int i;
    char dump[512];
    dump[0] = '\0';
//   char dupe[512];
    if (argc < 2 || strCmp(argv[1], "-h") == 0) {
        printf(2, "Usage: %s \n", dump[0]);
        printf(2, "Usage: \n");
        printf(2, "Usage: strace on|off|dump\n"); 
        printf(2, "Usage: strace [-c] [-o <filename>] run|-f|-s <command>\n");
        printf(2, "Usage: strace [-c] [-o <filename>] [-s|-f] -e <system-call-name> <command>\n");
        exit();
    }

    if(strCmp(argv[1], "on") == 0)
    {
        printf(2, "Turned on\n");
        // option,c,o,filename, run|-f|-s, e, systemcall name, command
        strace(1, -1, -1, "", -1, 0, "", "", dump);
        exit();
    }
    if(strCmp(argv[1], "off") == 0)
    {
        strace(2, -1, -1, "", -1, 0, "", "", "");
        exit();
    }
    if(strCmp(argv[1], "dump") == 0)
    {
        strace(5, -1, -1, "", -1, 0, "", "", "");
        exit();
    }

    if(strCmp(argv[1], "-o") == 0)
    {
        // option,c,o,filename, run|-f|-s, e, systemcall name, command
        int fd;
        fd = open(argv[i+1], O_CREATE | O_RDWR);

        if (fd < 0) {
            printf(1, "uniq: cannot open %s\n", argv[i+1]);
            exit();
        }
        else {
            printf(1, "Create Successful %s\n", argv[i+1]);
        }
        // if (write(fd, "aaaaaaaaaa", 10) != 10) {
        //     printf(1, "error: write aa new file failed\n");
        //     exit();
        // }
        int k;
        for(k = 0; k< N; k++)
        {
            dump[0] = '\0';
            strace(6, -1, 1, argv[i+1], -1, 0, "", "", dump);
            if(dump[0] == '\0')
            {
                break;
            }
            write(fd, dump, 512);
            memset(dump, 0, 512);

            // printf(1, "%s", dump);
        }
        dump[0] = '\0';
        write(fd, dump, sizeof(dump));
        close(fd);
        exit();
    }

    if(strCmp(argv[i], "run") == 0)
    {   
        // // option,c,o,filename, run|-f|-s, e, systemcall name, command
        strace(3, -1, -1, "", -1, 0, "", "", "");
        // strace(3, cFlag, oFlag, sysFileName, sFlag, eFlag, sysCallName, "");
        int j;
        char remBuf[100];
        for(j = i+1; j< argc; j++)
        {
            strcat(remBuf, argv[j]);
            strcat(remBuf, " ");
        }
        runcmd(parsecmd(remBuf));
        exit();
    }


    
    int eFlag = 0;
    int oFlag = 0;
    int cFlag = 0;
    int sFlag = -1;
    char sysCallName[100] = {'\0'};
    char sysFileName[100] = {'\0'};
    k = -1;
    // Add error handling for all the command scan codes
    for(int j = 1; j<argc; j++)
    {
        if(strCmp(argv[j], "-e") == 0)
        {   
            strcat(sysCallName, argv[j+1]);
            k = j + 2;
            eFlag = 1;
            break;
        }
    }

    for(int j = 1; j<argc; j++)
    {
        if(strCmp(argv[j], "-o") == 0)
        {   
            strcat(sysFileName, argv[j+1]);
            oFlag = 1;
            break;
        }
    }
    
    for(int j = 1; j<argc; j++)
    {
        if(strCmp(argv[j], "-s") == 0)
        {   
            sFlag = 1;
            break;
        }
        if(strCmp(argv[j], "-f") == 0)
        {
            sFlag = 0;
            break;  
        }
    }

    for(int j = 1; j<argc; j++)
    {
        if(strCmp(argv[j], "-c") == 0)
        {   
            cFlag = 1;
            break;
        }
    }
    // int count = 0;
    // printf(2, "count %d Flag Status: cFlag %d eFlag: %d %s \t oFlag: %d %s \t sFlag: %d\n",count, cFlag, eFlag, sysCallName, oFlag, sysFileName, sFlag);
    // Strace will have the format of strace flags followed by run followed by the command
    // count = count + 1;

    // if(strCmp(argv[i], "run") == 0)
    // {   
        // // option,c,o,filename, run|-f|-s, e, systemcall name, command
    strace(4, cFlag, oFlag, sysFileName, sFlag, eFlag, sysCallName, "");
    int j;
    char remBuf[100];
    for(j = k; j< argc; j++)
    {
        strcat(remBuf, argv[j]);
        strcat(remBuf, " ");
    }
    runcmd(parsecmd(remBuf));
    // }
   

    exit();
}
