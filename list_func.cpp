#include "list_header.h"

type_t* MakeData(type_t* data, int_t data_size) {
    assert(data_size >= 0);

    for(int_t i = 0; i < data_size + 1; ++i) {
        data[i] = NAN;
    }

    return data;
}

int_t* MakeNext(int_t* next, int_t next_size) {
    assert(next_size >= 0);

    next[0] = 0;
    for(int_t i = 1; i < next_size; ++i) {
        next[i] = i + 1;
    }
    next[next_size] = 0;

    return next;
};

int_t* MakePrev(int_t* prev, int_t prev_size) {
    assert(prev_size >= 0);

    prev[0] = 0;
    for(int_t i = 1; i < prev_size + 1; ++i) {
        prev[i] = -1;
    }

    return prev;
};

ListExitCodes ListConstruct(List* list, int_t construct_size) {
    assert(list != NULL);
    assert(construct_size >= 0);

    list->data = (type_t*) calloc(construct_size + 1, sizeof(int_t));
    assert(list->data != NULL);
    list->next = (int_t*) calloc(construct_size + 1, sizeof(int_t));
    assert(list->next != NULL);
    list->prev = (int_t*) calloc(construct_size + 1, sizeof(int_t));
    assert(list->prev != NULL);

    MakeData(list->data, construct_size);
    MakeNext(list->next, construct_size);
    MakePrev(list->prev, construct_size);

    list->capacity = construct_size;
    list->size = 0;
    list->head = 0;
    list->tail = 0;
    list->free = 1;
    list->is_sort = false;

    ASSERT_LIST(list);

    return SUCCESS;
}

void ListTextDump(List* list, FILE* fp, ListErrors error_no) {
    assert(list != NULL);
    assert(fp != NULL);

    fprintf(fp, "Error code : %d (%s)\n\n", error_no, ListErrorNames[error_no]);
    fprintf(fp, "adress of List: [%p]\n", list);

    fprintf(fp, "{\n    capacity = %ld\n", list->capacity);
    fprintf(fp, "    size = %lld\n", list->size);
    fprintf(fp, "    head = %lld\n", list->head);
    fprintf(fp, "    tail = %lld\n", list->tail);
    fprintf(fp, "    free = %lld\n\n", list->free);

    fprintf(fp, "    data adress: [%p]\n", list->data);
    fprintf(fp, "    next adress: [%p]\n", list->next);
    fprintf(fp, "    prev adress: [%p]\n\n", list->prev);

    fprintf(fp, "    {\n");
    fprintf(fp, "             Data                        Next            Prev\n");
    for (int_t i = 0; i < list->capacity + 1; i++) {            //надо понять где poison а где нет
        if (i != 0 && list->prev[i] != -1) {
            fseek(fp, 7, SEEK_CUR);
            int symbols_count = 0;

            fprintf(fp, "*[%lld] %n", i, &symbols_count);

            fseek(fp, 10 - symbols_count, SEEK_CUR);
            fprintf(fp, " = %f (Valid!) %n", list->data[i], &symbols_count);

            fseek(fp, 24 - symbols_count, SEEK_CUR);
            fprintf(fp, "%lld %n", list->next[i], &symbols_count);

            fseek(fp, 15 - symbols_count, SEEK_CUR);
            fprintf(fp, "%lld\n", list->prev[i]);
        }
        else {
            fseek(fp, 8, SEEK_CUR);
            int symbols_count = 0;

            fprintf(fp, "[%lld]%n", i, &symbols_count);

            fseek(fp, 10 - symbols_count, SEEK_CUR);
            fprintf(fp, "= %f (Poison!)%n", list->data[i], &symbols_count);

            fseek(fp, 24 - symbols_count, SEEK_CUR);
            fprintf(fp, "%lld%n", list->next[i], &symbols_count);

            fseek(fp, 15 - symbols_count, SEEK_CUR);
            fprintf(fp, "%lld\n", list->prev[i]);
        }
    }
    fprintf(fp, "   }\n");

    fprintf(fp, "}\n");
}

