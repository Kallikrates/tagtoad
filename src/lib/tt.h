#ifndef TAGTOAD_H
#define TAGTOAD_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define tterr(x) fprintf(stderr, "%s: %s\n", __func__, x);

//============================ VECTOR ============================

#define GENFUNP(X) ((void(*)(void *))X)

typedef struct vector_s {
	void * data;
	size_t unit;
	size_t reserve;
	uint32_t count;
	void (*destructor)(void *);
} vector_t;

void vector_create(vector_t * v, size_t unit, size_t reserve, void(*destructor)(void *));
void vector_destroy(vector_t * v);
void vector_reserve(vector_t * v, size_t reserve);
void vector_push_back(vector_t * v, void * src);
void vector_pop_back(vector_t * v);
void * vector_at(vector_t * v, size_t at);
void vector_remove_at(vector_t * v, size_t at);
void * vector_back(vector_t * v);
void vector_clear(vector_t * v);

//================================================================

//============================= TANK =============================

typedef enum tagtoad_tagtype_e {
	TAGTYPE_INT8,
	TAGTYPE_INT16,
	TAGTYPE_INT32,
	TAGTYPE_INT64,
	TAGTYPE_UINT8,
	TAGTYPE_UINT16,
	TAGTYPE_UINT32,
	TAGTYPE_UINT64,
	TAGTYPE_VECTOR,
	TAGTYPE_MAX = TAGTYPE_VECTOR
} tagtoad_tagtype_t;

#define TT_I8(tt) *((int8_t *) tt->data)
#define TT_I16(tt) *((int16_t *) tt->data)
#define TT_I32(tt) *((int32_t *) tt->data)
#define TT_I64(tt) *((int64_t *) tt->data)
#define TT_U8(tt) *((uint8_t *) tt->data)
#define TT_U16(tt) *((uint16_t *) tt->data)
#define TT_U32(tt) *((uint32_t *) tt->data)
#define TT_U64(tt) *((uint64_t *) tt->data)
#define TT_VEC(tt) &((tagtoad_tagtype_vector_t *) tt->data)->vec

typedef struct tagtoad_tagtype_vector_s {
	vector_t vec;
	tagtoad_tagtype_t type;
} tagtoad_tagtype_vector_t;

//================================

typedef struct tagtoad_tagtypeinfo_s {
	size_t size;
	char const * name;
	void (*destructor) (void *);
} tagtoad_tagtypeinfo_t;

tagtoad_tagtypeinfo_t const * tagtoad_tagtype_info(tagtoad_tagtype_t);

//================================

typedef struct tagtoad_tank_s {
	char * name;
	tagtoad_tagtype_t type;
	void * data;
} tagtoad_tank_t;

bool tagtoad_tank_create(tagtoad_tank_t *, char const *, tagtoad_tagtype_t);
bool tagtoad_tank_vector_init(tagtoad_tank_t *, tagtoad_tagtype_t);

void tagtoad_tank_destroy(tagtoad_tank_t *);
void tagtoad_tank_write(tagtoad_tank_t *, FILE *);
void tagtoad_tank_read(tagtoad_tank_t *, FILE *);

//================================================================

//============================= NODE =============================

typedef struct tagtoad_node_s {
	char * path;
	vector_t tanks;
} tagtoad_node_t;

void tagtoad_node_create(tagtoad_node_t *, char const * path);
void tagtoad_node_destroy(tagtoad_node_t *);
void tagtoad_node_write(tagtoad_node_t *, FILE *);
void tagtoad_node_read(tagtoad_node_t *, FILE *);
tagtoad_tank_t * tagtoad_node_tank_add(tagtoad_node_t *, char const * name, tagtoad_tagtype_t);
tagtoad_tank_t * tagtoad_node_tank_find(tagtoad_node_t *, char const * name);

//================================================================


//============================= ROOT =============================

typedef struct tagtoad_root_s {
	char * name;
	char * path;
	vector_t nodes;
} tagtoad_root_t;

void tagtoad_root_create(tagtoad_root_t *, char const * path, char const * name);
void tagtoad_root_destroy(tagtoad_root_t *);
bool tagtoad_root_save(tagtoad_root_t *);
bool tagtoad_root_load(tagtoad_root_t *);
void tagtoad_root_clear(tagtoad_root_t *);
tagtoad_node_t * tagtoad_root_node_add(tagtoad_root_t *, char const * path);
tagtoad_node_t * tagtoad_root_node_find(tagtoad_root_t *, char const * path);

void tagtoad_root_debug_print(tagtoad_root_t *);

//================================================================

#endif //TAGTOAD_H (tt.h)
