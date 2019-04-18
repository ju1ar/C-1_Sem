#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "../Log/log.h"  // Specify  -D __DEBUG__  if you want to write to log file

//----------------------------------------------------------------------------------------------------------

tree_info_t* Trees  = NULL;                 //global array of trees
size_t Trees_num = 0;

const int MAX_LEN_STR = 128;

#define dbl2ptr(dbl) (*((char**)&(dbl)))


#define TREE_OK(tree, command)                              \
{                                                           \
    if((log_err = TreeOk(tree)) != LOG_NOERR)               \
    {                                                       \
        DumpTreeDot(tree, "bad_tree.dot");                  \
        command;                                            \
    }                                                       \
}
//system in debug version
//Dump with name?

//-------------------------------------------------------------------------------------------------------------
log_error_t AddNewTreeInfo(Node* tree, const char* tree_name_)
{
    CHECK_LOG(_PTR, tree, return LOG_BAD_PTR);

    Trees_num++;
    Trees = (tree_info_t*)realloc(Trees, Trees_num * sizeof(*Trees));
    CHECK_LOG(_PTR, Trees, return LOG_BAD_PTR);

    Trees[Trees_num - 1].tree_name = tree_name_;
    Trees[Trees_num - 1].root      = tree;
    Trees[Trees_num - 1].nodes_num = CountNumOfNodes(tree, 0);

    return LOG_NOERR;
}

size_t CountNumOfNodes(Node* node, size_t num)
{
    if(!node) return num;
    num++;

    num = CountNumOfNodes(node->L_child, num);
    num = CountNumOfNodes(node->R_child, num);

    return num;
}

Node* NodeCtor(Node* parent_)
{
    Node* new_node = (Node*)calloc(1, sizeof(*new_node));
    CHECK_LOG(_PTR, new_node, return NULL);

    new_node->parent  = parent_;
    new_node->L_child = NULL;
    new_node->R_child = NULL;

    new_node->data = (dota_t*)calloc(1, sizeof(*new_node->data));
    CHECK_LOG(_PTR, new_node->data, return NULL);

    new_node->data->type    = 0;
    new_node->data->content = 0;

    return new_node;
}

Node* NodeCtor(const int type_, const double content_, Node* L_child_, Node* R_child_, Node* parent_)
{
    Node* new_node = (Node*)calloc(1, sizeof(*new_node));
    CHECK_LOG(_PTR, new_node, return NULL);

    new_node->parent  = parent_;
    new_node->L_child = L_child_;
    new_node->R_child = R_child_;
    
    new_node->data = (dota_t*)calloc(1, sizeof(*new_node->data));
    CHECK_LOG(_PTR, new_node->data, return NULL);

    new_node->data->type    = type_;
    new_node->data->content = content_;

    return new_node;
}

Node* NodeCopy(Node* node, Node* parent_)
{
    if(!node) return NULL;

    Node* copy = NodeCtor(node->data->type, node->data->content, NULL, NULL, parent_);

    copy->L_child = NodeCopy(node->L_child, copy);
    copy->R_child = NodeCopy(node->R_child, copy);

    return copy;
}

/*
Node* NodeCopy(Node* node)
{
    if(!node) return NULL;

    return NodeCtor(node->data->type, node->data->content, NodeCopy(node->L_child), 
                                                           NodeCopy(node->R_child), node->parent);
}
*/
log_error_t NodeDtor(Node** node)
{
    CHECK_LOG(_PTR, *node, return LOG_BAD_PTR);

/*
    if((*node)->L_child || (*node)->R_child)
    {
        log_err = LOG_BAD_NODE_DTOR;
        PrintLog("Please,  DON'T KILL ME!!! I have children!!!, line: %d", __LINE__);
        return log_err;
    }
*/
    
    if((*node)->data->type == STR)
    {
        free(dbl2ptr((*node)->data->content));
        dbl2ptr((*node)->data->content);
    }

    free((*node)->data);
    (*node)->data = NULL;

    free(*node);
    *node = NULL;

    return LOG_NOERR;
}