void ListGraphDump(List* list, const char* file_name) {
    assert(list != NULL);
    assert(file_name != NULL);

    char* command_dot = (char*) calloc(100, sizeof(char));
    char* graph_file = (char*) calloc(strlen(file_name) + 10, sizeof(char));

    strcpy(graph_file, file_name);
    strcat(graph_file, ".dot");
    FILE* fp = fopen(graph_file, "w");
    assert(fp != NULL);

    strcat(command_dot, "dot -Tpng ");   //graph_test1.dot -o Demo3.png"
    strcat(command_dot, graph_file);
    strcat(command_dot, " -o ");
    strcat(command_dot, file_name);
    strcat(command_dot, "_graph.png");

    fprintf(fp, "digraph structs {\n    rankdir=LR\n    node [shape=\"record\", style=\"filled\", color=\"#008000\", fillcolor=\"#C0FFC0\"];");  //rankdir = LR\n

   // fprintf(fp, "    \"list_node%lld\" [fillcolor=\"crimson\", label =\"{<index> index\\n %lld |{<value> value\\n %f |<next> next\\n %lld |<prev> prev\\n %lld}}\"]\n",
     //       0, 0, list->data[0], list->next[0], list->prev[0]);

    for (int_t i = 0; i < list->capacity + 1; ++i) {
        if (i == 0) {
            fprintf(fp, "    \"list_node%lld\" [fillcolor=\"crimson\", label =\"<index> index\\n %lld |{<value> value\\n %f |<next> next\\n %lld |<prev> prev\\n %lld}\"]\n",
                    i, i, list->data[i], list->next[i], list->prev[i]);
            continue;
        }
        if (list->prev[i] == 0) {
            fprintf(fp, "    \"list_node%lld\" [fillcolor=\"pink\", label =\"<head> HEAD|<index> index\\n %lld |{<value> value\\n %f |<next> next\\n %lld |<prev> prev\\n %lld}\"]\n",
                    i, i, list->data[i], list->next[i], list->prev[i]);
        }
        else if (list->next[i] == 0 && list->prev[i] != -1) {
            fprintf(fp, "    \"list_node%lld\" [fillcolor=\"pink\", label =\"<tail> TAIL|<index> index\\n %lld |{<value> value\\n %f |<next> next\\n %lld |<prev> prev\\n %lld}\"]\n",
                    i, i, list->data[i], list->next[i], list->prev[i]);
        }
        else if (list->prev[i] != -1) {
            fprintf(fp, "    \"list_node%lld\" [fillcolor=\"#C0FFC0\", label =\"<index> index\\n %lld |{<value> value\\n %f |<next> next\\n %lld |<prev> prev\\n %lld}\"]\n",
                    i, i, list->data[i], list->next[i], list->prev[i]);
        }
        else{
            fprintf(fp, "    \"list_node%lld\" [fillcolor=\"yellow\", label =\"<index> index\\n %lld |{<value> value\\n %f |<next> next\\n %lld |<prev> prev\\n %lld}\"]\n",
                    i, i, list->data[i], list->next[i], list->prev[i]);
        }
    }

    for(int_t i = 0; i < list->capacity; ++i) {
        fprintf(fp, "    \"list_node%lld\":index -> \"list_node%lld\":index\n", i, i + 1);
        if (list->prev[i] != -1) {
            fprintf(fp, "    \"list_node%lld\":next -> \"list_node%lld\":index[color=\"blue\", constraint=false];\n", i, list->next[i]);
        }
    }

    fprintf(fp, "}");

    fclose(fp);

    printf("graphing done with code: %d", system(command_dot));
}

