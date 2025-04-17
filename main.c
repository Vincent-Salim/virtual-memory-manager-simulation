#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MASKEDBITS          0b11111111110000000000000000000000
#define PAGENUMBERBITS      0b00000000001111111111000000000000
#define OFFSETBITS          0b00000000000000000000111111111111
#define COUNTMASKEDBITS     10
#define COUNTPAGENUMBERBITS 10
#define COUNTOFFSETBITS     12
#define FRAMESIZE           4096
#define NOFRAME             256
#define PAGETABLEENTRYSIZE  1024
#define RBIT                1
#define TLBSIZE             32 
#define TLBFRAMENUMBERBITS  0b00000000001111111111000000000000
#define TLBPAGENUMBERBITS   0b00000000000000000000111111111100
#define TLBFRAMEOFFSETBITS  12
#define TLBPAGEOFFSETBITS   2
#define NONESTRING          "none" 
typedef uint32_t u32;
typedef uint64_t u64;

void task1();
void task2();
void task3();
void task4();
int evict_page();


// task 1
u32 logical_address;
u32 page_number;
u32 offset_number;

// task 2
// least significant bit is present/absent bit, the frame number starts from the 13th least significant bit to account for offset of page and frame size
u32 page_table_entry[PAGETABLEENTRYSIZE];
u32 frame_occupied[NOFRAME];
u32 free_frame_idx = 0;
u32 frame_number;
int page_fault;

// task 3
u32 first_in_idx = 0;
bool free_frame = true;
u32 fifo_array[NOFRAME];

// task 4
// TLB stores 0bXXXXXXXXXXAAAAAAAAAABBBBBBBBBBXR where A represents frame bit B page bit and R is r bit
u32 tlb[TLBSIZE];
u32 tlb_lru[TLBSIZE];
// each bit represents wheter the tlb spot for that has been used or not 32 bits for tlb size
u32 tlb_entry_used = 0;
u32 tlb_size = 0;
u32 clock = 0;


