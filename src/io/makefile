source=readBMP.cpp bitOpt.cpp readJPEG.cpp



compiler=g++ -std=c++11
names=$(source:%.cpp=%)
semifinished=$(source:%.cpp=%.o)
output=./libio.a
ECHO=./src/io:
core=core

makeO:=$(foreach fileName,$(names),$(compiler) -c $(fileName).cpp -o $(fileName).o$$;)

io.a: $(source)

	$(compiler) -c $(source) -I ../$(core)/ -L ../$(core)/ -l$(core) -static

	ar -rcs $(output) $(semifinished)

ifeq ($(OS),Windows_NT)
	del *.o
else
	rm *.o
endif