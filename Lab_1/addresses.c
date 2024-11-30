#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int addr5; //global variable unitialized
int addr6; //global variable unitialized

int foo() //text section
{
    return -1;
}
void point_at(void *p);
void foo1();
char g = 'g'; //global variable intialized
void foo2();

int secondary(int x)
{
    int addr2;
    int addr3;
    char *yos = "ree";
    int *addr4 = (int *)(malloc(50));
	int iarray[3];
    float farray[3];
    double darray[3];
    char carray[3]; 
	int iarray2[] = {1,2,3};
    char carray2[] = {'a','b','c'};
    int* iarray2Ptr;
    char* carray2Ptr; 
    
	printf("- &addr2: %p\n", &addr2);
    printf("- &addr3: %p\n", &addr3);
    printf("- foo: %p\n", &foo);
    printf("- &addr5: %p\n", &addr5);
	printf("Print distances:\n");
    point_at(&addr5);

    printf("Print more addresses:\n");
    printf("- &addr6: %p\n", &addr6);
    printf("- yos: %p\n", yos);
    printf("- gg: %p\n", &g);
    printf("- addr4: %p\n", addr4); //value of the pointer (heeap address) 
    printf("- &addr4: %p\n", &addr4); //address of the pointer (stack address)

    printf("- &foo1: %p\n", &foo1);
    printf("- &foo1: %p\n", &foo2);
    
    printf("Print another distance:\n");
    printf("- &foo2 - &foo1: %ld\n", (long) (&foo2 - &foo1));

   
    printf("Arrays Mem Layout (T1b):\n");

    /* task 1 b here */

    //my code helped by copilot
    printf("iarray: %p\n", (void*)iarray);
    printf("&iarray: %p\n", (void*)(&iarray));

    printf("iarray+1: %p\n", (void*)(iarray + 1));
    printf("&iarray+1: %p\n", (void*) (&iarray + 1));

    printf("farray: %p\n", (void*)farray);
    printf("farray+1: %p\n", (void*)(farray + 1));

    printf("darray: %p\n", (void*)darray);
    printf("darray: %p\n", (void*)(darray + 1));

    printf("carray: %p\n", (void*)carray);
    printf("carray: %p\n", (void*)(carray + 1));
    
    printf("Pointers and arrays (T1d): \n");

    /* task 1 d here */

    //my code helped by copilot
    iarray2Ptr = iarray2;
    carray2Ptr = carray2; 
    
    for (int i = 0; i < 3; i++) {
        printf("iarray2[%d]: %d\n", i, *(iarray2Ptr + i));
        printf("carray2[%d]: %c\n", i, *(carray2Ptr + i));
    }

    void* p;
    printf("Uninitialized pointer p: %p\n", p);

}

int main(int argc, char **argv)
{ 

    //basic code
    printf("Print function argument addresses:\n");

    printf("- &argc %p\n", &argc);
    printf("- argv %p\n", argv);
    printf("- &argv %p\n", &argv);
	
	secondary(0);
    
    printf("Command line arg addresses (T1e):\n");
    /* task 1 e here */

    // Print argv[0] and its address
    printf("argv[0]: %s\n", argv[0]);
    printf("Address of argv[0]: %p\n", (void*)argv[0]);

    // Print addresses and contents of command-line arguments
    for (int i = 0; i < argc; i++) {
        printf("argv[%d]: %p -> %s -> %p\n", i, (void*)argv[i], argv[i], (void*)(argv+i));
    }
    
    return 0;
}

void point_at(void *p)
{
    int local;
    static int addr0 = 2;
    static int addr1;

    long dist1 = (size_t)&addr6 - (size_t)p; //bss - bss unitialized
    long dist2 = (size_t)&local - (size_t)p; //stack - bss
    long dist3 = (size_t)&foo - (size_t)p; //text - bss

    printf("- dist1: (size_t)&addr6 - (size_t)p: %ld\n", dist1);
    printf("- dist2: (size_t)&local - (size_t)p: %ld\n", dist2);
    printf("- dist3: (size_t)&foo - (size_t)p:  %ld\n", dist3);
    
    printf("Check long type mem size (T1a):\n");
    /* part of task 1 a here */

    //additional code by me and copilot
    printf("%u\n", sizeof(long));
    //printf(sizeof(long));

    printf("- addr0: %p\n", &addr0);
    printf("- addr1: %p\n", &addr1);
}

void foo1()
{
    printf("foo1\n");
}

void foo2()
{
    printf("foo2\n");
}