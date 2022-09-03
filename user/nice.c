#include "kernel/types.h"
#include "user.h"


int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 1) {
        printf(2, "Wrong usage of nice\n");
        exit();
    }
    if(argc == 3)
    {
        nice((int) argv[1], (int) argv[2]);
        exit();
    }
    if(argc == 1)
    {
        nice(-1, -1);
        exit();
    }
    exit();
}

