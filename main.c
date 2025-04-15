#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define u32 int
#define MASKEDBITS     0b11111111110000000000000000000000
#define PAGENUMBERBITS 0b00000000001111111111000000000000
#define OFFSETBITS     0b00000000000000000000111111111111
#define COUNTMASKEDBITS     10
#define COUNTPAGENUMBERBITS 10
#define COUNTOFFSETBITS     12
void task1(const char *filename);

int main(int argc, char *argv[]) {
    char *filename;
    char *task;

    for (int i = 0; i < argc; ++i) {
        if (!strcmp(argv[i], "-f")) {
            filename = argv[++i];

        }
        else if (!strcmp(argv[i], "-t")) {
            task = argv[++i];
        }
    }
    fprintf(stderr,"filename: %s\n", filename);
    fprintf(stderr, "task: %s\n", task);
    task1(filename);

    return 0;
}

void task1(const char *filename) {

    fprintf(stderr, "Opening: %s\n", filename);
    FILE *fptr = fopen(filename, "r");

    if (fptr == NULL) {
        printf("No file\n");
        exit(1);
    }

    u32 logical_address;
    while (fscanf(fptr, "%d", &logical_address) == 1) {
        u32 page_number = (logical_address & PAGENUMBERBITS) >> COUNTOFFSETBITS;
        u32 offset = (logical_address & OFFSETBITS);
    
        printf("logical-address=%d,page-number=%d,offset=%d\n", logical_address, page_number, offset);    
    }

}