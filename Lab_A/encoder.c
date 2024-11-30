#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//my code helped by copilot
FILE* infile;
FILE* outfile;
bool debug_mode = 1;
char* encoding_key = "0"; // Default encoding key
int key_len = 0;
int key_index = 0;

int encode_add(int key_value, char c) {
    return c + key_value;
}

int encode_subtract(int key_value, char c) {
    return c - key_value;
}

int (*encode_mode)(int key_value, char c) = encode_add;

char encode(char c) {
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

void handle_arguments(int argc, char **argv) {
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-D") == 0){
            debug_mode = 0;
        } else if (strcmp(argv[i], "+D") == 0){
            debug_mode = 1;
        } else if (strncmp(argv[i], "+E", 2) == 0){
            encoding_key = argv[i] + 2;
            key_len = strlen(encoding_key);
            key_index = 0;
            encode_mode = encode_add;
        } else if (strncmp(argv[i], "-E", 2) == 0){
            encoding_key = argv[i] + 2;
            key_len = strlen(encoding_key);
            key_index = 0;
            encode_mode = encode_subtract;
        }
    }
    if (debug_mode == 1) {
        for (int i = 0; i < argc; i++){
            fprintf(stderr, "Argument %d: %s\n", i, argv[i]);
        }
    }
}

void init_files() {
    infile = stdin;
    outfile = stdout;
}

void close_files() {
    if (outfile != stdout) {
        fclose(outfile);
    } else {
        fflush(stdout);
    }
}

int main(int argc, char **argv) {
    init_files();
    handle_arguments(argc, argv);

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