Node* ReadTreeWrap(Node* parent_, FILE* file_ptr, const char* tree_name)
{
    CHECK_LOG(_FILE, file_ptr, return NULL);

    Node* tree = ReadTree(parent_, file_ptr);
    CHECK_LOG(_PTR, tree, return NULL);

    AddNewTreeInfo(tree, tree_name);
    LOG_OK(return NULL);

    return tree;
}

#define IF_FIND_LEFT_BRACKET(_node_)                       \
{                                                          \
   if (fgetc(file_ptr) == '(')  is_left_brace = 1;         \
       else                     is_left_brace = 0;         \
                                                           \
   fseek(file_ptr, -1, SEEK_CUR);                          \
                                                           \
   if(is_left_brace)                                       \
       _node_ = ReadTree(node, file_ptr);                  \
}                                               

Node* ReadTree(Node* parent_, FILE* file_ptr)
{
    bool is_left_brace = 0;

    fscanf(file_ptr, "(");
    Node* node = NodeCtor(parent_);

    IF_FIND_LEFT_BRACKET(node->L_child);

    node->data = ReadData(node->data, file_ptr);

    IF_FIND_LEFT_BRACKET(node->R_child);

    fscanf(file_ptr, ")");

    return node;
}

#undef IF_FIND_LEFT_BRACKET

dota_t* ReadData(dota_t* data, FILE* file_ptr)
{
    CHECK_LOG(_FILE, file_ptr, return NULL);
    CHECK_LOG(_PTR,  data,     return NULL);
    
    double value = 0;

    if(fscanf(file_ptr, "%lg", &value) == 1)             //case CONST:
    {
        data->type    = CONST;
        data->content = value;

        return data;
    }

//  char str[MAX_LEN_STR] = {};                             //aaaaaaaaaa
    char* str = (char*)calloc(MAX_LEN_STR ,sizeof(*str));   //читстить

    fscanf(file_ptr, "%[^)(]", str);
    size_t length = strlen(str);
    int asci_code = (int)str[0];

    if(length > 1)                                      //case STR:
    {
        data->type             = STR;
        dbl2ptr(data->content) = str;
    }
    else if (length == 1)
    {
        if(IsOperator(asci_code))                       //case OP:
        {
            data->type    = OP;
            data->content = asci_code;
        }
        else if(IsVariable(asci_code))                  //case VAR:
        {
            data->type    = VAR;
            data->content = asci_code;
        }
    }
    else PrintLog("Error: length = %d!, line: %d", length, __LINE__);

    return data;
}

bool IsOperator(int asci_code)
{
    if(asci_code == PLUS || asci_code == MINUS ||
       asci_code == MUL  || asci_code == DIV     )
        return true;

    return false;
}


bool IsVariable(int asci_code)
{
    if(asci_code == X || asci_code == Y)
        return true;

    return false;
}

log_error_t PrintTreeWrap(Node* tree, FILE* file_ptr)
{
    CHECK_LOG(_PTR,  tree,     return LOG_BAD_PTR);
    CHECK_LOG(_FILE, file_ptr, return LOG_BAD_FILE);
    TREE_OK(tree, );

    PrintTree(tree, file_ptr);

    TREE_OK(tree, );
    return LOG_NOERR;
}

log_error_t PrintTree(Node* node, FILE* file_ptr)
{
    if(!node) return LOG_NOERR;
    fprintf(file_ptr, "(");
     
    PrintTree(node->L_child, file_ptr);

    PrintData(node->data, file_ptr);

    if(node->R_child == NULL && node->L_child == NULL)
    {
        fprintf(file_ptr, ")");
        return LOG_NOERR;
    }

    PrintTree(node->R_child, file_ptr);
   
    fprintf(file_ptr, ")");

    return LOG_NOERR;
}


