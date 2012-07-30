include vars.mask
PROGRAM_NAME=echoserver
PROGRAM_CLIENT=client
CLEAN=rm -f
SOURCE_FILES=echoserver.c
SOURCE_CLIENT=echoclient.c
DEST_DIR=${HOME}/bin
COPY_SCRIPT=./copyext.sh

$(PROGRAM_NAME): echoserver.o liberror.a libtransport.a 
	$(CC) $(CCFLAGS) -o $@ echoserver.o $(LIBS) -L. -ltransport -lerror 
echoserver.o: echoserver.c
	$(CC) $(CCFLAGS) -c -o $@ $^
libtransport.a: libtransport.o 
	ar crv $@ $^
libtransport.o: mtransport.c 
	$(CC) $(CCFLAGS) -c -o $@ $^
liberror.a: liberror.o
	ar crv $@ $^
liberror.o: merror.c
	$(CC) $(CCFLAGS) -c -o $@ $^
$(PROGRAM_CLIENT): $(SOURCE_CLIENT)
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBS)
install:
	$(COPY_SCRIPT) $(DEST_DIR)
clean:
	$(CLEAN) $(PROGRAM_NAME) $(PROGRAM_CLIENT) *.o
