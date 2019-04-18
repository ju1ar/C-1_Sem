#pragma once
#include "../Tree/tree.h"

Node* GetG(char* expression);
Node* GetE();
Node* GetT();
Node* GetP();
Node* GetId();
Node* GetN();
Node* RecDescentReadTree(FILE* file_ptr, const char* tree_name);
