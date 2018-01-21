#ifndef __CR_INC_DUMP_H__
#define __CR_INC_DUMP_H__
#include "asm/dump.h"

extern struct pstree_item *global_item;

extern int arch_set_thread_regs(struct pstree_item *item, bool with_threads);

#endif
