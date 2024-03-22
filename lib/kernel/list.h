#ifndef __LIB_KERNEL_LIST_H
#define __LIB_KERNEL_LIST_H
#define offset(struct_type, member_name) (int)(&(((struct_type*)0) -> member_name))
#define elem2entry(struct_type, member_name, member_ptr) (struct_type*)((int)member_ptr - offset(struct_type, member_name))
#include "global.h"

struct list_elem {
    struct list_elem* prev;
    struct list_elem* next;
};

struct list {
    struct list_elem head;
    struct list_elem tail;
};

typedef bool (function)(struct list_elem*, int arg);

void list_init(struct list* list);
void list_insert_before(struct list_elem* before, struct list_elem* elem);
void list_push(struct list* plist, struct list_elem* elem);
void list_append(struct list* plist, struct list_elem* elem);
void list_remove(struct list_elem* pelem);
struct list_elem* list_pop(struct list* plist);
bool elem_find(struct list* plist, struct list_elem* obj_elem);
struct list_elem* list_traversal(struct list* plist, function func, int arg);
uint32_t list_len(struct list* plist);
bool list_empty(struct list* plist);

// void list_init(struct list* list);
// void list_insert(struct list* link,struct list* new_link);
// void list_push(struct list* list, struct list* new_link);
// void list_append(struct list* list,struct list_elem* new_link);
// void list_remove(struct list* link);
// struct list* list_pop(struct list* list);
// int list_find(struct list* list, struct list* link);
// int list_empty(struct list* list);
// uint32_t list_len(struct list* list);
// struct list* list_traversal(struct list* list,function func,int arg);

#endif