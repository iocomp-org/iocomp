# define c compiler 
CC = $(OMPI_DIR)/bin/mpicc 
CFLAGS = -Wall -Wpedantic -Werror -Wno-unused-variable -Wno-unused-but-set-variable

# define compile time flags 
CFLAGS += -fPIC -g -Og -DADIOS2_USE_MPI -DNDEBUG

# linker flags
LFLAGS += -I$(OMPI_DIR)/include -I$(IOCOMP_DIR)/include 

# object file directory
ODIR = Object_files

# define library paths 
LIBS +=  -L$(OMPI_DIR)/lib -lmpi -lm -L$(IOCOMP_DIR)/lib -liocomp # -L$(HDF5_DIR)/lib -lhdf5_hl -lhdf5 # -L$(ADIOS2_DIR)/lib -ladios2_c -ladios2_c_mpi  -lm 

# define C source files 
SRCS = main.c computeStep.c computeOutput.c 

# define executable file  
MAIN = test 

.PHONY: depend clean

# define C object files, all .c files converted to .o files, define path substitution 
_OBJS = $(SRCS:.c=.o)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: %.c
	$(CC) $(CFLAGS) ${LFLAGS} -c $< -o $@

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) ${LFLAGS} -o $(MAIN) $(OBJS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 

clean:
	rm -f $(ODIR)/*.o *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(CFLAGS) $^

# DO NOT DELETE THIS LINE -- make depend needs it

