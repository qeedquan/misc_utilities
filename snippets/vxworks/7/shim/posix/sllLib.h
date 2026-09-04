#ifndef _SLL_LIB_H_
#define _SLL_LIB_H_

typedef struct slnode {
	struct slnode *next;
} SL_NODE;

typedef struct {
	SL_NODE *head;
	SL_NODE *tail;
} SL_LIST;

#define SLL_NEXT(node) (((SL_NODE *)node)->next)
#define SLL_EMPTY(list) (((SL_LIST *)list)->head == NULL)

#endif
