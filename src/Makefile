# define c compiler 
CC = cc 

# define compile time flags 
# DADIOS2_USE_MPI for adios2 
CFLAGS = -fpic -shared -g -DADIOS2_USE_MPI  -Wall -Wpedantic -Werror -Wextra 

# enable and disable different iocomp related flags 
# DNDEBUG for not including error messages 
# DIOCOMP_TIMERS for printing out time taken to write by ioLibraries.c 
# DPRINT_ORDERING for printing out orders of splitting between compute and I/O
# processes
# DREADACK for printing after the data is written into different files 
# DNDELETE for disabling the delete file commands after writing is completed  
# DNOADIOS2 for disabling ADIOS2 
# DNOHDF5 for disabling HDF5 
IOCOMP_FLAGS +=-DNODELETE #-DNDEBUG #-DPRINT_ORDERING ##-DREADBACK  #-DIOCOMP_TIMERS #

# linker flags
LFLAGS = -I$(HDF5_DIR)/include -I$(ADIOS2_DIR)/include 

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

endif 

# define library name
# if LIB variable provided it will save the library into directory given by $(DIR). 
ifdef DIR
MAIN = $(DIR)/lib/libiocomp.so
else 
MAIN = ../lib/libiocomp.so
endif 

.PHONY: depend clean

# define C object files, all .c files converted to .o files, define path substitution 
_OBJS = $(SRCS:.c=.o)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(IOCOMP_FLAGS) $(LFLAGS) -c $< -o $@

# if DIR specified then build under that directory, else build under ../lib  
ifdef DIR 
$(MAIN): $(OBJS) 
	rm -rf $(DIR)
	mkdir $(DIR)
	mkdir $(DIR)/include 
	mkdir $(DIR)/lib 
	cp $(DEPS) $(DIR)/include 
	$(CC) $(CFLAGS) $(LFLAGS) -o $(MAIN)  $(OBJS) 
else
$(MAIN): $(OBJS) 
	#$(CC) $(CFLAGS) $(IOCOMP_FLAGS) $(LFLAGS) -o $(MAIN) Object_files/*.o
	$(CC) $(CFLAGS) $(LFLAGS) -o $(MAIN)  $(OBJS) 
endif 

	

clean:
	rm -f $(ODIR)/*.o *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(CFLAGS) $^

# DO NOT DELETE THIS LINE -- make depend needs it

