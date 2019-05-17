#include "pager.h"

Pager* pager_open(const char* filename) {
	//1. Read/Write mode, Create file if it does not exist. 
	//2. Give user write & read permission 
	int fd = open(filename, O_RDWR|O_CREAT, S_IWUSR|S_IRUSR);
	if (fd == -1) {
		printf("Error: Unable to open file %s\n",filename);
		exit(EXIT_FAILURE);
	}

	// seek to the end and get the file length
	off_t file_length = lseek(fd, 0, SEEK_END);

	Pager* pager = (Pager *)malloc(sizeof(Pager));
	if(!pager){
		printf("Error: Unable to malloc pager.\n");
		exit(EXIT_FAILURE);
	}
	pager->file_descriptor = fd;
	pager->file_length = file_length;
	pager->num_pages = (file_length / PAGE_SIZE);
	if (file_length % PAGE_SIZE != 0) {
		printf("Error: Db file is not a whole number of pages. Corrupt file.\n");
		exit(EXIT_FAILURE);
	}

	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		pager->pages[i] = NULL;
	}
	return pager;
}

//when data base close, flush pages to files for persist.
void pager_flush(Pager* pager, uint32_t page_num) {
	if (pager->pages[page_num] == NULL) {
		printf("Error: Tried to flush null page\n");
		exit(EXIT_FAILURE);
	}

	off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
	if (offset == -1) {
		printf("Error: error seeking: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	//ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], size);
	ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], PAGE_SIZE);
	if (bytes_written == -1) {
		printf("Error: error writing: %d\n", errno);
		exit(EXIT_FAILURE);
	}
}

//return the page you want (page_num)
//	will load intent in file if have
void* pager_get_page(Pager* pager, uint32_t page_num) {
	if (page_num >= TABLE_MAX_PAGES) {
		printf("Error: Tried to fetch page number out of bounds. %d > %d\n", page_num, TABLE_MAX_PAGES);
		exit(EXIT_FAILURE);
	}
	if (pager->pages[page_num] == NULL) {	// Cache miss. Allocate memory and load from file.
		//todo: to avoid frequently malloc and free, we can implement a cache that pre-malloced many pages ?
		void* page = malloc(PAGE_SIZE);
		uint32_t num_pages = pager->file_length / PAGE_SIZE;	//already have (num_pages) pages in this file

		if (pager->file_length % PAGE_SIZE) {	//
			printf("Error: Corrupt file.\n");
			num_pages += 1;
		}
		if (page_num <= num_pages) {
			lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
			ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);	//when file is empty, bytes_read seems tobe 0
			if (bytes_read == -1) {
				printf("Error: error reading file: %d\n", errno);
				exit(EXIT_FAILURE);
			}
		}
		pager->pages[page_num] = page;

		if (page_num >= pager->num_pages) {
			pager->num_pages = page_num + 1;
		}
	}
	return pager->pages[page_num];
}

/*
 todo:
 	Until we start recycling free pages, new pages will always
 	go onto the end of the database file
 fang:
 	later, here can implement a list and hold those pages which are set unused.
 	and get unused will pick a node from that list.
*/
uint32_t pager_get_unused_page_num(Pager* pager) {
	return pager->num_pages;
}

