#unit tests
UNIT = unit
#object directory
OBJ = obj
#source directory
SRC = src
#bin directory
BIN = bin
#executables
EXE = lab4 lab5 yess

.PHONY: runlab4 clean runlab5 yess

yess:
	(cd $(SRC) && make ../$(BIN)/yess && ../$(BIN)/run.sh)

#Get into unit directory and compile/link lab4 and then run it
runlab4:
	(cd $(UNIT) && make lab4 && ../$(BIN)/lab4)

runlab5:
	(cd $(UNIT) && make lab5 && ../$(BIN)/lab5)

#Get into the OBJ directory and delete .o files
#Get into the BIN directory and delete executable files
clean:
	(cd $(OBJ) && rm -f *.o)
	(cd $(BIN) && rm -f $(EXE))
