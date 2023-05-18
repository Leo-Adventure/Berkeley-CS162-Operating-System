/*
 * mm_alloc.h
 *
 * A clone of the interface documented in "man 3 malloc".
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>

void *mm_malloc(size_t size);
void *mm_realloc(void *ptr, size_t size);
void mm_free(void *ptr);

struct metadata
{
    struct metadata* prev;
    struct metadata* next;
    bool free;
    int size;
    char contents[0];  //zero-length array
};