#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MASKEDBITS          0b11111111110000000000000000000000
#define PAGENUMBERBITS      0b00000000001111111111000000000000
#define OFFSETBITS          0b00000000000000000000111111111111
#define COUNTMASKEDBITS     10
#define COUNTPAGENUMBERBITS 10
#define COUNTOFFSETBITS     12
#define FRAMESIZE           1024
#define NOFRAME             256
void task1(const char *filename);

typedef uint32_t u32;
typedef uint64_t u64;

u32 logical_address;
u32 page_number;
u32 offset_number;

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
    
    FILE *fptr = fopen(filename, "r");
    if (fptr == NULL) {
        printf("No file\n");
        exit(1);
    }
    
    while (fscanf(fptr, "%u", &logical_address) == 1) {
        if (strcmp(task, "task1")) {
            task1(&logical_address);
        } 
        // else if (strcmp(task, "task2")) {
        //     task2(logical_address);
        // } 
        // else if (strcmp(task, "task3")) {
        //     task3(logical_address);
        // } 
        // else if (strcmp(task, "task4")) {
        //     task4(logical_address);
        // } 
    }

    fclose(fptr);
    return 0;
}

int logical_to_page(u32 *logical_address) {
    return (*logical_address & PAGENUMBERBITS) >> COUNTOFFSETBITS;
}

int logical_to_offset(u32 *logical_address) {
    return (*logical_address & OFFSETBITS);
}

void task1(u32 *logical_address) {
     {
        page_number = logical_to_page(logical_address);
        offset_number = logical_to_offset(logical_address);
    
        printf("logical-address=%d,page-number=%d,offset=%d\n", logical_address, page_number, offset);    
    }
}

void task2(FILE *fptr) {

    int page_table_entry[FRAMESIZE];
    // 64 * 4 = framesize
    u64 frames[4];
    int free_frame;

    if (!page_table_entry[page_number]) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 64; ++j) {
                if (frames[i] & 0 << j) {
                    free_frame = i * 64 + j;
                }
            }
        }    
        page_table_entry[page_number] = free_frame;
    }
}