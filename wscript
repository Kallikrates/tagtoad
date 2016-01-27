from waflib import *
import os

#---DECL---

top = '.'
out = 'build'

execn = 'tagtoad'

#---UTIL---

g_cflags = ["-Wall", "-std=c11"]

def btype_cflags(ctx):
	return {
		"DEBUG"   : g_cflags + ["-Wextra", "-Og", "-ggdb3", "-march=core2", "-mtune=native"],
		"NATIVE" : g_cflags + ["-Ofast", "-march=native", "-mtune=native"],
		"RELEASE" : g_cflags + ["-O3", "-march=core2", "-mtune=generic"],
	}.get(ctx.env.BUILD_TYPE, g_cflags)

#---WAFC---

def testbed(ctx):
	if os.name != "posix":
		Logs.pprint("RED", "testbed supported only on POSIX systems")
		return
	ctx.cmd_and_log("mkdir -p testbed", output = Context.STDOUT, quiet = Context.BOTH)
	ctx.cmd_and_log("rsync -au build/"+"lib"+execn+".so"+" testbed", output = Context.STDOUT, quiet = Context.BOTH)
	ctx.cmd_and_log("rsync -au build/"+execn+"-cli"+" testbed", output = Context.STDOUT, quiet = Context.BOTH)

#---WAFB---

def options(opt):
	opt.load("gcc")
	opt.add_option('--build_type', dest='build_type', type="string", default='RELEASE', action='store', help="DEBUG, NATIVE, RELEASE")

def configure(ctx):
	ctx.load("gcc")
	btup = ctx.options.build_type.upper()
	if btup in ["DEBUG", "NATIVE", "RELEASE"]:
		Logs.pprint("PINK", "Setting up environment for known build type: " + btup)
		ctx.env.BUILD_TYPE = btup
		ctx.env.CFLAGS = btype_cflags(ctx)
		Logs.pprint("PINK", "CFLAGS: " + ' '.join(ctx.env.CFLAGS))
	else:
		Logs.error("UNKNOWN BUILD TYPE: " + btup)

def build(bld):
	bld (
		features = "c cshlib",
		lib = [],
		target = execn,
		source = bld.path.ant_glob('src/lib/*.c'),
	)
	bld (
		features = "c cprogram",
		use = [execn],
		lib = [],
		target = execn+"-cli",
		rpath = ".",
		source = bld.path.ant_glob('src/cli/*.c'),
	)
	
def clean(ctx):
	pass
