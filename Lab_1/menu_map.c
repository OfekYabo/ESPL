#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define Input_Len 1
#define Array_len 5

//Task 2 functions
char* map(char *array, int array_length, char (*f) (char)) {
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  /* TODO: Complete during task 2.a */
  //my code
  for (int i = 0; i<array_length; i++) {
    mapped_array[i] = f(array[i]);
  }
  return mapped_array;
}

//task 2.b- my code
/* Ignores c, reads and returns a character from stdin using fgetc. */
char my_get(char c) {
  return fgetc(stdin);
}

/* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns the value of c unchanged. */
char cprt(char c) {
  if (c >= 0x20 && c <= 0x7e) {
    printf("%c\n", c);
  } else {
    printf(".\n");
  }
  return c;
}

/* Gets a char c and returns its encrypted form by adding 1 to its value. If c is not between 0x1F and 0x7E it is returned unchanged */
char encrypt(char c) {
  char encrypt = c;
  if (c >= 0x1f && c <= 0x7e) {
    encrypt++;
  }
  return encrypt;
}

/* Gets a char c and returns its decrypted form by reducing 1 from its value. If c is not between 0x21 and 0x7F it is returned unchanged */
//TODO: maybe 0x20 and not 0x21
char decrypt(char c) {
  char dencrypt = c;
  if (c >= 0x21 && c <= 0x7f) {
    dencrypt--;
  }
  return dencrypt;
}

/* xprt prints the value of c in a hexadecimal representation followed by a new line, and returns c unchanged. */
//my code + copilot help
char xprt(char c) {
  printf("%x\n", (unsigned char)c);
  return c;
}

/* dprt prints the value of c in a decimal representation followed by a new line, and returns c unchanged. */
char dprt(char c) {
  printf("%i\n", (unsigned char)c);
  return c;
}

//Task 3
typedef struct {
    char* name;
    char (*fun)(char);
} fun_desc;

int main(int argc, char **argv) {
    char array[Array_len] = "";
    char* carray = array;
    fun_desc Menu_Funcs[] = {
        {"my_get", my_get},
        {"cprt", cprt},
        {"encrypt", encrypt},
        {"decrypt", decrypt},
        {"xprt", xprt},
        {"dprt", dprt},
        {NULL, NULL}
    };

    int bound = 0;
    while (Menu_Funcs[bound].name != NULL) {
        bound++;
    }

    //my code, advised  by copilot to improve it

    char option_str[Input_Len + 2]; //input + newline + null terminator
    while (1) {
        printf("Select operation from the following menu by number (CTR+D to Exit) :\n");
        for (int i = 0; i < bound; i++) {
            printf("%i) %s\n", i, Menu_Funcs[i].name);
        }
        printf("Option : ");

        if (fgets(option_str, sizeof(option_str), stdin) == NULL) {
            break;
        }

        char *endptr;
        int option = strtol(option_str, &endptr, 10);
        if (endptr == option_str || *endptr != '\n') {
            printf("Invalid option\n");
            continue;
        };

        if (option >= 0 && option < bound) {
            printf("Whithin bounds\n");
            carray = map(carray, Array_len, Menu_Funcs[option].fun);
        } else {
            printf("Not within bounds\n");
        }
    }
}