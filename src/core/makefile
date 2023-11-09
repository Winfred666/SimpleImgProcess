source=image.cpp colorTrans.cpp binaryImage.cpp zoneSplit.cpp imageOpt.cpp

compiler=g++ -std=c++11
names=$(source:%.cpp=%)
semifinished=$(source:%.cpp=%.o)

output=./libcore.a
ECHO=./src/core:

makeO:=$(foreach fileName,$(names),$(compiler) -c $(fileName).cpp -o $(fileName).o$$;)

core.a: $(source)

	$(compiler) -c $(source)

	ar -rcs $(output) $(semifinished)

ifeq ($(OS),Windows_NT)
	del *.o
else
	rm *.o
endif