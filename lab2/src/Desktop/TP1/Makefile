#######################################
# Makefile PBM                        #
#                                     #
# E.B.                                #
#######################################


PROG = pgmtopgm ppm_split

all : $(PROG)

# Variables for file compilation
CC        =  gcc
CFLAGS    =  -g -Wall
CPPFLAGS  =  -DDEBUG
LDFLAGS   =  -g -lm

#pxmtopxm : pxmtopxm.o Util.o

pgmtopgm : pgmtopgm.o Util.o

ppm_split : ppm_split.o Util.o

clean :
	@rm -f *.o

cleanall : clean
	@rm -f $(PROG)