log_error_t PrintData(dota_t* data, FILE* file_ptr)
{
    CHECK_LOG(_PTR,  data,     return LOG_BAD_PTR);
    CHECK_LOG(_FILE, file_ptr, return LOG_BAD_FILE);

    switch(data->type)
    {
        case CONST:
            fprintf(file_ptr, "%lg", data->content);
            break;
        
        case OP:
            fprintf(file_ptr, "%c", char(data->content));
            break;

        case VAR:
            fprintf(file_ptr, "%c", char(data->content));
            break;

        case STR:
            fprintf(file_ptr, "%s", dbl2ptr(data->content));
            break;

        default:
            PrintLog("Unexpected type = %d!, line = %d", data->type, __LINE__); 
    }
    
    return LOG_NOERR;
}

log_error_t TreeOk(Node* tree)
{
    CHECK_LOG(_PTR,  tree, return LOG_BAD_PTR);                      

    int index = FindTreeIndex_Info(tree);
    if(index == -1)                                                 //допилить 
    {
        log_err = LOG_NOTREE;
        PrintLog("Tree isn't planted!, line: %d", __LINE__);
        return log_err;                                              
    }

    int nodes_num_ = CountNumOfNodes(tree, 0);
    if(Trees[index].nodes_num != nodes_num_)
    {
        log_err = LOG_BAD_NODES_NUM;
        PrintLog("WHERE ME CHILDREN?!? / WHOSE ARE CHILDREN???, line: %d", __LINE__);  // писать кол-во
        return log_err;                                              
    }

    return LOG_NOERR;
}

int FindTreeIndex_Info(Node* tree)
{
    CHECK_LOG(_PTR,  tree, return LOG_BAD_PTR);

    for(size_t index = 0; index < Trees_num; index++)
        if (Trees[index].root == tree) return index;

    return -1;
}

Node* GetTreePtr_Info(const char* tree_name_)
{
    CHECK_LOG(_PTR,  tree_name_, return NULL);

    for(size_t i = 0; i < Trees_num; i++)
        if (!strcmp(Trees[i].tree_name, tree_name_)) return Trees[i].root;

    return NULL;
}

size_t GetTreeNodesNum_Info(Node* tree)
{
    CHECK_LOG(_PTR,  tree, return LOG_BAD_PTR);

    int index = FindTreeIndex_Info(tree);

    if(index != -1) 
        return Trees[index].nodes_num;

    return -1;
}

Node* TreeDtorWrap(Node** tree)
{
    CHECK_LOG(_PTR, tree, return NULL);
    TREE_OK(*tree, return NULL);

    int nodes_num = GetTreeNodesNum_Info(*tree);
    int node_dtors_num = 0;

    *tree = TreeDtor(tree, &node_dtors_num);
    if(*tree) PrintLog("Tree is alive!, line: %d", __LINE__);

    if(nodes_num != node_dtors_num) 
        PrintLog("Memory leak: Some nodes are alive!,            "
                 "node_dtors_num =  %d, nodes_num = %d, line = %d",
                  node_dtors_num, nodes_num, __LINE__);

    return *tree;                        
}

Node* TreeDtor(Node** node, int* node_dtors_num)
{
    if(!(*node)) return NULL;

    if(!(*node)->R_child && !(*node)->L_child)
    {
        NodeDtor(node);
        if ((*node) == NULL) (*node_dtors_num)++;
        return *node;
    }

    TreeDtor(&((*node)->L_child), node_dtors_num);
    TreeDtor(&((*node)->R_child), node_dtors_num);

    NodeDtor(node);
    if ((*node) == NULL) (*node_dtors_num)++;

    return *node;
}

log_error_t TreesInfoDtor()
{
    free(Trees);
    Trees = NULL;                       // забвл освободить дерерво?

    return LOG_NOERR;
}
// удaлить некоторые инфу о деревьях

