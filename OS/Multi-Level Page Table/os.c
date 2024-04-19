#define _GNU_SOURCE

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/mman.h>

#include "os.h"

/* 2^20 pages ought to be enough for anybody */
#define NPAGES	(1024*1024)

static char* pages[NPAGES];

uint64_t alloc_page_frame(void)
{
	static uint64_t nalloc;
	uint64_t ppn;
	void* va;

	if (nalloc == NPAGES)
		errx(1, "out of physical memory");

	/* OS memory management isn't really this simple */
	ppn = nalloc;
	nalloc++;

	va = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if (va == MAP_FAILED)
		err(1, "mmap failed");

	pages[ppn] = va;
	return ppn + 0xbaaaaaad;
}

void* phys_to_virt(uint64_t phys_addr)
{
	uint64_t ppn = (phys_addr >> 12) - 0xbaaaaaad;
	uint64_t off = phys_addr & 0xfff;
	char* va = NULL;

	if (ppn < NPAGES)
		va = pages[ppn] + off;

	return va;
}

void test_NO_MAPPING_same_pte()
{
	uint64_t pt = alloc_page_frame();
	uint64_t vpn1 = 0xcafecafeee1;
	uint64_t vpn2 = 0xcafecafeee2;
	uint64_t ppn1 = 0xf001;
	uint64_t ppn2 = 0xf002;

	printf("test_NO_MAPPING_same_pte\n");
	
	// map two different vpn to two different ppn
	page_table_update(pt, vpn1, ppn1);
	page_table_update(pt, vpn2, ppn2);
	assert(page_table_query(pt, vpn1) == ppn1);
	assert(page_table_query(pt, vpn2) == ppn2);
	// unmap vpn1
	page_table_update(pt, vpn1, NO_MAPPING);
	// make sure vpn2 is still mapped
	assert(page_table_query(pt, vpn1) == NO_MAPPING);
	assert(page_table_query(pt, vpn2) == ppn2);
}

void test_mapping_to_ppn_0()
{
	uint64_t pt = alloc_page_frame();
	uint64_t vpn = 0xcafecafeee1;
	uint64_t ppn = 0x0;

	printf("test_mapping_to_ppn_0\n");
	
	// map to ppn 0
	page_table_update(pt, vpn, ppn);
	assert(page_table_query(pt, vpn) == ppn);
}

void test_override_mapping()
{
	uint64_t pt = alloc_page_frame();
	uint64_t vpn = 0xcafecafeee1;
	uint64_t ppn1 = 0xf001;
	uint64_t ppn2 = 0xf002;

	printf("test_override_mapping\n");
	
	// map vpn to ppn1
	page_table_update(pt, vpn, ppn1);
	assert(page_table_query(pt, vpn) == ppn1);
	
	// map vpn to ppn2
	page_table_update(pt, vpn, ppn2);
	assert(page_table_query(pt, vpn) == ppn2);
}

void Test_unmapped_from_each_level()
{
	uint64_t pt = alloc_page_frame();
	uint64_t vpn = 0xcafecafeee1;
	uint64_t ppn = 0x0;

	printf("Test_unmapped_from_each_level\n");

	// map to ppn
	page_table_update(pt, vpn, ppn);
	assert(page_table_query(pt, vpn) == ppn);

	// unmap from each level
	// unmap_from_each_level(pt, vpn);

	// test that we get back no_mapping
	// assert(page_table_query(pt, vpn) == NO_MAPPING);
	
}


int main(int argc, char **argv)
{
	printf("HW1 main\n");
	uint64_t pt = alloc_page_frame();

	printf("basic_functionality_fails\n");
	assert(page_table_query(pt, 0xcafecafeeee) == NO_MAPPING);
	assert(page_table_query(pt, 0xfffecafeeee) == NO_MAPPING);
	assert(page_table_query(pt, 0xcafecafeeff) == NO_MAPPING);
	page_table_update(pt, 0xcafecafeeee, 0xf00d);
	assert(page_table_query(pt, 0xcafecafeeee) == 0xf00d);
	assert(page_table_query(pt, 0xfffecafeeee) == NO_MAPPING);
	assert(page_table_query(pt, 0xcafecafeeff) == NO_MAPPING);
	page_table_update(pt, 0xcafecafeeee, NO_MAPPING);
	assert(page_table_query(pt, 0xcafecafeeee) == NO_MAPPING);
	assert(page_table_query(pt, 0xfffecafeeee) == NO_MAPPING);
	assert(page_table_query(pt, 0xcafecafeeff) == NO_MAPPING);

	test_NO_MAPPING_same_pte();
	test_mapping_to_ppn_0();
	test_override_mapping();
	Test_unmapped_from_each_level();

	printf("Finished all tests\n");
	return 0;
}


// void unmap_from_each_level(uint64_t pt, uint64_t vpn)
// {
//     int i = 0;
//     int pte_index = 0;
//     uint64_t* pt_node = NULL;
//     uint64_t next_node_ppn = 0;
//     uint64_t pte = 0;
    
//     pt_node = ppn_to_va(pt); // get the page table root
//     // walk through the page table
//     for (i = 0; i < LEVELS; i++)
//     {
//         pte_index = get_pte_index(vpn, i); // use vpn to get the index of the pte
//         pte = pt_node[pte_index]; // a pt node is treated as an array of uint64_t
//         pt_node[pte_index] = pte - 0x1; // unvalidate the pte 
//         return;  
//         // get the next pt node
//         next_node_ppn = pte >> OFFSET_BITS; // PPN field is used to store pointer to child
//         // only before the last level, update to next node
//         if (i < LEVELS - 1)
//             pt_node = ppn_to_va(next_node_ppn);
//     }
// }