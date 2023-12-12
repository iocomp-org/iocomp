# define c compiler 
CC = cc  
CFLAGS = -Wall -Wpedantic -Werror -O2 -Wno-unused-variable -Wno-unused-but-set-variable #-fsanitize=address

# define compile time flags 
# DMPI_TESTS for including MPI tests in stream kernels 
# DADIOS2_USE_MPI for adios2 
# DNOADIOS2 for disabling ADIOS2 in iocomp 
# DNOHDF5 for disabling HDF5 in iocomp 
CFLAGS += -DADIOS2_USE_MPI # -DMPI_TESTS # #-DNOADIOS2 -DNOHDF5 

# linker flags
LFLAGS +=  -I$(IOCOMP_DIR)/include  
ifndef NOADIOS2
LFLAGS +=  -I$(ADIOS2_DIR)/include 
endif 
ifndef NOHDF5
LFLAGS +=  -I$(HDF5_DIR)/include  
endif 

# object file directory
ODIR = Object_files

# define library paths 
LIBS =   	-L$(IOCOMP_DIR)/lib -liocomp \
					#-L${FORGE_DIR}/map/libs/default/gnu/ofi \
					#-lmap-sampler-pmpi -lmap-sampler \
					#-Wl,--eh-frame-hdr -Wl,-rpath=${FORGE_DIR}/map/libs/default/gnu/ofi

ifndef NOADIOS2 
LIBS +=   -L$(ADIOS2_DIR)/lib64 -ladios2_c -ladios2_c_mpi -lm -L$(ADIOS2_DIR)/lib -ladios2_c -ladios2_c_mpi -lm 
endif 

ifndef NOHDF5
LIBS +=   -L$(HDF5_DIR)/lib -lhdf5_hl -lhdf5 
endif 


# define C source files 
# SRCS = $(wildcard *.c)
SRCS = $(filter-out copy.c, $(wildcard *.c))
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

