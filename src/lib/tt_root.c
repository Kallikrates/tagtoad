#include "tt.h"

#include <stdio.h>
#include <string.h>

void tagtoad_root_create(tagtoad_root_t * r, char const * path, char const * name) {
	vector_create(&r->nodes, sizeof(tagtoad_node_t), 64, GENFUNP(tagtoad_node_destroy));
	size_t pathlen = strlen(path);
	r->path = malloc(pathlen);
	strcpy(r->path, path);
	size_t namelen = strlen(name);
	r->name = malloc(namelen);
	strcpy(r->name, name);
}

void tagtoad_root_destroy(tagtoad_root_t * r) {
	vector_destroy(&r->nodes);
	free(r->name);
	free(r->path);
}

bool tagtoad_root_save(tagtoad_root_t * r) {
	char fn[1024];
	snprintf(fn, 1024, "%s/%s", r->path, r->name);
	FILE * f = fopen(fn, "wb");
	if (!f) {
		fprintf(stderr, "tagtoad_root_save: could not open/create file for writing, check permissions\n");
		return false;
	}

	fwrite("TAGTOAD", 7, 1, f);
	uint32_t nc = r->nodes.count;
	fwrite(&nc, sizeof(nc), 1, f);
	tagtoad_node_t * node = r->nodes.data;
	for (size_t i = 0; i < r->nodes.count; i++, node += 1) {
		tagtoad_node_write(node, f);
	}

	fclose(f);
	return true;
}

bool tagtoad_root_load(tagtoad_root_t * r) {
	char fn[1024];
	snprintf(fn, 1024, "%s/%s", r->path, r->name);
	FILE * f = fopen(fn, "rb");
	if (!f) {
		fprintf(stderr, "tagtoad_root_load_create: could not open/create file for reading, check file and permissions\n");
		return false;
	}

	tagtoad_root_clear(r);

	char th [7];
	fread(th, 7, 1, f);
	if (strncmp(th, "TAGTOAD", 7)) {
		fprintf(stderr, "tagtoad_root_load_create: file is not a tagtoad file, or header corrupted\n");
		return false;
	}
	uint32_t nc;
	fread(&nc, 4, 1, f);
	tagtoad_node_t n;
	for (uint32_t i = 0; i < nc; i++) {
		tagtoad_node_create(&n, "");
		tagtoad_node_read(&n, f);
		vector_push_back(&r->nodes, &n);
	}

	fclose(f);
	return true;
}

void tagtoad_root_clear(tagtoad_root_t * r) {
	vector_clear(&r->nodes);
}

tagtoad_node_t * tagtoad_root_node_add(tagtoad_root_t * r, char const * path) {
	if (tagtoad_root_node_find(r, path)) return NULL;
	tagtoad_node_t nn;
	tagtoad_node_create(&nn, path);
	vector_push_back(&r->nodes, &nn);
	return vector_back(&r->nodes);
}

tagtoad_node_t * tagtoad_root_node_find(tagtoad_root_t * r, char const * path) {
	tagtoad_node_t * n = r->nodes.data;
	for (size_t i = 0; i<r->nodes.count; i++, n+=1) {
		if (!strcmp(path, n->path)) return n;
	}
	return NULL;
}

//DEBUG

void tagtoad_root_debug_print(tagtoad_root_t * r) {
	printf("root path: %s\n", r->path);
	printf("root name: %s\n", r->name);
	printf("node count: %u\n", (uint32_t)r->nodes.count);
	printf("================\n");
	tagtoad_node_t * node = r->nodes.data;
	for (uint32_t i = 0; i < r->nodes.count; i++, node += 1) {
		printf("node %u: %s\ntank count: %u\n====\n", i, node->path, (uint32_t)node->tanks.count);
		tagtoad_tank_t * tank = node->tanks.data;
		for (uint32_t j = 0; j<node->tanks.count; j++, tank+=1) {
			printf("tank %u: (%s) %s\n", j, tagtoad_tagtype_info(tank->type)->name, tank->name);
			printf("    ");
			switch(tank->type) {
			case TAGTYPE_INT8:
				printf("%hhi", TT_I8(tank));
				break;
			case TAGTYPE_INT16:
				printf("%hi", TT_I16(tank));
				break;
			case TAGTYPE_INT32:
				printf("%i", TT_I32(tank));
				break;
			case TAGTYPE_INT64:
				printf("%li", TT_I64(tank));
				break;
			case TAGTYPE_UINT8:
				printf("%hhu", TT_U8(tank));
				break;
			case TAGTYPE_UINT16:
				printf("%hu", TT_U16(tank));
				break;
			case TAGTYPE_UINT32:
				printf("%u", TT_U32(tank));
				break;
			case TAGTYPE_UINT64:
				printf("%lu", TT_U64(tank));
				break;
			case TAGTYPE_VECTOR:
				printf("VECTOR PRINT NOT IMPLEMENTED");
				break;
			}
			printf("\n");
		}
		printf("================\n");
	}
}
