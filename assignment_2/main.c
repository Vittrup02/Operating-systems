
// C program for linked list implementation of stack
#include "io.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "mm.h"
// A structure to represent a stack
struct StackNode {
    int data;
    struct StackNode* next;
};
//Laver en ny node, og allokere dynamisk memory til den.
struct StackNode* newNode(int data)
{
    struct StackNode* stackNode =
            (struct StackNode*)
                    simple_malloc(sizeof(struct StackNode));
    stackNode->data = data;
    stackNode->next = NULL;
    return stackNode;
}

int isEmpty(struct StackNode* root)
{
    return !root;
}

void push(struct StackNode** root, int data)
{
    struct StackNode* stackNode = newNode(data);
    stackNode->next = *root;
    *root = stackNode;
}

int pop(struct StackNode** root)
{
    if (isEmpty(*root))
        return -1;
    struct StackNode* temp = *root;
    *root = (*root)->next;
    int popped = temp->data;
    simple_free(temp);
    return popped;
}

int peek(struct StackNode* root)
{
    if (isEmpty(root))
        return -1;
    return root->data;
}

int main() {
    int count = 0;
    struct StackNode* root = NULL;
    int result;
    int stack_size = 0;
    int *stack_elements = NULL;

    // Process the input until 'q' is received or any invalid character is encountered
    do {
        result = read_char();
        if (result == 'a') {
            push(&root, count);
            count++;
        } else if (result == 'b') {
            count++;
        } else if (result == 'c') {
            pop(&root);
            count++;
        } else {
            break;  // Terminate on any character other than 'a', 'b', or 'c'
        }
    } while (result != 'q');  // Loop until 'q' is found

    // Determine the size of the stack
    struct StackNode* temp = root;
    while (temp != NULL) {
        stack_size++;
        temp = temp->next;
    }

    // Allocate memory for stack elements
    stack_elements = (int*)simple_malloc(stack_size * sizeof(int));
    if (!stack_elements) {
        return -1; // Handle memory allocation failure
    }

    // Store stack elements in the array
    int index = 0;
    while (root != NULL) {
        stack_elements[index++] = peek(root);
        pop(&root);
    }

    // Print elements in reverse order
    for (int i = stack_size - 1; i >= 0; i--) {
        write_int(stack_elements[i]);
        if (i > 0) {
            write_char(',');
        } else {
            write_char(';');
        }
    }
    write_char('\n');

    // Clean up
    simple_free(stack_elements);
    return 0;
}


