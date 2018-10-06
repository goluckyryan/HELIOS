#ifeq ($(ROOTSYS),)
#$(error ROOTSYS is not define, do setuproot)
#endif

ROOTINC = $(shell root-config --cflags)
ROOTLIB = $(shell root-config --new --libs)

CC      =  g++
CCFLAG   = -O -Wall -fPIC

DigiOS:		DigiOS.cc DigiOS.h
		$(CC) $(CCFLAG) $(ROOTINC) $(ROOTLIB) -lm $< -o $@

.PHONY:		clean
clean:
		rm -f DigiOS
