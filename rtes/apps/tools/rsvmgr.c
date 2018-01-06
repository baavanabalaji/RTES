#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <asm/unistd.h>

#define KILO 1000
#define MICRO 1000000

int main(int argc, char *argv[])
{
    const char *cmd = argv[1];
    int pid = atoi(argv[2]);
    unsigned long long C, T;
    int cpuid;
    int rc;

    struct timespec C_ts, T_ts;

    if (!strcmp(cmd, "set")) {
        C = atof(argv[3]) * KILO * KILO; // to ns
        T = atof(argv[4]) * KILO * KILO; // to ns
        cpuid = atoi(argv[5]);

        C_ts.tv_sec = C / (KILO * KILO * KILO);
        C_ts.tv_nsec = C % (KILO * KILO * KILO);
        T_ts.tv_sec = T / (KILO * KILO * KILO);;
        T_ts.tv_nsec = T % (KILO * KILO * KILO);;

        rc = syscall(__NR_set_reserve, pid, &C_ts, &T_ts, cpuid);
    }
    else if (!strcmp(cmd, "cancel")) {
        rc = syscall(__NR_cancel_reserve, pid);
    }
    else {
        fprintf(stderr, "unknown command: %s\n", cmd);
        return 1;
    }

    if (rc) {
        fprintf(stderr, "reserve syscall failed with %d\n", rc);
        return 2;
    }
    return 0;
}

