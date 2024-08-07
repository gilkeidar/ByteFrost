//	Binary Search Tree (Insert + traversal)
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
LDR R0, #30		//	Set next_node = root address
SMA R0, #11     //	.
//	Make the root node
LMA R0, #1		//	R0 = elements[0]
PUSH R0			//	add_new_node(elements[0])
JSR :add_new_node_func	//	.
LDR R1, #2		//	i = 2
LDR R2, #11		//	R2 = num_elements (11)
:do_while
OUT R1, I		//	Print i
OUT #0x10, A	//	Print '\n'
PUSH R1			//	Save R1
PUSH R2			//	Save R2
LMR R3, R1		//	R3 = element
PUSH R3			//	Push element
LDR R3, #30		//	R3 = root address (30)
PUSH R3			//	Push root
JSR :insert_func	//	insert(tree, elements[i]);
//LDR R3, #30		//	R3 = root address (30)
//PUSH R3			//	Push root address
//JSR :traverse_func	//	traverse(root);
POP R2			//	Restore R2
POP R1			//	Restore R1
INC R1			//	i++
COMP R1, R2		//	while (i < num_elements);
BMI :do_while	//	.
LDR R0, #30		//	R0 = root address (30)
PUSH R0			//	Push root address
JSR :traverse_func	//	traverse(root);
OUT #0x10, A		//	printf("\n");
BRK

:add_new_node_func
POP R0			//	R0 = return address
POP R1			//	R1 = element
LMA R2, #11		//	R2 = next_node
PUSH R2			//  push next_node
PUSH R0			// 	push return address
MOV R0, R2		//	R0 = next_node
SMR R1, R0		//	next_node->value = element
INC R0			//	R0 = &(next_node->left)
LDR R1, #0		//	R1 = NULL
SMR R1, R0		//	next_node->left = NULL
INC R0			//	R0 = &(next_node->right)
SMR R1, R0		//	next_node->right = NULL
INC R0			//	R0 = next_node + 1
SMA R0, #11		//	next_node = R0 (next_node + 1)
RTS				//	return;

:insert_func
POP R0					//	R0 = return address
POP R1					//	R1 = root (parameter variable)
POP R2					//	R2 = element (parameter variable)
PUSH R0					//	push return address
MOV R0, R1				// 	R0 = root
MOV R1, R2				//	R1 = element
LMR R2, R0				//	R2 = root->value
COMP R2, R1				//	comp root->value - element
BPL :insert_else_if		//	if (root->value < element) {
LDR R2, #2				//	R2 = 2
ADD R2, R0, R2			//	R2 = &(root->right)
LMR R3, R2				//	R3 = root->right
TST R3					//	if (root->right == NULL)
BNE :insert_if_else		//	.
PUSH R0					//	Save R0
PUSH R1					//	Save R1
PUSH R2					//	Save R2
PUSH R1					//	add_new_node(element);
JSR :add_new_node_func	//	.
POP R3					//	R3 = next_node (return value)
POP R2					//	Restore R2
POP R1					//	Restore R1
POP R0					// 	Restore R0
SMR R3, R2				//	root->right = next_node (return value)
RTS
:insert_if_else
PUSH R1					//	Push R1 (element)
LMR R2, R2				//	R2 = root->right
PUSH R2					//	Push R2 (root->right)
JSR :insert_func		//	insert(root->right, element);
RTS
:insert_else_if
BNE :insert_else_if_if			// if (root->value == element) return;
RTS
:insert_else_if_if
LDR R2, #1				//	R2 = 1
ADD R2, R0, R2			//	R2 = &(root->left)
LMR R3, R2				// 	R3 = root->left
TST R3					//	if (root->left == NULL)
BNE :insert_else_if_else	//	.
PUSH R0					//	Save R0
PUSH R1					//	Save R1
PUSH R2					//	Save R2
PUSH R1					//	add_new_node(element);
JSR :add_new_node_func	//	.
POP R3					//	R3 = next_node (return value)
POP R2					//	Restore R2
POP R1					//	Restore R1
POP R0					// 	Restore R0
SMR R3, R2				//	root->left = next_node (return value)
RTS
:insert_else_if_else
PUSH R1					//	Push R1 (element)
LMR R2, R2				//	R2 = root->left
PUSH R2					//	Push R2 (root->left)
JSR :insert_func		//	insert(root->left, element);
RTS

:traverse_func
POP	R0					//	R0 = return address
POP R1					//	R1 = root (parameter variable)
PUSH R0					// 	Push return address
TST R1					// 	if (R1 (root) == NULL)
BNE :traverse_func_core	//	.
RTS						//	return;
:traverse_func_core
INC R1					//	R1 = &(root->left)
LMR R0, R1				//	R0 = root->left
PUSH R1					//	Save R1
PUSH R0					//	traverse(root->left);
JSR :traverse_func		//	.
POP R1					//	Restore R1
DEC R1					//	R1 = &(root->value)
LMR R0, R1				//	R0 = root->value
OUT R0, I				//	printf("%d", root->value);
OUT #0x20, A			//	printf(" ");
ADD R1, #2				//	R1 = &(root->right)
LMR R0, R1				// 	R0 = root->right
PUSH R0					//	traverse(root->right);
JSR :traverse_func		//	.
RTS
