comp:
	xmake project -k compile_commands

toolchain:
	 xmake f --toolchain=llvm

build: comp
	xmake

theme:
	xmake g --theme=emoji

run: build
	xmake run