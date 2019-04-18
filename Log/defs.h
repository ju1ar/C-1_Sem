#define DEFS_H
#ifdef DEFS_H

DEF_ERR(NOERR,         0, "No error detected.")
DEF_ERR(BAD_PTR,       1, "Pointer is NULL!")
DEF_ERR(BAD_FILE,      2, "File wasn't opened.")
DEF_ERR(BAD_ALLOC,     3, "Memory wasn't allocated.")
DEF_ERR(NOTREE,        4, "There isn't such tree!")
DEF_ERR(BAD_NODES_NUM, 5, "Tree has wrong number of nodes!")
DEF_ERR(BAD_NODE_DTOR, 6, "Destroying tree with alive child.")

#endif
