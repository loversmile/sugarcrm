Basics =   sugar_soapC.o sugar_soapClient.o sugar_stdsoap2.o  md5.o sugar_client.o  sugar_function.o

Target = clientSugar

CFLAGS =  -lpthread

all: ${Basics}
	gcc -o ${Target} ${Basics}  $(CFLAGS)
%.o: %.c
	gcc -c $< -o $@  $(CFLAGS)

clean:
	rm *.o -rf
lclean:
	rm global.o clientSugar.o interface.o
