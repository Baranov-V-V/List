#include "list_header.h"
#include "list_func.cpp"

int main() {
    /*List list = {};
    ListConstruct(&list, 20);

    for (int i = 1; i <= 3; ++i) {
        ListPushBack(&list, -10 * i);
        ListPushFront(&list, 10 * i);
    }
    int_t index = ListGetIndex(&list, 4);

    index = ListPushBefore(&list, index, -1000);
    index = ListPushAfter(&list, index, 1000);

    ListPopBack(&list);

    LIST_DUMP(&list, LIST_OK);*/
    UnitTest();
    return 0;
}
