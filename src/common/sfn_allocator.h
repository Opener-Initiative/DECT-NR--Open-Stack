/* sfn_allocator.h -------------------------------------------------------- */
#ifndef SFN_ALLOCATOR_H
#define SFN_ALLOCATOR_H

/* Original API - for direct use with specific instances */

/* Create the structure and return it. Returns NULL on failure. */
struct sfn_pool *sfn_create(void);

/* Free all internal resources.  */
void sfn_destroy(struct sfn_pool *p);

/* Return the smallest unused SFN.
   Returns -1 on memory exhaustion.                */
int  sfn_alloc(struct sfn_pool *p);

/* Free a specific SFN (if it was in use).        */
void sfn_free(struct sfn_pool *p, int idx);

/* Global API - for simplified use with the global pool */

/* Initialize the global SFN pool */
int sfn_pool_init(void);

/* Destroy the global SFN pool */
void sfn_pool_deinit(void);

/* Allocate an SFN from the global pool */
int sfn_alloc_global(void);

/* Free an SFN in the global pool */
void sfn_free_global(int idx);

#endif /* SFN_ALLOCATOR_H */
