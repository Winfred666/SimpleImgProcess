appName=histogram

testFile=test.cpp

ioSource=./io/readBMP.cpp ./io/readJPEG.cpp ./io/bitOpt.cpp

compiler=g++ -std=c++11
core=core
exec=$(appName).exe
ECHO=./src:

$(appName): $(ioSource) $(testFile)
	@echo off
ifeq ($(OS),Windows_NT)
	mingw32-make -C core -s
else
	make -C core
endif
	$(compiler) $(ioSource) $(testFile) -o $(exec) -I $(core)/ -L $(core)/ -l$(core) -static
	@echo Success!
