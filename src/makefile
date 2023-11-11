
appName=geometry

aLibs=core io


testFile=test.cpp


#above are files that could change frequently

compiler=g++ -std=c++11
exec=$(appName).exe
ECHO=./src:

gatherA:=$(foreach alib,$(aLibs), -I $(alib)/ -L $(alib)/ -l $(alib) )

$(appName): $(ioSource) $(testFile)
	@echo off
ifeq ($(OS),Windows_NT)
	mingw32-make -C core -s
	mingw32-make -C io -s
else
	make -C core
	make -C io
endif
#$(compiler) $(testFile) -o $(exec) -I $(core) -L $(core) -l $(core) -static
	$(compiler) $(testFile) -o $(exec) $(gatherA) -static
	@echo Success!
