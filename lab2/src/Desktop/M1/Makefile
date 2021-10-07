#######################################
# Makefile PBM                        #
#                                     #
# E.B.                                #
#######################################


PROG = prog
all : $(PROG)

# Variables for file compilation
CC        =  gcc
CFLAGS    =  -g -Wall
CPPFLAGS  =  -DDEBUG
LDFLAGS   =  -g -lm

#pxmtopxm : pxmtopxm.o Util.o

prog : prog.o Util.o

clean :
	@rm -f *.o

cleanall : clean
	@rm -f $(PROG)