#define PRINT_DATA_DOT(_node_)                                                          \
{                                                                                       \
    fprintf(file_ptr, "  ptr%p[label=\"", _node_);                                      \
    PrintData(node->data, file_ptr);                                                    \
    fprintf(file_ptr, "\", fillcolor=\"%s\"];\n", GetColorType(_node_->data->type));    \
}


/* AKINATOR
#define IF_NOT_NULL_PRINT_DATA_DOT(_node_, answer)                                      \
{                                                                                       \
    if(_node_)                                                                          \
    {                                                                                   \
        fprintf(file_ptr, "  ptr%p->ptr%p[label=\"%s\"];\n", node, _node_, answer);     \
                                                                                        \
        DumpNodeDot(_node_, file_ptr);                                                  \
    }                                                                                   \
}


Node* DumpNodeDot(Node* node, FILE* file_ptr)
{
    if(!node) return NULL;
    
    PRINT_DATA_DOT(node);

    IF_NOT_NULL_PRINT_DATA_DOT(node->L_child, "да" );
    IF_NOT_NULL_PRINT_DATA_DOT(node->R_child, "нет");

    return node;
}
*/

#define IF_NOT_NULL_PRINT_DATA_DOT(_node_)                                      \
{                                                                                       \
    if(_node_)                                                                          \
    {                                                                                   \
        fprintf(file_ptr, "  ptr%p->ptr%p;\n", node, _node_);     \
                                                                                        \
        DumpNodeDot(_node_, file_ptr);                                                  \
    }                                                                                   \
}


Node* DumpNodeDot(Node* node, FILE* file_ptr)
{
    if(!node) return NULL;
    
    PRINT_DATA_DOT(node);

    IF_NOT_NULL_PRINT_DATA_DOT(node->L_child);
    IF_NOT_NULL_PRINT_DATA_DOT(node->R_child);

    return node;
}

#undef PRINT_DATA_DOT
#undef IF_NOT_NULL_PRINT_DATA_DOT

log_error_t DumpTreeDot(Node* tree, const char* file_name)
{
    CHECK_LOG(_PTR, tree,      return LOG_BAD_PTR);
    CHECK_LOG(_PTR, file_name, return LOG_BAD_PTR);

    FILE* file_ptr = fopen(file_name, "w");
    CHECK_LOG(_FILE, file_ptr, return LOG_BAD_FILE);
    
    fprintf(file_ptr,  "digraph G                                                             \n"
                       "{                                                                     \n"
                       "  rankdir=HR;                                                         \n"
                       "  graph [bgcolor=\"pink\"];                                           \n"
                       "  node[shape = \"ellipse\", color=\"red\",fontsize=14,                \n"
                       "  style=\"filled\"];                                                 \n"
                       "  edge[color=\"black\",fontcolor=\"blue\",fontsize=14,style=\"bold\"];\n");

    DumpNodeDot(tree, file_ptr);

    fprintf(file_ptr,  "}");
    fclose(file_ptr);

    return LOG_NOERR;
}
    
//переделать да и нет и добавить цвета на разные узлы

log_error_t NodesNumRefresh_Info(Node* tree)
{
    CHECK_LOG(_PTR, tree, return LOG_BAD_PTR);

    int index = FindTreeIndex_Info(tree);

    if(index == -1)                                                 //допилить 
    {
        log_err = LOG_NOTREE;
        PrintLog("Tree isn't planted!, line: %d", __LINE__);
        return log_err;                                              
    }

    Trees[index].nodes_num = CountNumOfNodes(tree, 0);

    return LOG_NOERR;
}

const char* GetColorType(int type)
{

    switch(type)
    {
        case CONST:
            return "palegreen"; 

        case OP:
            return "lightsalmon3";

        case VAR:
            return "gray78";

        case STR:
            return "palegreen";

        default:
            PrintLog("There's no such type = %d, line = %d", type, __LINE__);
            return NULL;
    }
}
