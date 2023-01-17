#include <stdio.h>
#include "DCL_MsgQueue.h"
#include "TST_MsgQueue.h"

#define TEST_LEN 10

int main() {
    DCL_Queue_type MsgQueue;
    DCL_Queue_init(&MsgQueue);

    Msg_data_type buffer;
    for (int i = 0; i < TEST_LEN; i++) {
        TST_GenMsg(&buffer);
        printf("Operation:%d ID:%d MSG:%s\n", i, buffer.ID, buffer.message);
        TST_MsgQueue(&MsgQueue, &buffer);
    }

    for (int i = 0; i < TEST_LEN; i++) {
        TST_MsgRead(&MsgQueue, &buffer);
        printf("Operation:%d ID:%d MSG:%s\n", i, buffer.ID, buffer.message);
    }

    return 0;
}
