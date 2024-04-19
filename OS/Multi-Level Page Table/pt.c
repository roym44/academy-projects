#include <stdio.h>
#include "os.h"

/* MACROS */
#define OFFSET_BITS (12)
#define SIGN_EXTEND_BITS (7)
#define VPN_BITS (64 - OFFSET_BITS - SIGN_EXTEND_BITS)
#define SYMBOL_BITS (9)
#define LEVELS (VPN_BITS / SYMBOL_BITS)

/* FUNCTIONS */

// Retrieves the VA given its PPN.
uint64_t* ppn_to_va(uint64_t ppn) 
{
    uint64_t pa = 0;
    pa = ppn << OFFSET_BITS; // shift left by 12 bits to get the PA
    return (uint64_t*)phys_to_virt(pa);
}

// Calculates the index of the PTE in the page table given the VPN and the level.
int get_pte_index(uint64_t vpn, int level)
{
    int shift = 0;
    // for level 0, we need to shift by 36 bits, for level 1 by 27 bits, etc.
    shift = (VPN_BITS - SYMBOL_BITS) - (level * SYMBOL_BITS);
    return (vpn >> shift) & 0x1FF; // get only the symbol itself
}

// Creates / destroys virtual memory mappings in a page table
void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn) 
{
    int i = 0;
    int valid = 0;
    int pte_index = 0;
    uint64_t* pt_node = NULL;
    uint64_t next_node_ppn = 0;
    uint64_t pte = 0;
    
    pt_node = ppn_to_va(pt); // get the page table root
    // walk through the page table
    for (i = 0; i < LEVELS; i++)
    {
        pte_index = get_pte_index(vpn, i); // use vpn to get the index of the pte
        pte = pt_node[pte_index]; // a pt node is treated as an array of uint64_t
        valid = pte & 0x1; // get the valid bit

        // get the next pt node
        if (valid == 0) // invalid - we need to allocate a new frame
        {
            next_node_ppn = alloc_page_frame(); // allocate a new frame
            pt_node[pte_index] = (next_node_ppn << OFFSET_BITS) + 0x1; // set the PTE to next node + valid bit
        }
        else // valid - extract the PPN from the PTE as usual
            next_node_ppn = pte >> OFFSET_BITS; // PPN field is used to store pointer to child
        
        // last level - map/unmap the given ppn
        if (i == LEVELS - 1)
        {
            if (ppn == NO_MAPPING)
                pt_node[pte_index] = 0; // destroy the mapping
            else
                pt_node[pte_index] = (ppn << OFFSET_BITS) + 0x1; // set the valid bit
            return;
        }
        pt_node = ppn_to_va(next_node_ppn); 
    }
}


// Returns the PPN that the given VPN is mapped to, or NO_MAPPING if no mapping exists.
uint64_t page_table_query(uint64_t pt, uint64_t vpn) 
{
    int i = 0;
    int valid = 0;
    int pte_index = 0;
    uint64_t* pt_node = NULL;
    uint64_t next_node_ppn = 0;
    uint64_t pte = 0;
    
    pt_node = ppn_to_va(pt); // get the page table root
    // walk through the page table
    for (i = 0; i < LEVELS; i++)
    {
        pte_index = get_pte_index(vpn, i); // use vpn to get the index of the pte
        pte = pt_node[pte_index]; // a pt node is treated as an array of uint64_t
        valid = pte & 0x1; // get the valid bit
        
        if (valid == 0)
            return NO_MAPPING;

        // get the next pt node
        next_node_ppn = pte >> OFFSET_BITS; // PPN field is used to store pointer to child
        // only before the last level, update to next node
        if (i < LEVELS - 1)
            pt_node = ppn_to_va(next_node_ppn); 
    }
    // we reached the last level - simply return the ppn
    return next_node_ppn;
}
