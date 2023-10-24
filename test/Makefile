# define c compiler 
CC = cc  
CFLAGS = -g -Wall -Wpedantic -Werror  -Wno-unused-variable -Wno-unused-but-set-variable

# define compile time flags 
# DADIOS2_USE_MPI for adios2 
CFLAGS += -O2 -DADIOS2_USE_MPI  

# linker flags
LFLAGS += -I$(STREAM_DIR) -I$(IOCOMP_DIR)/include -I$(HDF5_DIR)/include -I$(ADIOS2_DIR)/include 

# object file directory
ODIR = Object_files

# define library paths 
LIBS +=   -L$(IOCOMP_DIR)/lib -liocomp  -L$(HDF5_DIR)/lib -lhdf5_hl -lhdf5  -L$(ADIOS2_DIR)/lib64 -ladios2_c -ladios2_c_mpi -lm \
					-L$(ADIOS2_DIR)/lib -ladios2_c -ladios2_c_mpi -lm \
					#-L${FORGE_DIR}/map/libs/default/gnu/ofi \
					#-lmap-sampler-pmpi -lmap-sampler \
					#-Wl,--eh-frame-hdr -Wl,-rpath=${FORGE_DIR}/map/libs/default/gnu/ofi

# define C source files 
SRCS = $(wildcard *.c)
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

