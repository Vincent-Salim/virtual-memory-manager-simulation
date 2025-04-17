#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MASKEDBITS          0b11111111110000000000000000000000
#define PAGENUMBERBITS      0b00000000001111111111000000000000
#define OFFSETBITS          0b00000000000000000000111111111111
#define COUNTMASKEDBITS     10
#define COUNTPAGENUMBERBITS 10
#define COUNTOFFSETBITS     12
#define FRAMESIZE           4096
#define NOFRAME             256
#define PAGETABLEENTRYSIZE  1024
#define RBIT                0b1

typedef uint32_t u32;
typedef uint64_t u64;

void task1();
void task2();
void task3();

void evict_page(u32 assign_frame_num);


// task 1
u32 logical_address;
u32 page_number;
u32 offset_number;

// task 2
// least significant bit is present/absent bit, the frame number starts from the 13th least significant bit to account for offset of page and frame size
u32 page_table_entry[PAGETABLEENTRYSIZE];
u32 frame_occupied[NOFRAME];
int page_fault;
// task 3
u32 first_in_idx = 0;
u32 no_free_frame = 0;
u32 fifo_array[NOFRAME];
int main(int argc, char *argv[]) {
    char *filename = NULL;
    char *task = NULL;

    for (int i = 0; i < PAGETABLEENTRYSIZE; ++i) {
        // smallest bit is 0 when NOFRAME = 2^n for int n >= 1
        page_table_entry[i] = NOFRAME;
    }

    for (int i = 0; i < NOFRAME; ++i) {
        frame_occupied[i] = 0;
        fifo_array[i] = NOFRAME;
    }

    for (int i = 0; i < argc; ++i) {
        if (!strcmp(argv[i], "-f")) {
            filename = argv[++i];

        }
        else if (!strcmp(argv[i], "-t")) {
            task = argv[++i];
        }
    }

    if (!task || !filename) {
        fprintf(stderr, "no task or no fptr\n");
        exit(1);
    }

    fprintf(stderr,"filename: %s\n", filename);
    fprintf(stderr, "task: %s\n", task);
    
    FILE *fptr = fopen(filename, "r");
    if (fptr == NULL) {
        printf("No file\n");
        exit(1);
    }

    if (!strcmp(task, "task1")) {
        while (fscanf(fptr, "%u", &logical_address) == 1) {
            task1();
        }
    } 
    else if (!strcmp(task, "task2")) {
        while (fscanf(fptr, "%u", &logical_address) == 1) {
            task2();
        }
    } 
    else if (!strcmp(task, "task3")) {
        while (fscanf(fptr, "%u", &logical_address) == 1) {
            task3();
        }
    } 
    // else if (!strcmp(task, "task4")) {
    //     task4(logical_address);
    // } 

    fclose(fptr);
    return 0;
}

u32 logical_to_page() {
    return (logical_address & PAGENUMBERBITS) >> COUNTOFFSETBITS;
}

u32 logical_to_offset() {
    return (logical_address & OFFSETBITS);
}

u32 offset_frame_num(int frame_number) {
    return frame_number << COUNTOFFSETBITS;
}

u32 unoffset_frame_num(int offsetted_frame_number) {
    return (offsetted_frame_number & PAGENUMBERBITS) >> COUNTOFFSETBITS;
}

void assign_frame() {
    u32 assign_frame_num = PAGETABLEENTRYSIZE;
    page_fault = 0;
    if (page_table_entry[page_number] == NOFRAME) {
        page_fault = 1;

        if (!no_free_frame) {
            for (int i = 0; i < NOFRAME; ++i) {
                if (!frame_occupied[i]) {
                    assign_frame_num = i;
                    frame_occupied[i] = 1;
                    fifo_array[i] = page_number; 
                    break;
                }
                // for task 3
                if (i == NOFRAME - 1) {
                    no_free_frame = 1;
                    evict_page(assign_frame_num);
                }
            }
        }
        else {
            // this bit is just for task 3
            evict_page(assign_frame_num);
        }
        // smallest bit is present/absent bit
        page_table_entry[page_number] = offset_frame_num(assign_frame_num);
        page_table_entry[page_number] |= 1;
    }
}
// replace_true just for debugging
void evict_page(u32 assign_frame_num) {
    assign_frame_num = first_in_idx;
    printf("evicted-page=%u,freed-frame=%u\n", fifo[first_in_idx], unoffset_frame_num(page_table_entry[first_in_idx]));

    first_in_idx = first_in_idx >= PAGETABLEENTRYSIZE ? 0 : ++first_in_idx;
}
void task1() {
    page_number = logical_to_page();
    offset_number = logical_to_offset();

    printf("logical-address=%u,page-number=%u,offset=%u\n", logical_address, page_number, offset_number);    
}

void task2() {
    task1();
    assign_frame();
    int frame_number = unoffset_frame_num(page_table_entry[page_number]);
    printf("page-number=%u,page-fault=%u,frame-number=%u,physical-address=%u\n", page_number, page_fault, frame_number, (frame_number * FRAMESIZE) + offset_number);
}

void task3() {
    task1();
    assign_frame();
    int frame_number = unoffset_frame_num(page_table_entry[page_number]);
    printf("page-number=%u,page-fault=%u,frame-number=%u,physical-address=%u\n", page_number, page_fault, frame_number, (frame_number * FRAMESIZE) + offset_number);
}