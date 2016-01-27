#include "tt.h"

#include <memory.h>
#include <stdarg.h>

void vector_create(vector_t * v, size_t unit, size_t reserve, void(*destructor)(void *)) {
	v->data = malloc(reserve * unit);
	v->unit = unit;
	v->reserve = reserve;
	v->count = 0;
	v->destructor = destructor;
}

void vector_destroy(vector_t * v) {
	vector_clear(v);
	free(v->data);
	v->count = 0;
}

void vector_reserve(vector_t * v, size_t reserve) {
	if (reserve > v->reserve) {
		v->data = realloc(v->data, reserve * v->unit);
		v->reserve = reserve;
	}
}

void vector_push_back(vector_t * v, void * src) {
	if (v->count == v->reserve) {
		v->reserve = v->reserve * 1.25 + 1;
		v->data = realloc(v->data, v->reserve * v->unit);
	}
	memcpy(vector_at(v, v->count), src, v->unit);
	v->count++;
}

void vector_pop_back(vector_t * v) {
	if (v->destructor) v->destructor(v->data + (v->unit * (v->count - 1)));
	v->count--;
}

void * vector_at(vector_t * v, size_t at) {
	return v->data + (at * v->unit);
}

void vector_remove_at(vector_t * v, size_t at) {
	if (v->destructor) v->destructor(v->data + (v->unit * at));
	memmove(v->data + (at * v->unit), v->data + ((at + 1) * v->unit), v->count - (at + 1));
	v->count--;
}

void * vector_back(vector_t * v) {
	return v->data + ((v->count - 1) * v->unit);
}

void vector_clear(vector_t * v) {
	if (v->destructor) {
		void * dat = v->data;
		for (size_t i = 0; i < v->count; i++, dat += v->unit) {
			v->destructor(dat);
		}
	}
	v->count = 0;
}