int main(int argc, char *argv[]) {
    char *filename = NULL;
    char *task = NULL;

    // initialising arrays
    for (int i = 0; i < PAGETABLEENTRYSIZE; ++i) {
        // smallest bit is the r bit, it is 0 when NOFRAME = 2^n for int n >= 1
        page_table_entry[i] = NOFRAME;
    }

    for (int i = 0; i < NOFRAME; ++i) {
        frame_occupied[i] = 0;
        fifo_array[i] = NOFRAME;
    }

    for (int i = 0; i < TLBSIZE; ++i) {
        tlb[i] = NOFRAME;
        tlb_lru[i] = 0;
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
    else if (!strcmp(task, "task4")) {
        while (fscanf(fptr, "%u", &logical_address) == 1) {
            task4();
        }
    } 

    fclose(fptr);
    return 0;
}

u32 logical_to_page() {
    return (logical_address & PAGENUMBERBITS) >> COUNTOFFSETBITS;
}

u32 logical_to_offset() {
    return (logical_address & OFFSETBITS);
}

u32 offset_frame_num(u32 frame_num) {
    return frame_num << COUNTOFFSETBITS;
}

u32 unoffset_frame_num(u32 offsetted_frame_num) {
    return (offsetted_frame_num & PAGENUMBERBITS) >> COUNTOFFSETBITS;
}

u32 tlb_to_page(int index) {
    return (tlb[index] & TLBPAGENUMBERBITS) >> TLBPAGEOFFSETBITS;
}

u32 tlb_to_frame(int index) {
    return (tlb[index] & TLBFRAMENUMBERBITS) >> TLBFRAMEOFFSETBITS;
}

u32 frame_to_tlb() {
    u32 tlb_offset_page_num = page_number << TLBPAGEOFFSETBITS;
    return (offset_frame_num(frame_number) + tlb_offset_page_num + RBIT);
}

int assign_frame() {
    page_fault = 0;

    if (page_table_entry[page_number] == NOFRAME) {
        page_fault = 1;

        if (free_frame_idx < NOFRAME) {
            frame_number = free_frame_idx;
            fifo_array[free_frame_idx] = page_number; 
            free_frame_idx++;
                // for task 3
        }
        else {
            // this bit is just for task 3
            return -1;
        }
        // smallest bit is present/absent bit
        page_table_entry[page_number] = offset_frame_num(frame_number);
        page_table_entry[page_number] |= 1;
    }
    else {
        frame_number = unoffset_frame_num(page_table_entry[page_number]);
    }
    return 1;
}

int evict_page() {

    // for task 4 to allow the correct print sequence to occur
    if (free_frame_idx < NOFRAME) {
        return -1;
    }

    u32 evicted_page = fifo_array[first_in_idx];
    frame_number = unoffset_frame_num(page_table_entry[evicted_page]);

    printf("evicted-page=%u,freed-frame=%u\n", evicted_page, frame_number);

    page_table_entry[page_number] = offset_frame_num(frame_number);
    page_table_entry[page_number] |= 1;

    // unset frame and fifo_array
    page_table_entry[fifo_array[first_in_idx]] = NOFRAME;
    fifo_array[first_in_idx] = page_number;

    first_in_idx = first_in_idx >= PAGETABLEENTRYSIZE ? 0 : first_in_idx + 1;

    return evicted_page;
}
void task1() {
    page_number = logical_to_page();
    offset_number = logical_to_offset();

    printf("logical-address=%u,page-number=%u,offset=%u\n", logical_address, page_number, offset_number);    
}

void task2() {
    task1();
    assign_frame();
    printf("page-number=%u,page-fault=%u,frame-number=%u,physical-address=%u\n", page_number, page_fault, frame_number, (frame_number * FRAMESIZE) + offset_number);
}
void task3() {
    task1();
    if (assign_frame() < 0) {
        evict_page();
    }
    printf("page-number=%u,page-fault=%u,frame-number=%u,physical-address=%u\n", page_number, page_fault, frame_number, (frame_number * FRAMESIZE) + offset_number);
}


void task4() {
    task1();
    bool tlb_hit = false;
    clock++;
    for (int i = 0; i < TLBSIZE; ++i) {
        if (tlb_to_page(i) == page_number) {
            tlb_hit = true;
            tlb_lru[i] = clock;
            frame_number = tlb_to_frame(tlb_hit);
            printf("tlb-hit=%u,page-number=%u,frame=%u,physical-address=%u\n", tlb_hit, page_number, frame_number, (frame_number * FRAMESIZE) + logical_to_offset());
            break;
        }
    }
    if (!tlb_hit) {
        printf("tlb-hit=%u,page-number=%u,frame=%s,physical-address=%s\n", tlb_hit, page_number, NONESTRING, NONESTRING);
        u32 evicted_page;
        if (assign_frame() < 0) {
            if ((evicted_page = evict_page()) > 0) {
                fprintf(stderr, "evicting %u\n", evicted_page);
                for (int i = 0; i < TLBSIZE; ++i) {
                    if (tlb_to_page(i) == evicted_page) {
                        tlb_size--;
                        printf("tlb-flush=%u,tlb-size=%u\n", tlb_to_page(i), tlb_size);
                        tlb[i] = NOFRAME;
                        break;
                    }
                }
            }
        }
        // update tlb according to LRU (least recently used)
        if (tlb_size == TLBSIZE) {
            u32 lru_idx = 0;
            // find least recently used clock always increments and is assigned on creation and on use
            for (int i = 0; i < TLBSIZE; ++i) {
                lru_idx = tlb_lru[lru_idx] < tlb_lru[i] ? lru_idx : i;
            }
            printf("tlb-remove=%u,tlb-add=%u\n", tlb_to_page(lru_idx), page_number);
            tlb[lru_idx] = frame_to_tlb();
            tlb_lru[lru_idx] = clock;
        }
        else {
            for (int i = 0; i < TLBSIZE; ++i) {
                if (tlb[i] == NOFRAME) {
                    tlb[i] = frame_to_tlb();
                    tlb_lru[i] = clock;
                    break;
                }
            }
            tlb_size++;
            printf("tlb-remove=%s,tlb-add=%u\n", NONESTRING, page_number);
        }
        printf("page-number=%u,page-fault=%u,frame-number=%u,physical-address=%u\n", page_number, page_fault, frame_number, (frame_number * FRAMESIZE) + offset_number);
    }
}