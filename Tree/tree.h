#pragma once
#include "../Log/log.h"

//----------------------------------------------------------------------------------------------------------
#define NODES_NUM_REFRES(tree)   \
{                                \
    NodesNumRefresh_Info(tree);  \
}                   
//DEBUG

//----------------------------------------------------------------------------------------------------------

enum Type
{
    CONST = 1,
    OP    = 2,
    VAR   = 3,
    STR   = 4
};

enum Operators
{
    PLUS  = '+',
    MINUS = '-',
    MUL   = '*',
    DIV   = '/'
};

enum Variables
{
    X = 'x',
    Y = 'y'
};

//const int MAX_LEN_TREE_NAME = 20;


typedef struct dota_t
{
    int type;
    double content;
}
dota_t;


typedef struct Node
{
    Node* parent;
    Node* L_child;
    Node* R_child;

    dota_t* data;
}
Node;


typedef struct tree_info_t
{
    const char* tree_name;
    Node* root;
    size_t nodes_num;
}
tree_info_t;

//----------------------------------------------------------------------------------------------------------

extern tree_info_t* Trees;                 //global array of trees
extern size_t Trees_num;

//------------------------------------------Tree------------------------------------------------------------

Node* NodeCtor(const int type_, const double content_, Node* L_child_, Node* R_child_, Node* parent_);
Node* NodeCtor(Node* parent_);
Node* NodeCopy(Node* node, Node* _parent);
log_error_t NodeDtor(Node** node);
Node* ReadTreeWrap(Node* parent_, FILE* file_ptr, const char* tree_name);
Node* ReadTree(Node* parent_, FILE* file_ptr);
dota_t* ReadData(dota_t* data, FILE* file_ptr);
bool IsOperator(int asci_code);
bool IsVariable(int asci_code);
log_error_t PrintTreeWrap(Node* tree, FILE* file_ptr);
log_error_t PrintTree(Node* node, FILE* file_ptr);
log_error_t PrintData(dota_t* data, FILE* file_ptr);
Node* TreeDtorWrap(Node** tree);
Node* TreeDtor(Node** node, int* node_dtors_num);

//------------------------------------------TreeInfo--------------------------------------------------------

log_error_t AddNewTreeInfo(Node* node, const char* tree_name);
size_t CountNumOfNodes(Node* node, size_t num);
log_error_t TreeOk(Node* tree);
int FindTreeIndex_Info(Node* tree);
Node* GetTreePtr_Info(const char* tree_name_);
size_t GetTreeNodesNum_Info(Node* tree);
log_error_t TreesInfoDtor();

//--------------------------------------------Dot-----------------------------------------------------------

log_error_t DumpTreeDot(Node* tree, const char* file_name);
Node* DumpNodeDot(Node* node, FILE* file_ptr);
log_error_t NodesNumRefresh_Info(Node* tree);
const char* GetColorType(int type);

//----------------------------------------------------------------------------------------------------------
