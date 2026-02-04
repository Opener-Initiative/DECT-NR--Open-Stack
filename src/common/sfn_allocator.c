/* sfn_allocator.c -------------------------------------------------------- */
#include "sfn_allocator.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Global SFN pool instance */
static struct sfn_pool *global_sfn_pool = NULL;

/* ---------------- min-heap implementation ---------------- */
static void heap_swap(int *a, int *b) { int t=*a; *a=*b; *b=t; }

static void heap_up(int *h, int i)
{
    while (i && h[i] < h[(i-1)/2]) {
        heap_swap(&h[i], &h[(i-1)/2]);
        i = (i-1)/2;
    }
}
static void heap_down(int *h, int n, int i)
{
    while (1) {
        int l = 2*i+1, r = 2*i+2, m = i;
        if (l < n && h[l] < h[m]) m = l;
        if (r < n && h[r] < h[m]) m = r;
        if (m == i) break;
        heap_swap(&h[i], &h[m]);
        i = m;
    }
}

/* ---------------- data structure ------------------------ */
struct sfn_pool {
    int  next;          /* next SFN to allocate */
    int *free_heap;     /* min-heap of freed SFNs */
    int  free_cap;      /* array capacity */
    int  free_len;      /* number of valid elements */
};

#define INIT_CAP 16

static int heap_push(struct sfn_pool *p, int val)
{
    if (p->free_len == p->free_cap) {
        int nc = p->free_cap ? p->free_cap*2 : INIT_CAP;
        int *tmp = realloc(p->free_heap, nc*sizeof(int));
        if (!tmp) return -1;
        p->free_heap = tmp;
        p->free_cap  = nc;
    }
    p->free_heap[p->free_len] = val;
    heap_up(p->free_heap, p->free_len);
    p->free_len++;
    return 0;
}

static int heap_pop(struct sfn_pool *p, int *out)
{
    if (!p->free_len) return -1;
    *out = p->free_heap[0];
    p->free_len--;
    if (p->free_len) {
        p->free_heap[0] = p->free_heap[p->free_len];
        heap_down(p->free_heap, p->free_len, 0);
    }
    return 0;
}

/* ---------------- API original ------------------------ */
struct sfn_pool *sfn_create(void)
{
    struct sfn_pool *p = calloc(1, sizeof *p);
    return p;
}

void sfn_destroy(struct sfn_pool *p)
{
    if (p) {
        free(p->free_heap);
        free(p);
    }
}

int sfn_alloc(struct sfn_pool *p)
{
    if (!p) return -1;
    
    int ret;
    if (heap_pop(p, &ret) == 0) {
        return ret;
    }
    return p->next++;
}

void sfn_free(struct sfn_pool *p, int idx)
{
    if (idx < 0 || idx >= p->next)       /* out of range: ignore */
        return;

    /* Avoid duplicates: if it's the most recently allocated (still unassigned),
       simply decrement next; otherwise push it into the heap.       */
    if (idx == p->next - 1) {
        p->next--;
    } else {
        heap_push(p, idx);               /* O(log N)               */
    }
}

/* ---------------- API Global ------------------------ */

/* Initialize the global SFN pool */
int sfn_pool_init(void)
{
    if (global_sfn_pool) {
        return 0; // Already initialized
    }
    
    global_sfn_pool = sfn_create();
    return (global_sfn_pool != NULL) ? 0 : -1;
}

/* Deinitialize the global SFN pool */
void sfn_pool_deinit(void)
{
    if (global_sfn_pool) {
        sfn_destroy(global_sfn_pool);
        global_sfn_pool = NULL;
    }
}

/* Allocate an SFN from the global pool */
int sfn_alloc_global(void)
{
    if (!global_sfn_pool) {
        return -1; // Pool not initialized
    }
    return sfn_alloc(global_sfn_pool);
}

/* Free an SFN in the global pool */
void sfn_free_global(int idx)
{
    if (global_sfn_pool) {
        sfn_free(global_sfn_pool, idx);
    }
}
