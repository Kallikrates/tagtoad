#include "../lib/tt.h"

#include <stdio.h>
#include <unistd.h>

int main() {
	char cwd [1024];
	getcwd(cwd, 1024);

	char tdir [1024];
	snprintf(tdir, 1024, "%s/test", cwd);

	tagtoad_root_t r;
	tagtoad_root_create(&r, tdir, ".testlol");

	tagtoad_node_t * n;
	tagtoad_tank_t * t;

	n = tagtoad_root_node_add(&r, "test19.jpg");
	t = tagtoad_node_tank_add(n, "testint01", TAGTYPE_UINT8);
	TT_U8(t) = 180;
	t = tagtoad_node_tank_add_vector(n, "testvec01", TAGTYPE_UINT16);
	uint16_t vp = 3456;
	vector_push_back(TT_VEC(t), &vp);
	vp = 7890;
	vector_push_back(TT_VEC(t), &vp);
	t = tagtoad_node_tank_add(n, "testint02", TAGTYPE_INT64);
	TT_I64(t) = 454545454545454;

	tagtoad_root_save(&r);

	//================

	tagtoad_root_load(&r);
	tagtoad_root_debug_print(&r);
	tagtoad_root_destroy(&r);

	return 0;
}
