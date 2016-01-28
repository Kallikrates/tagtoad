#include "tt.h"

#include <string.h>

void tagtoad_node_create (tagtoad_node_t * n, char const * path) {
	vector_create(&n->tanks, sizeof(tagtoad_tank_t), 3, GENFUNP(tagtoad_tank_destroy));
	size_t pathlen = strlen(path);
	n->path = malloc(pathlen);
	strcpy(n->path, path);
}

void tagtoad_node_destroy(tagtoad_node_t * n) {
	vector_destroy(&n->tanks);
	free(n->path);
}

void tagtoad_node_write(tagtoad_node_t * n, FILE * f) {
	fwrite(n->path, strlen(n->path) + 1, 1, f);
	uint16_t ts = n->tanks.count;
	fwrite(&ts, sizeof(uint16_t), 1, f);
	tagtoad_tank_t * t = n->tanks.data;
	for (size_t i = 0; i<n->tanks.count; i++, t+=1) {
		tagtoad_tank_write(t, f);
		fflush(f);
	}
}

void tagtoad_node_read(tagtoad_node_t * n, FILE * f) {
	vector_clear(&n->tanks);

	vector_t rp;
	vector_create(&rp, 1, 1024, NULL);
	char c;
	do {
		c = fgetc(f);
		vector_push_back(&rp, &c);
	} while (c != '\0');
	n->path = realloc(n->path, rp.count);
	strcpy(n->path, rp.data);
	vector_destroy(&rp);

	uint16_t ts;
	fread(&ts, sizeof(uint16_t), 1, f);
	vector_reserve(&n->tanks, ts);
	tagtoad_tank_t tank;
	for (uint16_t i = 0; i < ts; i++) {
		tagtoad_tank_create(&tank, "", 0);
		tagtoad_tank_read(&tank, f);
		vector_push_back(&n->tanks, &tank);
	}
}

tagtoad_tank_t * tagtoad_node_tank_add(tagtoad_node_t * n, char const * name, tagtoad_tagtype_t type) {
	if (tagtoad_node_tank_find(n, name)) return NULL;
	tagtoad_tank_t nt;
	tagtoad_tank_create(&nt, name, type);
	vector_push_back(&n->tanks, &nt);
	return vector_back(&n->tanks);
}

tagtoad_tank_t * tagtoad_node_tank_find(tagtoad_node_t * n, char const * name) {
	tagtoad_tank_t * t = n->tanks.data;
	for (size_t i = 0; i<n->tanks.count; i++, t+=1) {
		if (!strcmp(name, t->name)) return t;
	}
	return NULL;
}
