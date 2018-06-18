# Generated automatically from Makefile.in by configure.
bindir = ${exec_prefix}/bin
sysconfdir = ${prefix}/etc
mandir = ${prefix}/man

CC = gcc 
CFLAGS = -g -DDEBUG -s -O2 -Wall 
#CFLAGS = -s -O2
DATE = $(shell date +"%Y%m%d")
SRC = $(shell ls -1 *.[ch])
OBJ = $(shell ./objects.sh)
IMP = Makefile AUTHORS COPYING TODO objects.sh README 

all: simulator 
	@echo "\n \n./simulator to run \n \n"

simulator: $(OBJ)
	$(CC) -o ./$@ $(OBJ) -lm

simulator.o: sensorHeaders.h initSensornet.c 
gnuplot.o: gnuplot_i.h gnuplot_i.c
runDirect.o: runDirect.h gnuplot_i.h
runMyalgo.o: runMyalgo.h gnuplot_i.h
runLeach.o: runLeach.h gnuplot_i.h
spreadSensor.o: spreadSensor.h spreadSensor.c
sensorConfig.o: sensorConfig.h sensorConfig.c
findEnergy.o: findEnergy.h findEnergy.c
findConfidenceValue.o: findConfidenceValue.h findConfidenceValue.c

.c.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean backup pretty

pretty: $(SRC)
	ls $? | xargs -n1 indent -bl -c41 -i4 -l72 -pcs
	gmake clean

clean:
	-rm -f *.o core *\~
	@echo "\nCleaning done...\n"

backup:
	tar -zcvf bkps/simulator.$(DATE).tar.gz $(SRC) $(IMP)
