# define c compiler 
CC = cc 

# define compile time flags 
# DADIOS2_USE_MPI for adios2 
CFLAGS = -Wall -Wpedantic -Werror -Wextra -fPIC  -DADIOS2_USE_MPI -shared -g -Og -O2  

# enable and disable different iocomp related flags 
# DNDEBUG for not including error messages 
# DIOCOMP_TIMERS for printing out time taken to write by ioLibraries.c 
# DPRINT_ORDERING for printing out orders of splitting between compute and I/O
# processes
# DREADACK for printing after the data is written into different files 
IOCOMP_FLAGS += -DNDEBUG #-DREADBACK #-DPRINT_ORDERING #-DIOCOMP_TIMERS 

# linker flags
LFLAGS += -I$(OMPI_DIR)/include -I$(HDF5_DIR)/include -I$(ADIOS2_DIR)/include 

# object file directory
ODIR = Object_files

# define headerfiles
DEPS = ../include/iocomp.h

# define C source files 
SRCS = $(wildcard *.c)

# define executable file  
MAIN = iocomp

.PHONY: depend clean

# define C object files, all .c files converted to .o files, define path substitution 
_OBJS = $(SRCS:.c=.o)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(IOCOMP_FLAGS) $(LFLAGS) -c $< -o $@

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(IOCOMP_FLAGS) $(LFLAGS) -o ../lib/libiocomp.so Object_files/*.o

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

