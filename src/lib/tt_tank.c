#include "tt.h"

#include <assert.h>
#include <string.h>

static void tt_vector_destroy(tagtoad_tagtype_vector_t * tv) {
	vector_destroy(&tv->vec);
}

tagtoad_tagtypeinfo_t const * tagtoad_tagtype_info(tagtoad_tagtype_t t) {
	static const tagtoad_tagtypeinfo_t TI_INT8 = {sizeof(int8_t), "int8", NULL};
	static const tagtoad_tagtypeinfo_t TI_INT16 = {sizeof(int16_t), "int16", NULL};
	static const tagtoad_tagtypeinfo_t TI_INT32 = {sizeof(int32_t), "int32", NULL};
	static const tagtoad_tagtypeinfo_t TI_INT64 = {sizeof(int64_t), "int64", NULL};
	static const tagtoad_tagtypeinfo_t TI_UINT8 = {sizeof(uint8_t), "uint8", NULL};
	static const tagtoad_tagtypeinfo_t TI_UINT16 = {sizeof(uint16_t), "uint16", NULL};
	static const tagtoad_tagtypeinfo_t TI_UINT32 = {sizeof(uint32_t), "uint32", NULL};
	static const tagtoad_tagtypeinfo_t TI_UINT64 = {sizeof(uint64_t), "uint64", NULL};
	static const tagtoad_tagtypeinfo_t TI_VECTOR = {sizeof(tagtoad_tagtype_vector_t), "vector", GENFUNP(tt_vector_destroy)};
	switch(t) {
	case TAGTYPE_INT8:
		return &TI_INT8;
	case TAGTYPE_INT16:
		return &TI_INT16;
	case TAGTYPE_INT32:
		return &TI_INT32;
	case TAGTYPE_INT64:
		return &TI_INT64;
	case TAGTYPE_UINT8:
		return &TI_UINT8;
	case TAGTYPE_UINT16:
		return &TI_UINT16;
	case TAGTYPE_UINT32:
		return &TI_UINT32;
	case TAGTYPE_UINT64:
		return &TI_UINT64;
	case TAGTYPE_VECTOR:
		return &TI_VECTOR;
	}
	return NULL;
}

bool tagtoad_tank_create(tagtoad_tank_t * t, char const * name, tagtoad_tagtype_t type) {
	assert(type <= TAGTYPE_MAX);
	t->type = type;
	size_t namelen = strlen(name);
	t->name = malloc(namelen + 1);
	strcpy(t->name, name);
	tagtoad_tagtypeinfo_t const * tinfo = tagtoad_tagtype_info(t->type);
	switch(type) {
	default:
		t->data = calloc(1, tinfo->size);
		break;
	}
	return true;
}

bool tagtoad_tank_vector_init(tagtoad_tank_t * t, tagtoad_tagtype_t vtype) {
	if (t->type != TAGTYPE_VECTOR) {
		tterr("tank does not contain a vector");
		return false;
	}
	tagtoad_tagtypeinfo_t const * vinfo = tagtoad_tagtype_info(vtype);
	vector_create(TT_VEC(t), vinfo->size, 8, vinfo->destructor);
	return true;
}

void tagtoad_tank_destroy(tagtoad_tank_t * t) {
	tagtoad_tagtypeinfo_t const * tinfo = tagtoad_tagtype_info(t->type);
	if (tinfo->destructor) tinfo->destructor(t->data);
	free(t->data);
	free(t->name);
}

void tagtoad_tank_write(tagtoad_tank_t * t, FILE * f) {
	fwrite(t->name, strlen(t->name) + 1, 1, f);
	uint8_t ttype = t->type;
	fwrite(&ttype, 1, 1, f);
	switch(t->type) {
	default:
		fwrite(t->data, tagtoad_tagtype_info(t->type)->size, 1, f);
		break;
	case TAGTYPE_VECTOR:
		break;
	} //TODO - oh god what if they nest the vectors D:
}

void tagtoad_tank_read(tagtoad_tank_t * t, FILE * f) {
	vector_t rp;
	vector_create(&rp, 1, 1024, NULL);
	char c;
	do {
		c = fgetc(f);
		vector_push_back(&rp, &c);
	} while (c != '\0');
	t->name = realloc(t->name, rp.count);
	strcpy(t->name, rp.data);
	vector_destroy(&rp);

	uint8_t ttype;
	fread(&ttype, 1, 1, f);
	assert(ttype <= TAGTYPE_MAX);
	t->type = ttype;

	tagtoad_tagtypeinfo_t const * tinfo = tagtoad_tagtype_info(t->type);

	switch(t->type) {
	default:
		t->data = realloc(t->data, tinfo->size);
		fread(t->data, tinfo->size, 1, f);
		break;
	case TAGTYPE_VECTOR:
		break;
	}
}