ListErrors ListOk(List* list) {
    assert(list != NULL);

    if (list->size < 0) {
        return LIST_SIZE_ERROR;
    }
    if (list->capacity < 0) {
        return LIST_CAPACITY_ERROR;
    }
    if (list->size > list->capacity) {
        return LIST_SIZE_OVERFLOW;
    }
    if (list->head < 0 || list->head > list->capacity) {
        return LIST_HEAD_ERROR;
    }
    if (list->tail < 0 || list->tail > list->capacity) {
        return LIST_TAIL_ERROR;
    }
    if (list->free < 0 || list->free > list->capacity) {
        return LIST_FREE_ERROR;
    }

    if (list->data == NULL) {
        return LIST_DATA_NULL;
    }
    if (list->prev == NULL) {
        return LIST_PREV_NULL;
    }
    if (list->next == NULL) {
        return LIST_NEXT_NULL;
    }

    if (std::isfinite(list->data[0]) != 0 || list->prev[0] != 0 || list->next[0] != 0) {
        return ZERO_ELEM_ERROR;
    }

    for (int i = 1; i < list->capacity + 1; ++i) {
        if (std::isfinite(list->data[i]) != 0 && list->prev[i] == -1) {
            return LIST_POISON_ERROR;
        }
    }

    for (int i = 2; i < list->capacity; ++i) {
        if (list->prev[i] != -1 && list->prev[list->next[i]] != i && list->next[list->prev[i]] != i) {
            if (list->data[list->prev[list->next[i]]] != list->data[i]) {
                return LIST_PREV_NEXT_ERROR;
            }
            if (list->data[list->next[list->prev[i]]] != list->data[i]) {
                return LIST_NEXT_PREV_ERROR;
            }
        }
    }

    return LIST_OK;
}

ListExitCodes ListDestruct(List* list) {
    assert(list != NULL);
    ASSERT_LIST(list);

    list->size = -1;
    list->capacity = -1;
    list->head = -1;
    list->tail = -1;
    list->free = -1;

    free(list->data);
         list->data = NULL;
    free(list->next);
         list->next = NULL;
    free(list->prev);
         list->prev = NULL;

    return SUCCESS;
}

ListExitCodes ListClear(List* list) {
    assert(list != NULL);
    ASSERT_LIST(list);

    list->size = 0;
    list->head = 0;
    list->tail = 0;
    list->free = 1;

    MakeData(list->data, list->capacity);
    MakePrev(list->prev, list->capacity);
    MakeNext(list->next, list->capacity);

    ASSERT_LIST(list);
    return SUCCESS;
}

int_t FindCurrentIndex(List* list, int_t index) {
    assert(list != NULL);
    ASSERT_LIST(list);

    if (index >= list->size) {
        return -1;  //invalid index;
    }

    int_t current_index = list->head;
    for (int_t i = 0; i < index; ++i) {
        current_index = list->next[current_index];
    }

    ASSERT_LIST(list);
    return current_index;
}

ListExitCodes ListPushBack(List* list, type_t value) {
    assert(list != NULL);
    assert(std::isfinite(value) != 0);
    ASSERT_LIST(list);

    if (list->size >= list->capacity) {
        fprintf(stderr, "Cant push oversize\n");
        return PUSH_ERROR;  //can't push
    }

    list->data[list->free] = value;

    if (list->size == 0) {
        list->head = 1;
    }
    else {
        list->next[list->tail] = list->free;
    }

    list->prev[list->free] = list->tail;

    list->tail = list->free;
    list->free = list->next[list->free];

    list->next[list->tail] = 0;

    ++list->size;

    ASSERT_LIST(list);
    return SUCCESS;
}

ListExitCodes ListPushFront(List* list, type_t value) {
    assert(list != NULL);
    assert(std::isfinite(value) != 0);
    ASSERT_LIST(list);

    if (list->size >= list->capacity) {
        fprintf(stderr, "Cant push oversize\n");
        return PUSH_ERROR;
    }

    list->data[list->free] = value;

    if (list->size == 0) {
        list->tail = 1;
        list->head = 1;
    }

    int_t current_index = list->free;
    list->free = list->next[list->free];

    list->prev[current_index] = 0;
    list->prev[list->head] = current_index;

    list->next[current_index] = list->head;

    if (list->size == 0) {   // <= 1
        list->next[list->head] = 0;
    }

    list->head = current_index;

    ++list->size;

    ASSERT_LIST(list);
    return SUCCESS;
}

