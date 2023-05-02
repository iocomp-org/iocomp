# define c compiler 
CC = cc 

# define compile time flags 
# DADIOS2_USE_MPI for adios2 
CFLAGS = -Wall -Wpedantic -Werror -Wextra -fPIC  -DADIOS2_USE_MPI -shared -O2 

# enable and disable different iocomp related flags 
# DNDEBUG for not including error messages 
# DIOCOMP_TIMERS for printing out time taken to write by ioLibraries.c 
# DPRINT_ORDERING for printing out orders of splitting between compute and I/O
# processes
# DREADACK for printing after the data is written into different files 
# DNDELETE for disabling the delete file commands after writing is completed  
IOCOMP_FLAGS += -DNDEBUG #-DNDELETE #-DREADBACK #-DPRINT_ORDERING #-DIOCOMP_TIMERS #-DNDELETE

# linker flags
LFLAGS += -I$(OMPI_DIR)/include -I$(HDF5_DIR)/include -I$(ADIOS2_DIR)/include 

# object file directory
ODIR = Object_files

# define headerfiles
DEPS = ../include/iocomp.h

# define C source files 
SRCS = $(wildcard *.c)

# remove adios2 file from compilation if NOADIOS2 passed to Makefile 
# and remove linker files 
# and pass appropriate flag to iocomp files to remove the references 
ifdef NOADIOS2
$(info ************ ADIOS2 REMOVED ************)
IOCOMP_FLAGS += -DNOADIOS2 

ifdef NOHDF5 # if HDF5 also no wanted 
$(info ************  HDF5 REMOVED ************)
IOCOMP_FLAGS += -DNOHDF5
SRCS = $(filter-out hdf5Write.c hdf5Read.c  adios2Write.c adios2Read.c ,$(wildcard *.c))
LFLAGS = -I$(OMPI_DIR)/include 
endif 

else # only ADIOS2 not wanted 
SRCS = $(filter-out adios2Write.c adios2Read.c ,$(wildcard *.c))
LFLAGS = -I$(OMPI_DIR)/include -I$(HDF5_DIR)/include  
endif 

# define library name
# if LIB variable provided it will save the library into directory given by $(DIR). 
# if not then it will save in ../lib 
ifdef DIR
MAIN = $(DIR)/lib/libiocomp.so
# to make library into another directory given in DIR variable  
LIB: $(OBJS)
	rm -rf $(DIR)
	mkdir $(DIR)
	mkdir $(DIR)/include 
	mkdir $(DIR)/bin
	mkdir $(DIR)/lib 
	# $(CC) $(CFLAGS) $(LFLAGS) -o $(DIR)/lib/libiocomp.so  $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(LFLAGS) -o $(MAIN)  $(OBJS) $(LIBS)
	cp $(DEPS) $(DIR)/include 
else 
MAIN = ../lib/libiocomp.so
endif 

.PHONY: depend clean

# define C object files, all .c files converted to .o files, define path substitution 
_OBJS = $(SRCS:.c=.o)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(IOCOMP_FLAGS) $(LFLAGS) -c $< -o $@

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(IOCOMP_FLAGS) $(LFLAGS) -o $(MAIN) Object_files/*.o

	

clean:
	rm -f $(ODIR)/*.o *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(CFLAGS) $^

# DO NOT DELETE THIS LINE -- make depend needs it

