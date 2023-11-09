source=image.cpp colorTrans.cpp

compiler=g++
names=$(source:%.cpp=%)
semifinished=$(source:%.cpp=%.o)

output=./libcore.a
ECHO=./src/core:

makeO:=$(foreach fileName,$(names),$(compiler) -c $(fileName).cpp -o $(fileName).o$$;)

core.a: $(source)
	@echo $(ECHO)precompiling program,generate .o files

	$(compiler) -c $(source)

	ar -rcs $(output) $(semifinished)

	@echo $(ECHO)removing .o files

ifeq ($(OS),Windows_NT)
	del *.o
else
	rm *.o
endif