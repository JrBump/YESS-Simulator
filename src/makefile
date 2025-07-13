#src makefile
#Compiles .C files in src directory

CC = g++
INC = ../inc
BIN = ../bin
SRC = ../src
OBJD = ../obj
CFLAGS = -g -c -Wall -std=c++23 -I $(INC)

#Header files included by any .C file in this directory
_DEPS = CPU.h D.h DecodeStage.h E.h ExecuteStage.h F.h FetchStage.h Instruction.h M.h Memory.h MemoryStage.h PipeReg.h PipeRegArray.h PipeRegField.h RegisterFile.h ConditionCodes.h Tools.h String.h Loader.h Stage.h StageArray.h Status.h W.h WritebackStage.h\

#Define DEPS macro which is the same as _DEPS with the INC path in front
# of each .h file name
DEPS = $(patsubst %,$(INC)/%,$(_DEPS))

#Object files for each .C file in this directory
_OBJ =  CPU.o D.o DecodeStage.o E.o ExecuteStage.o F.o FetchStage.o M.o Memory.o MemoryStage.o PipeReg.o PipeRegArray.o PipeRegField.o RegisterFile.o ConditionCodes.o Tools.o String.o Loader.o Stage.o StageArray.o W.o WritebackStage.o yess.o\
#Define OBJ macro which is the same as _OBJ with the OBJD path in front
# of each .C file name
OBJ = $(patsubst %,$(OBJD)/%,$(_OBJ))

#Suffix rule. Each .C is dependent upon all .h files
# listed in the macro DEPS
#First command creates OBJD directory if one doesn't exist
#Second command creates .o out of .C file
$(OBJD)/%.o: %.C $(DEPS)
	@mkdir -p $(OBJD)
	$(CC) $(CFLAGS) $< -o $@

#all target causes .C files do be compiled if needed
$(BIN)/yess: $(OBJ)
	$(CC) $(OBJ) -o $(BIN)/yess

#phony target doesn't correspond to a file
.PHONY: clean
clean:
	rm -f $(OBJ) $(BIN)/yess