int_t ListPushBefore(List* list, int_t index_to_push, type_t value) {
    assert(list != NULL);
    assert(std::isfinite(value) != 0);
    ASSERT_LIST(list);

    if (list->size >= list->capacity) {
        fprintf(stderr, "Cant push oversize\n");
        return PUSH_ERROR;  //can't push
    }

    if (index_to_push <= 0 || index_to_push > list->capacity || list->prev[index_to_push] == -1) {
        printf("invalid index to push before: %lld\n", index_to_push);
        return PUSH_ERROR;
    }

    if (index_to_push == list->head) {
        ListPushFront(list, value);
        return SUCCESS;
    }

    list->data[list->free] = value;    //insert and free update
    int_t current_index = list->free;
    list->free = list->next[list->free];

    list->next[current_index] = index_to_push;                 //
    list->next[list->prev[index_to_push]] = current_index;

    list->prev[current_index] = list->prev[index_to_push];
    list->prev[index_to_push] = current_index;

    ASSERT_LIST(list);

    return current_index;
}

int_t ListPushAfter(List* list, int_t index_to_push, type_t value) {
    assert(list != NULL);
    assert(std::isfinite(value) != 0);
    ASSERT_LIST(list);

    if (list->size >= list->capacity) {
        fprintf(stderr, "Cant push oversize\n");
        return PUSH_ERROR;  //can't push
    }

    if (index_to_push <= 0 || index_to_push > list->capacity || list->prev[index_to_push] == -1) {
        printf("invalid index to push before: %lld\n", index_to_push);
        return PUSH_ERROR;
    }

    if (index_to_push == list->tail) {
        ListPushBack(list, value);
        return SUCCESS;
    }

    list->data[list->free] = value;
    int_t current_index = list->free;

    list->free = list->next[list->free];

    list->next[current_index] = list->next[index_to_push];
    list->next[index_to_push] = current_index;

    list->prev[list->next[current_index]] = current_index;
    list->prev[current_index] = index_to_push;

    ASSERT_LIST(list);
    return current_index;
}

ListExitCodes ListPopBack(List* list) {
    assert(list != NULL);
    ASSERT_LIST(list);

    if (list->size <= 0) {
        fprintf(stderr, "Cant pop undersize\n");
        return POP_ERROR;
    }

    list->data[list->tail] = NAN;
    list->next[list->tail] = list->free;
    list->free = list->tail;

    if (list->size == 1) {
        //list->next[list->tail] = 2;
        list->prev[list->tail] = -1;
        list->head = 0;
        list->tail = 0;
        //list->free = 1;
    }
    else {
        //list->next[list->tail] = list->free;
        //list->free = list->tail;

        list->tail = list->prev[list->tail];
        list->prev[list->next[list->tail]] = -1;
        list->next[list->tail] = 0;
    }

    --list->size;

    ASSERT_LIST(list);
    return SUCCESS;
}

ListExitCodes ListPopFront(List* list) {
    assert(list != NULL);
    ASSERT_LIST(list);

    if (list->size <= 0) {
        fprintf(stderr, "Cant pop undersize\n");
        return POP_ERROR;  //can't push
    }

    list->data[list->head] = NAN;

    int_t next_tmp = list->next[list->head];
    list->next[list->head] = list->free;
    list->free = list->head;
    list->prev[list->head] = -1;

    if (list->size == 1) {
        //list->next[list->head] = 2;
        //list->prev[list->head] = -1;
        //list->free = 1;
        list->head = 0;
        list->tail = 0;
    }
    else {
        //int_t next_tmp = list->next[list->head];
        //list->next[list->head] = list->free;
        //list->free = list->head;

        //list->prev[list->head] = -1;
        list->head = next_tmp;
        list->prev[list->head] = 0;
    }

    --list->size;

    ASSERT_LIST(list);
    return SUCCESS;
}

ListExitCodes ListPop(List* list, int_t index_to_pop) {
    assert(list != NULL);
    ASSERT_LIST(list);

    if (list->size <= 0) {
        fprintf(stderr, "Cant pop undersize\n");
        return POP_ERROR;  //can't push
    }

    if (index_to_pop <= 0 || index_to_pop > list->capacity || list->prev[index_to_pop] == -1) {
        printf("invalid index to pop: %lld\n", index_to_pop);
        return POP_ERROR;
    }

    if (index_to_pop == list->head) {
        ListPopFront(list);
        return SUCCESS;
    }

    if (index_to_pop == list->tail) {
        ListPopBack(list);
        return SUCCESS;
    }

    list->next[list->prev[index_to_pop]] = list->next[index_to_pop];
    list->prev[list->next[index_to_pop]] = list->prev[index_to_pop];

    list->data[index_to_pop] = NAN;
    list->prev[index_to_pop] = -1;
    list->next[index_to_pop] = list->free;
    list->free = index_to_pop;

    return SUCCESS;
}

