#include <stdio.h>
#include "DCL_MsgQueue.h"

int main() {
    printf("Hello, World!\n");
    DCL_Queue_type InstMsgQ;
    DCL_Queue_init(&InstMsgQ);

    int info1 = 1234;
    int info2 = 5678;
    int info3 = 9012;
    int info4 = 3456;
    int info5 = 7890;
    int *pointer;

    while (1) {
        DCL_Queue_pushBack(&InstMsgQ, &info1);
        DCL_Queue_pushBack(&InstMsgQ, &info2);
        DCL_Queue_pushBack(&InstMsgQ, &info3);
        DCL_Queue_pushBack(&InstMsgQ, &info4);

        printf("First:%d Last:%d\n", *(int *) InstMsgQ.first->data, *(int *) InstMsgQ.last->data);


        pointer = (int *) DCL_Queue_pop(&InstMsgQ);

        printf("Popped: %d\n", *pointer);
        printf("First:%d Last:%d\n", *(int *) InstMsgQ.first->data, *(int *) InstMsgQ.last->data);

        pointer = (int *) DCL_Queue_pop(&InstMsgQ);
        printf("Popped: %d\n", *pointer);
        printf("First:%d Last:%d\n", *(int *) InstMsgQ.first->data, *(int *) InstMsgQ.last->data);

        DCL_Queue_pushFront(&InstMsgQ, &info5);
        printf("First:%d Last:%d\n", *(int *) InstMsgQ.first->data, *(int *) InstMsgQ.last->data);

        DCL_Queue_free(&InstMsgQ);
        printf("First:%p Last:%p\n", InstMsgQ.first, InstMsgQ.last);
    }

    return 0;
}
