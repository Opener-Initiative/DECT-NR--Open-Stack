/* sfn_allocator.h -------------------------------------------------------- */
#ifndef SFN_ALLOCATOR_H
#define SFN_ALLOCATOR_H

/* API Original - para uso directo con instancias específicas */

/* Crea la estructura y la deja lista. Devuelve NULL si falla. */
struct sfn_pool *sfn_create(void);

/* Libera todos los recursos internos.  */
void sfn_destroy(struct sfn_pool *p);

/* Devuelve el SFN más pequeño NO usado.
   Retorna -1 si se agotó la memoria.                */
int  sfn_alloc(struct sfn_pool *p);

/* Libera un SFN concreto (si estaba en uso).        */
void sfn_free(struct sfn_pool *p, int idx);

/* API Global - para uso simplificado con pool global */

/* Inicializa el pool global SFN */
int sfn_pool_init(void);

/* Destruye el pool global SFN */
void sfn_pool_deinit(void);

/* Aloca un SFN del pool global */
int sfn_alloc_global(void);

/* Libera un SFN en el pool global */
void sfn_free_global(int idx);

#endif /* SFN_ALLOCATOR_H */
