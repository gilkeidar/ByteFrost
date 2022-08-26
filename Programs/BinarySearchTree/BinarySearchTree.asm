//  Binary Search Tree (insertion + sort by in-order traversal)
:main_func      //  main()
//  Store elements in RAM
//  Store elements 3, 12, 4, and 0
LDR R0, #3
LDR R1, #12
LDR R2, #4
LDR R3, #0
SMA R0, #1
SMA R1, #2
SMA R2, #3
SMA R3, #4
//  Store elements 36, 235, 89, 54
LDR R0, #36
LDR R1, #235
LDR R2, #89
LDR R3, #54
SMA R0, #5
SMA R1, #6
SMA R2, #7
SMA R3, #8
//  Store elements 21, 100
LDR R2, #21
LDR R3, #100
SMA R2, #9
SMA R3, #10
//  Create Binary Search Tree
LDR R0, #1      //  Set num_nodes to 1 (used to index new node placements as a replacement for malloc)
SMA R0, #11     //  Store num_nodes value in address 11
//  main_func continues here

:insert_func    //  insert()
//  Get element argument from the stack
POP R0              //  R0 contains return address of calling function
POP R1              //  R1 contains element argument
PUSH R0             //  Restore return address on the stack
LDR R0, #12         //  R0 = (current_node = root)
:while_cond         //  while (current_node != NULL)
SUB R0, #0          //  .
BEQ :end_while      //  .
LMR R2, R0          //  R2 = current_node->value
SUB R3, R2, R1      //  R3 = current_node->value - element
BPL :while_else_if  //  if (current_node->value < element)
ADD R3, R0, #2      //  R3 = current_node->right
LMR R3, R3          //  .
SUB R3, #0          //  if (current_node->right == NULL)
BNE :while_if_if    //  .  (add new node)
LMA R2, #11         //  R2 = num_nodes
LDR R3, #3          //  R3 = 3
PUSH R0             //  Save current node value on stack
LDR R0, #12         //  R0 = 12
:find_new_node
SUB R2, #0          //  while (R2 != 0)
BEQ :found_address  //  .
ADD R0, R0, R3      //  R0 += 3
DEC R2              //  R2--
JMP :find_new_node
:found_address      //  R0 contains address of new node
SMR R1, R0          //  new_node->value = element
INC R0              //  R0++
SMR R2, R0          //  new_node->left = NULL (0) (since R2 should be 0 right now)
INC R0              //  R0++
SMR R2, R0          //  new_node->right = NULL (0)
SUB R0, R0, #2      //  R0 = new_node
POP R3              //  R3 = current_node
ADD R3, R0, #2      //  R3 = current_node->right
SMR R0, R3          //  current_node->right = new_node
JMP :while_cond
:while_if_if