type_t ListGetLogicalElem(List* list, int_t index) {
    assert(list != NULL);
    ASSERT_LIST(list);

    int_t index_to_get = FindCurrentIndex(list, index);

    if (index_to_get == -1) {
        printf("invalid index to get elem\n");
        return GET_ERROR;
    }

    ASSERT_LIST(list);
    return list->data[index_to_get];
}

type_t ListGetPhysElem(List* list, int_t index_to_get) {
    assert(list != NULL);
    ASSERT_LIST(list);


    if (index_to_get <= 0 || index_to_get > list->capacity || list->prev[index_to_get] == -1) {
        printf("invalid index to pop: %lld\n", index_to_get);
        return GET_ERROR;
    }

    ASSERT_LIST(list);
    return list->data[index_to_get];
}

int_t ListGetIndex(List* list, int_t index) {
    assert(list != NULL);
    ASSERT_LIST(list);

    int_t index_to_get = FindCurrentIndex(list, index);

    if (index_to_get == -1) {
        printf("invalid index to get\n");
        return GET_ERROR;
    }

    ASSERT_LIST(list);
    return index_to_get;
}

bool IsEqual(List* lhs, List* rhs) {
    if (lhs->capacity != rhs->capacity || lhs->size != rhs->size || lhs->head != rhs->head ||
        lhs->tail != rhs->tail || lhs->free != rhs->free) {
            return false;
    }
    if (lhs->prev[0] != rhs->prev[0] || lhs->next[0] != rhs->next[0]) {
        return false;
    }

    for (int i = 1 ; i < lhs->capacity + 1; ++i) {
        if (lhs->prev[i] == -1 || rhs->prev[i] == -1) {
            if (lhs->next[i] != rhs->next[i]) {
                return false;
            }
        }
        else if (lhs->data[i] != rhs->data[i] || lhs->next[i] != rhs->next[i]) {
            return false;
        }
    }

    return true;
}

void UnitTest() {
    List test_list = {};
    ListConstruct(&test_list, 10);
    List test_list_correct = {};
    ListConstruct(&test_list_correct, 10);

    test_list_correct.tail = 3;
    test_list_correct.head = 8;
    test_list_correct.free = 5;
    test_list_correct.size = 4;

    test_list_correct.data[1] = 1000;
    test_list_correct.data[2] = -1000;
    test_list_correct.data[3] = 2000;
    test_list_correct.data[4] = -2000;
    test_list_correct.data[7] = 10;
    test_list_correct.data[8] = 11;

    test_list_correct.next[1] = 3;
    test_list_correct.next[2] = 1;
    test_list_correct.next[3] = 0;
    test_list_correct.next[4] = 2;
    test_list_correct.next[5] = 6;
    test_list_correct.next[6] = 9;
    test_list_correct.next[7] = 4;
    test_list_correct.next[8] = 7;
    test_list_correct.next[9] = 10;
    test_list_correct.next[10] = 0;

    test_list_correct.prev[1] = 2;
    test_list_correct.prev[2] = 4;
    test_list_correct.prev[3] = 1;
    test_list_correct.prev[4] = 7;
    test_list_correct.prev[7] = 8;
    test_list_correct.prev[8] = 0;

    for (int i = 1; i < 4; ++i) {
        ListPushBack(&test_list, 1000 * i);
        ListPushFront(&test_list, -1000 * i);
    }

    ListPushAfter(&test_list, 6, 10);
    ListPushAfter(&test_list, 6, 11);

    ListPopFront(&test_list);
    ListPopBack(&test_list);

    if (IsEqual(&test_list, &test_list_correct) == true) {
        fprintf(stderr, "Unit Test 1 done\n");
    }
    else {
        fprintf(stderr, "Unit Test 1 failed, stop program\n");
    }

    ListDestruct(&test_list);
    ListDestruct(&test_list_correct);
    //LIST_DUMP(&test_list, LIST_OK);
}
