#include "kernel/syscall.h"
#include "kernel/defs.h"
#include "kernel/memlayout.h"
#include "kernel/mmu.h"
#include "kernel/param.h"
#include "kernel/proc.h"
#include "kernel/strace.h"
#include "kernel/types.h"
#include "kernel/x86.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int fetchint(uint addr, int *ip) {
  if (addr >= proc->sz || addr + 4 > proc->sz)
    return -1;
  *ip = *(int *)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int fetchstr(uint addr, char **pp) {
  char *s, *ep;

  if (addr >= proc->sz)
    return -1;
  *pp = (char *)addr;
  ep = (char *)proc->sz;
  for (s = *pp; s < ep; s++)
    if (*s == 0)
      return s - *pp;
  return -1;
}

// Fetch the nth 32-bit system call argument.
int argint(int n, int *ip) { return fetchint(proc->tf->esp + 4 + 4 * n, ip); }

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size n bytes.  Check that the pointer
// lies within the process address space.
int argptr(int n, char **pp, int size) {
  int i;

  if (argint(n, &i) < 0)
    return -1;
  if ((uint)i >= proc->sz || (uint)i + size > proc->sz)
    return -1;
  *pp = (char *)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int argstr(int n, char **pp) {
  int addr;
  if (argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_strace(void);

static int (*syscalls[])(void) = {
    [SYS_fork] sys_fork,     [SYS_exit] sys_exit,     [SYS_wait] sys_wait,
    [SYS_pipe] sys_pipe,     [SYS_read] sys_read,     [SYS_kill] sys_kill,
    [SYS_exec] sys_exec,     [SYS_fstat] sys_fstat,   [SYS_chdir] sys_chdir,
    [SYS_dup] sys_dup,       [SYS_getpid] sys_getpid, [SYS_sbrk] sys_sbrk,
    [SYS_sleep] sys_sleep,   [SYS_uptime] sys_uptime, [SYS_open] sys_open,
    [SYS_write] sys_write,   [SYS_mknod] sys_mknod,   [SYS_unlink] sys_unlink,
    [SYS_link] sys_link,     [SYS_mkdir] sys_mkdir,   [SYS_close] sys_close,
    [SYS_strace] sys_strace,
};

const char *syscall_name[] = {
    "fork",  "exit",   "wait",   "pipe",  "read",  "kill",   "exec", "fstat",
    "chdir", "dup",    "getpid", "sbrk",  "sleep", "uptime", "open", "write",
    "mknod", "unlink", "link",   "mkdir", "close", "strace"};

int continueTrace = 0;

int eFlag = 0;
int oFlag = 0;
int cFlag = 0;
int sFlag = -1;
char sysCallName[100] = "\0";
char sysFileName[100] = "\0";

void updateTraceFlags(int eFlagInp, int oFlagInp, int cFlagInp, int sFlagInp,
                      char *sysCallNameInp, char *sysFileNameInp) {
  eFlag = eFlagInp;
  oFlag = oFlagInp;
  cFlag = cFlagInp;
  sFlag = sFlagInp;
  strcat(sysCallName, sysCallNameInp);
  strcat(sysFileName, sysCallNameInp);
}

void setContinueTrace(int toggle) { continueTrace = toggle; }

int shouldContinueTrace() { return continueTrace; }

int isRunTrace = 0;

void setRunTrace(int toggle) { isRunTrace = toggle; }

int runTrace() { return isRunTrace; }

int strCmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char *strcat(char *dest, const char *src) {
  int i, j;
  for (i = 0; dest[i] != '\0'; i++)
    ;
  for (j = 0; src[j] != '\0'; j++)
    dest[i + j] = src[j];
  dest[i + j] = '\0';
  return dest;
}

// Function to swap two numbers
void swap(char *x, char *y) {
  char t = *x;
  *x = *y;
  *y = t;
}

// Function to reverse `buffer[i…j]`
char *reverse(char *buffer, int i, int j) {
  while (i < j) {
    swap(&buffer[i++], &buffer[j--]);
  }

  return buffer;
}

int abs(int num) {
  /* if the passed value (num) is less than 0 (zero),
  the number multiplied by (-1) to return an absolute value. */
  if (num < 0) {
    num = (-1) * num; // given negative number multiplied by (-1)
  }
  return num;
}

// Iterative function to implement `itoa()` function in C
char *itoa(int value, char *buffer, int base) {
  // invalid input
  if (base < 2 || base > 32) {
    return buffer;
  }

  // consider the absolute value of the number
  int n = abs(value);

  int i = 0;
  while (n) {
    int r = n % base;

    if (r >= 10) {
      buffer[i++] = 65 + (r - 10);
    } else {
      buffer[i++] = 48 + r;
    }

    n = n / base;
  }

  // if the number is 0
  if (i == 0) {
    buffer[i++] = '0';
  }

  // If the base is 10 and the value is negative, the resulting string
  // is preceded with a minus sign (-)
  // With any other base, value is always considered unsigned
  if (value < 0 && base == 10) {
    buffer[i++] = '-';
  }

  buffer[i] = '\0'; // null terminate string

  // reverse the string and return it
  return reverse(buffer, 0, i - 1);
}

int startAcc = 0;
int dumpBufferHead = 0;
int dumpStartFrom = -1;
char dump_buffer[N][512];
int bufferHead = 0;
int startFrom = -1;
char trace_buffer[1024][512];
int previousPid = -1;
char previousCommandName[50] = "\0";
char previousSyscall[50] = "\0";
int previousReturnValue = -1;
int userHead = -1;

void clearBuffer() {
  bufferHead = 0;
  startFrom = -1;
  int i;
  for (i = 0; i < 1024; i++) {
    trace_buffer[i][0] = '\0';
  }
}

void writeDumpToUserBuffer(char *userDump) {
  if (dumpStartFrom == -1) {
    if (userHead == -1) {
      userHead = 0;
    }
    // cprintf("\nuserHead %d ", userHead);
    // cprintf("\ndumpBufferHead%d ", dumpBufferHead);

    if (dump_buffer[userHead][0] == '\0' || userHead == dumpBufferHead) {
      userHead = -1;
    } else {
      // char numStr[10] = "\0";
      // strcat(userDump, "User Head: ");
      // strcat(userDump, itoa(userHead, numStr, 10));
      // strcat(userDump, " - ");
      strcat(userDump, dump_buffer[userHead]);
      strcat(userDump, "\n");
      userHead += 1;
    }
  } else {
    if (userHead == -1) {
      userHead = dumpStartFrom;
    }
    // cprintf("\nuserHead %d ", userHead);
    strcat(userDump, dump_buffer[userHead]);
    strcat(userDump, "\n");
    userHead += 1;
    if (userHead == N) {
      userHead = 0;
    }
  }
}

void printDump() {
  if (dumpStartFrom == -1) {
    int i;
    for (i = 0; i < N; i++) {
      if (dump_buffer[i][0] == '\0') {
        break;
      }
      cprintf("\n %s ", dump_buffer[i]);
    }
  } else {
    int i;
    for (i = dumpStartFrom; i < N; i++) {
      cprintf("\n %s ", dump_buffer[i]);
    }
    for (i = 0; i < dumpStartFrom; i++) {
      cprintf("\n %s ", dump_buffer[i]);
    }
  }
  cprintf("\n\n");
}

void printCurrentTrace() {
  if (startFrom == -1) {
    int i;
    for (i = 0; i < 1024; i++) {
      if (trace_buffer[i][0] == '\0') {
        break;
      }
      cprintf("\n %s ", trace_buffer[i]);
    }
  } else {
    int i;
    for (i = startFrom; i < 1024; i++) {
      cprintf("\n %s ", trace_buffer[i]);
    }
    for (i = 0; i < startFrom; i++) {
      cprintf("\n %s ", trace_buffer[i]);
    }
  }
  cprintf("\n\n");
}

void syscall(void) {
  int num;
  num = proc->tf->eax;

  if (num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    proc->tf->eax = syscalls[num]();
    // sprintf(trace_buffer, sizeof(trace_buffer), "TRACE: pid = %d |
    // command_name = %s | syscall = %s | return value = %d\n", proc->pid,
    // proc->name, syscall_name[num-1], proc->tf->eax);

    if (startAcc && strCmp(proc->name, "strace") != 0) {
      char trace[512] = "\0";
      char pidstr[10] = "\0";
      char eaxstr[10] = "\0";
      // if(!(previousPid == proc->pid && strCmp(previousCommandName,
      // proc->name) == 0 && strCmp(previousSyscall, syscall_name[num-1]) == 0
      // && previousReturnValue == proc->tf->eax))
      // {
      strcat(trace, "Trace: pid = ");
      strcat(trace, itoa(proc->pid, pidstr, 10));
      strcat(trace, " | ");
      strcat(trace, "command_name = ");
      strcat(trace, proc->name);
      strcat(trace, " | ");
      strcat(trace, "syscall = ");
      strcat(trace, syscall_name[num - 1]);
      strcat(trace, " | ");
      strcat(trace, "return value = ");
      strcat(trace, itoa(proc->tf->eax, eaxstr, 10));
      strcat(trace, "\0");
      if (dumpBufferHead == N) {
        dumpBufferHead -= N;
        dumpStartFrom = dumpBufferHead;
      }
      dump_buffer[dumpBufferHead][0] = '\0';
      strcat(dump_buffer[dumpBufferHead], trace);
      dumpBufferHead += 1;
      if (dumpStartFrom != -1) {
        dumpStartFrom += 1;
      }

      if (bufferHead == 1024) {
        bufferHead -= 1024;
        startFrom = bufferHead;
      }
      trace_buffer[bufferHead][0] = '\0';
      strcat(trace_buffer[bufferHead], trace);
      bufferHead += 1;
      if (startFrom != -1) {
        startFrom += 1;
      }

      previousPid = proc->pid;
      previousCommandName[0] = '\0';
      strcat(previousCommandName, proc->name);
      previousSyscall[0] = '\0';
      strcat(previousSyscall, syscall_name[num - 1]);
      previousReturnValue = proc->tf->eax;
      // }
      //
    }
    if ((strCmp(proc->name, "sh") == 0 &&
         strCmp(syscall_name[num - 1], "wait") == 0) ||
        strCmp(syscall_name[num - 1], "end") == 0) {
      if (shouldContinueTrace() || runTrace()) {
        // cprintf("\nTRACE: pid = %d | command_name = %s | syscall = %s |
        // return value = %d\n", proc->pid, proc->name, syscall_name[num-1],
        // proc->tf->eax);
        if (eFlag == 1 && sFlag == 1) {
          if (strCmp(syscall_name[num - 1], sysCallName) == 0) {
            if (proc->tf->eax != -1) {
                printCurrentTrace();
            }
          }
        } else if (eFlag == 1 && sFlag == 0) {
          if (strCmp(syscall_name[num - 1], sysCallName) == 0) {
            if (proc->tf->eax == -1) {
                printCurrentTrace();
            }
          }
        }
        // case 1: eFlag is set
        else if (eFlag == 1) {
          if (strCmp(syscall_name[num - 1], sysCallName) == 0) {
            printCurrentTrace();
          }
        } else if (sFlag == 1) { // case 2: s flag is set
          if (proc->tf->eax != -1) {
            printCurrentTrace();
          }
        } else if (sFlag == 0) { // case 3: flag is set
          if (proc->tf->eax == -1) {
            printCurrentTrace();
          }
        }
        else
        {
            printCurrentTrace();
        }
      }
      clearBuffer();
      setRunTrace(0);
    }
  }
  if (strCmp(proc->name, "sh") == 0) {
    startAcc = 1;
  }
}
else {
  cprintf("%d %s: unknown sys call %d\n", proc->pid, proc->name, num);
  proc->tf->eax = -1;
}
}
