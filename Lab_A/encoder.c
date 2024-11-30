#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//my code helped by copilot
//part 1
FILE* infile;
FILE* outfile;
bool debug_mode = 1;
//part 2
char* encoding_key = "0"; // Default encoding key
int key_len = 0;
int key_index = 0;

int encode_add(int key_value, char c) { //part 2
    return c + key_value;
}

int encode_subtract(int key_value, char c) { //part 2
    return c - key_value;
}

int (*encode_mode)(int key_value, char c) = encode_add; //part 2

//my code helped by copilot
char encode(char c) { //part 2
    int key_value = encoding_key[key_index] - '0';

    if (c >= 'A' && c <= 'Z') {
        c = (encode_mode(key_value, c - 'A') + 26) % 26 + 'A';
    } else if (c >= 'a' && c <= 'z') {
        c = (encode_mode(key_value, c - 'a') + 26) % 26 + 'a';
    } else if (c >= '0' && c <= '9') {
        c = (encode_mode(key_value, c - '0') + 10) % 10 + '0';
    }

    key_index = (key_index + 1) % key_len; // Cycle through the key
    return c;
}

//my code helped by copilot
void handle_arguments(int argc, char **argv) {
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-D") == 0){ //part 1
            debug_mode = 0;
        } else if (strcmp(argv[i], "+D") == 0){ //part 1
            debug_mode = 1;
        } else if (strncmp(argv[i], "+E", 2) == 0){ //part 2
            encoding_key = argv[i] + 2;
            key_index = 0;
            encode_mode = encode_add;
        } else if (strncmp(argv[i], "-E", 2) == 0){ //part 2
            encoding_key = argv[i] + 2;
            key_index = 0;
            encode_mode = encode_subtract;
        } else if (strncmp(argv[i], "-i", 2) == 0) { //part 3
            infile = fopen(argv[i] + 2, "r");
            if (infile == NULL) {
                fprintf(stderr, "Error opening input file: %s\n", argv[i] + 2);
                exit(1);
            }
        } else if (strncmp(argv[i], "-o", 2) == 0) { //part 3
            outfile = fopen(argv[i] + 2, "w");
            if (outfile == NULL) {
                fprintf(stderr, "Error opening output file: %s\n", argv[i] + 2);
                exit(1);
            }
        }
    }
    if (debug_mode == 1) { //part 1
        for (int i = 0; i < argc; i++){
            fprintf(stderr, "Argument %d: %s\n", i, argv[i]);
        }
    }
}

//my code
void init_files() { //part 1
    infile = stdin;
    outfile = stdout;
}

//my code helped by copilot
void close_files() { //part 1
    if (infile != stdin) {
        fclose(infile);
    }
    if (outfile != stdout) {
        fclose(outfile);
    } else {
        fflush(stdout);
    }
}

//my code helped by copilot
int main(int argc, char **argv) {
    //part 1
    init_files();
    handle_arguments(argc, argv);

    //part 2
    while (encoding_key[key_len]) {
        key_len++;
    }

    //part 1
    int c;
    while ((c = fgetc(infile)) != EOF) {
        char encoded_char = encode((char)c);
        fputc(encoded_char, outfile);
    }

    if (feof(infile)) {
        fprintf(stderr, "End of file reached.\n");
    } else {
        fprintf(stderr, "Error reading file.\n");
    }

    close_files();
    return 0;
}