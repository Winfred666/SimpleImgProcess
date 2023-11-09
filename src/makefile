appName=BMPReader

testFile=test.cpp

ioSource=./io/readBMP.cpp 
compiler=g++
core=core
exec=$(appName).exe
ECHO=./src:

$(appName):
	@echo $(ECHO)precompiling "cores",generate static lib

ifeq ($(OS),Windows_NT)
	mingw32-make -C core
else
	make -C core
endif

	@echo $(ECHO)linking .a, generate executable file
	$(compiler) $(ioSource) $(testFile) -o $(exec) -I $(core)/ -L $(core)/ -l$(core) -static
