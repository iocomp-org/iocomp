# define c compiler 
CC = cc 
CFLAGS = -Wall -Wpedantic -Werror -Wextra -Wno-unused-variable -Wno-unused-but-set-variable -Werror=deprecated-declarations 
#CFLAGS = -Wall #-Wpedantic -Werror -Wextra 
# define compile time flags 
CFLAGS += -fPIC  -DADIOS2_USE_MPI -shared #-DNDEBUG

# linker flags
LFLAGS += -I$(OMPI_DIR)/include -I$(HDF5_DIR)/include -I$(ADIOS2_DIR)/include 

# object file directory
ODIR = Object_files

# define headerfiles
DEPS = ../include/iocomp.h

# define library paths 
LIBS +=  -L$(OMPI_DIR)/lib -lmpi -lm -L$(HDF5_DIR)/lib -lhdf5_hl -lhdf5 -L$(ADIOS2_DIR)/lib -ladios2_c -ladios2_c_mpi  -lm 

# define C source files 
#SRCS =  computeStep.c main.c  ioServer.c  computeServer.c intercomm.c initData.c  mpiWrite.c intercommInit.c intercommCreate.c commSplit.c dataOutput.c ioLibraries.c  #hdf5Write.c# hdf5_write.c adios_write.c mpi_write.c 
SRCS =   arrayParamsInit.c ioServer.c ioServerInitialise.c checks.c getPair.c dataSend.c dataWait.c dataSendTest.c mpiWrite.c iocompInit.c intercommCreate.c commSplit.c ioLibraries.c stopSend.c  hdf5Write.c adios2Write.c mpiWrite.c 

# define executable file  
MAIN = iocomp

.PHONY: depend clean

# define C object files, all .c files converted to .o files, define path substitution 
_OBJS = $(SRCS:.c=.o)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) ${LFLAGS} -c $< -o $@

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) ${LFLAGS} -o ../lib/libiocomp.so Object_files/*.o

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 

LIB: $(OBJS)
	rm -rf $(DIR)/* 
	mkdir $(DIR)/include 
	mkdir $(DIR)/bin
	mkdir $(DIR)/lib 
	gcc -shared -o $(DIR)/lib/libiocomp.so Object_files/*.o
	#$(CC) $(CFLAGS) ${LFLAGS} -o $(MAIN) $(OBJS) $(LIBS)
	cp iocomp.h $(DIR)/include/ 
	cp iocomp $(DIR)/bin/ 

clean:
	rm -f $(ODIR)/*.o *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(CFLAGS) $^

# DO NOT DELETE THIS LINE -- make depend needs it

