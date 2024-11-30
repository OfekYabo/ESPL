#include <stdlib.h>
#include <stdio.h>
#include <string.h>


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


// int main(int argc, char **argv){
//   /* TODO: Test your code */
//   int base_len = 5;
//   char arr1[base_len];

//   char* arr2 = map(arr1, base_len, my_get);
//   printf("arr2: %s\n", arr2);
//   char* arr3 = map(arr2, base_len, dprt);
//   char* arr4 = map(arr3, base_len, xprt);
//   char* arr5 = map(arr4, base_len, encrypt);

//   free(arr2);
//   free(arr3);
//   free(arr4);
//   free(arr5);

//   return 0;
// }