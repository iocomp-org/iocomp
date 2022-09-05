# define c compiler 
CC = mpicc
CFLAGS = -Wall -Wpedantic -Werror -Wno-unused-variable -Wno-unused-but-set-variable

# define compile time flags 
CFLAGS += -fPIC -g -Og -DADIOS2_USE_MPI -DNDEBUG
## linking with benchmark_c  
LFLAGS += -I$(BENCH_DIR)/include -I$(HDF5_DIR)/include -I$(ADIOS2_DIR)/include 

#object file directory
ODIR = Object_files

# define headerfiles
# DEPS = bench_headerfiles.h global_structs.h 
DEPS = iocomp.h

# define library paths 
## linking with benchmark_c  
LIBS += -L$(BENCH_DIR)/lib -lbench   -L$(HDF5_DIR)/lib -lhdf5_hl -lhdf5  -L$(ADIOS2_DIR)/lib -ladios2_c -ladios2_c_mpi  -lm 
#LIBS +=  -lm -lmpi

# define C source files 
SRCS = main.c time_stats.c bench_init.c ioServer.c  computeServer.c intercomm.c # mpi_write.c # hdf5_write.c adios_write.c mpi_write.c 

#define executable file  
MAIN = test

.PHONY: depend clean

#define C object files, all .c files converted to .o files, define path substitution 
_OBJS = $(SRCS:.c=.o)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: %.c $(DEPS)
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

