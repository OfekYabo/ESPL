#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//my code helped by copilot
FILE* infile;
FILE* outfile;
bool debug_mode = 1;

char encode(char c) {
    return c;
}

void handle_arguments(int argc, char **argv) {
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "-D") == 0){
            debug_mode = 0;
        } else if (strcmp(argv[i], "+D") == 0){
            debug_mode = 1;
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