/*  Labels struct and methods   */

/*  Keep track of labels with binary search tree    */
typedef struct Label {
    char * label;           //  Label name
    int instruction;        //  Instruction (position of label)
    struct Label * left;    //  Left child
    struct Label * right;   //  Right child
} Label;