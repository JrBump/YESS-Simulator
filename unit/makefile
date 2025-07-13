#unit makefile
#Compiles/links unit testing code

CC = g++
INC = ../inc
BIN = ../bin
SRC = ../src
OBJD = ../obj
CFLAGS = -g -c -Wall -std=c++23 -I $(INC)

#.h files included by any file in this directory
_DEPS = Memory.h RegisterFile.h MemoryTester.h RegisterFileTester.h \
  ConditionCodes.h ConditionCodesTester.h

#Define a macro DEPS that is the same as _DEPS with the INC in front
# of each .h file name
DEPS = $(patsubst %,$(INC)/%,$(_DEPS))

#-----Macros specific to lab4
#List of .o files needed by lab4 created from .C files in this directory
_L4OBJ1 =  MemoryTester.o RegisterFileTester.o ConditionCodesTester.o lab4.o

#Define a macro L4OBJ1 that is the same as _L4OBJ1 with the OBJD in front
# of each .o file name
L4OBJ1 = $(patsubst %,$(OBJD)/%,$(_L4OBJ1))

#List of .o files needed by lab4 created from .C files in the source directory
_L4OBJ2 =  Memory.o RegisterFile.o ConditionCodes.o Tools.o

#Define a macro L4OBJ2 that is the same as _L4OBJ2 with the OBJD in front
# of each .o file name
L4OBJ2 = $(patsubst %,$(OBJD)/%,$(_L4OBJ2))
#-----End of macros specific to lab4

#-----Macros specific to lab5
#List of .o files needed by lab5 created from .C files in this directory
_L5OBJ1 =  lab5.o

#Define a macro L4OBJ1 that is the same as _L5OBJ1 with the OBJD in front
# of each .o file name
L5OBJ1 = $(patsubst %,$(OBJD)/%,$(_L5OBJ1))

#List of .o files needed by lab4 created from .C files in the source directory
_L5OBJ2 = String.o

#Define a macro L4OBJ2 that is the same as _L5OBJ2 with the OBJD in front
# of each .o file name
L5OBJ2 = $(patsubst %,$(OBJD)/%,$(_L5OBJ2))
#-----End of macros specific to lab5


#All .C files in this directory are dependent upon all .h files
# listed in the macro DEPS
#Create an OBJD directory if it doesn't exist
#Second command specifies how to create a .o file
$(OBJD)/%.o: %.C $(DEPS)
	@mkdir -p $(OBJD)
	$(CC) $(CFLAGS) $< -o $@ 

.PHONY: clean all lab4 lab5
#Make the executables from the files in this directory
all:
	make lab4
	make lab5

#Get into the src directory and compile those files
#Create a BIN directory if it doesn't exist
#Link the .o files together
lab4: 
	(cd $(SRC) && make $(L4OBJ2))
	@mkdir -p $(BIN)
	make $(BIN)/lab4
	$(CC) $(L4OBJ1) $(L4OBJ2) -o $(BIN)/lab4

#The dependence on L4OBJ1 causes those files to be recompiled if needed
$(BIN)/lab4: $(L4OBJ1) 


#The dependence on L5OBJ1 causes those files to be recompiled if needed
#Get into the src directory and compile those files
#Create a BIN directory if it doesn't exist
#Link the .o files together
lab5:
	(cd $(SRC) && make $(L5OBJ2))
	make $(BIN)/lab5
	@mkdir -p $(BIN)
	$(CC) $(L5OBJ1) $(L5OBJ2) -o $(BIN)/lab5

$(BIN)/lab5: $(L5OBJ1) 

clean:
	rm -f $(OBJD)/*.o $(BIN)/lab4 $(BIN)/lab5
