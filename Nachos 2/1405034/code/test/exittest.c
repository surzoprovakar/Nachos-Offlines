/* 
 * exittest.c
 *
 *	Simple program to test exit system call.
 */

#include "syscall.h"

int
main()
{
    printf("Exit Test\n");
    Exit(123);
}
