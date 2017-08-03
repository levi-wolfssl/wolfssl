#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <unistd.h>

#include <fuzz/fuzzer.h>

int main(int argc, char **argv)
{
    int      i;
    size_t   n_read;
    size_t   sz;
    uint8_t *data = NULL;
    FILE    *file = NULL;

    /* @temporary: long, long name. Get a shorter one. */
    printf("%s:\n", argv[0]);

    /* @temporary: assume every argument is a file */
    for (i = 1; i < argc; ++i) {
        printf("\ttesting %s\n", argv[i]);

        if (!(file = fopen(argv[i], "rb"))) {
            fprintf(stderr, "ERROR: failed to open file\n");
            goto error;
        }

        fseek(file, 0, SEEK_END);
        sz = ftell(file);
        rewind(file);

        if (!(data = malloc(sz))) {
            fprintf(stderr, "ERROR: out of memmory\n");
            goto error;
        }

        if ((n_read = fread(data, 1, sz, file)) != sz) {
            fprintf(stderr, "ERROR: failed to read the whole thing\n");
            goto error;
        }

        fclose(file); file = NULL;

        LLVMFuzzerTestOneInput(data, sz);

        free(data); data = NULL;
    }

    return 0;
error:
    if (data) free(data);
    if (file) fclose(file);
    return 1;
}
