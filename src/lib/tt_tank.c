#include "tt.h"

#include <assert.h>
#include <string.h>

tagtoad_tagtypeinfo_t const * tagtoad_tagtype_info(tagtoad_tagtype_t t) {
	static const tagtoad_tagtypeinfo_t TI_INT8 = {sizeof(int8_t), "int8"};
	static const tagtoad_tagtypeinfo_t TI_INT16 = {sizeof(int16_t), "int16"};
	static const tagtoad_tagtypeinfo_t TI_INT32 = {sizeof(int32_t), "int32"};
	static const tagtoad_tagtypeinfo_t TI_INT64 = {sizeof(int64_t), "int64"};
	static const tagtoad_tagtypeinfo_t TI_UINT8 = {sizeof(uint8_t), "uint8"};
	static const tagtoad_tagtypeinfo_t TI_UINT16 = {sizeof(uint16_t), "uint16"};
	static const tagtoad_tagtypeinfo_t TI_UINT32 = {sizeof(uint32_t), "uint32"};
	static const tagtoad_tagtypeinfo_t TI_UINT64 = {sizeof(uint64_t), "uint64"};
	static const tagtoad_tagtypeinfo_t TI_VECTOR = {sizeof(vector_t), "vector"};
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

void tagtoad_tank_create(tagtoad_tank_t * t, char const * name, tagtoad_tagtype_t type) {
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
	case TAGTYPE_VECTOR:
		assert(type != TAGTYPE_VECTOR); //use tagtoad_tank_create_vector!
		break;
	}
}

void tagtoad_tank_create_vector(tagtoad_tank_t * t, char const * name, tagtoad_tagtype_t type) {
	assert(type <= TAGTYPE_MAX);
	assert(type != TAGTYPE_VECTOR); //2deep4me
	t->type = TAGTYPE_VECTOR;
	size_t namelen = strlen(name);
	t->name = malloc(namelen + 1);
	strcpy(t->name, name);

	t->vtype = type;
	t->data = malloc(sizeof(vector_t));
	vector_create(t->data, tagtoad_tagtype_info(type)->size, 16, NULL);
}

void tagtoad_tank_destroy(tagtoad_tank_t * t) {
	switch (t->type) {
	case TAGTYPE_VECTOR:
		vector_destroy(t->data);
		break;
	default:
		break;
	}
	free(t->data);
	free(t->name);
}

void tagtoad_tank_write(tagtoad_tank_t * t, FILE * f) {
	fwrite(t->name, strlen(t->name) + 1, 1, f);
	uint8_t ttype = t->type;
	uint8_t vtype = t->vtype;
	fwrite(&ttype, 1, 1, f);
	switch(t->type) {
	default:
		fwrite(t->data, tagtoad_tagtype_info(t->type)->size, 1, f);
		break;
	case TAGTYPE_VECTOR:
		fwrite(&vtype, sizeof(vtype), 1, f);
		fwrite(&TT_VEC(t)->count, sizeof(TT_VEC(t)->count), 1, f);
		char * data = TT_VEC(t)->data;
		for (uint32_t i = 0; i < TT_VEC(t)->count; i++, data += TT_VEC(t)->unit) {
			fwrite(data, TT_VEC(t)->unit, 1, f);
		}
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
		t->data = realloc(t->data, tinfo->size);
		uint8_t vtype;
		fread(&vtype, sizeof(vtype), 1, f);
		assert(vtype <= TAGTYPE_MAX);
		t->vtype = vtype;
		tagtoad_tagtypeinfo_t const * vinfo = tagtoad_tagtype_info(vtype);
		uint32_t count;
		fread(&count, sizeof(count), 1, f);
		vector_create(t->data, vinfo->size, count, NULL);
		void * data = TT_VEC(t)->data;
		for (uint32_t i = 0; i < count; i++, data += TT_VEC(t)->unit) {
			fread(data, TT_VEC(t)->unit, 1, f);
			vector_push_back(t->data, data);
		}
		break;
	}
}
