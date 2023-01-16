#include <stdio.h>
#include "DCL_MsgQueue.h"

int main() {
    printf("Hello, World!\n");
    DCL_Queue_type InstMsgQ;
    DCL_Queue_init(&InstMsgQ);
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for (int i = 0; i < 9; i++) {
        DCL_Queue_pushBack(&InstMsgQ, &data[i]);
    }

    DCL_Queue_print(&InstMsgQ);
    return 0;
}
