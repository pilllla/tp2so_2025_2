#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef unsigned long addr_t;

struct pagemid_e_t;
struct pagetbl_e_t;

/**
 * 3 level page table
 */

/**
 *  Page directory entry (top-level)
 */
typedef struct pagedir_e_3_t
{
    int present;
    struct pagetbl_e2_3_t *middle_table;
};

/**
 * Page table entry (2nd-level)
 */
typedef struct pagetbl_e2_3_t
{
    int present;
    struct pagetbl_e_3_t *page_table;
};

/**
 * Page table entry (3rd-level)
 */
typedef struct pagetbl_e1_3_t
{
    int present;
    int referenced;
    int modified;
    unsigned int frame_num;
    off_t swap_off;
};

/**
 * 2 level page table
 */

typedef struct pagedir_e_2_t
{
    int present;
    struct pagetbl_e1_t *page_table;
};

/**
 * Frame
 */
typedef struct frame
{
    int in_use;
    int ref_p;
    int altered_p;
    time_t last_acess;
    pagetbl_e_t *pte; // Pointer back to pagetable entry (pte) for page stored in this frame
    addr_t vaddr;     // Store vaddr for use in opt algorithm
};

/**
 * Inverted page table
 */
typedef struct page_tbl_inv
{
    /* data */
};

#endif