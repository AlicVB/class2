# Fichier executable test
CFLAGS     += `pkg-config --cflags gtk+-2.0 exiv2`
LDFLAGS    += `pkg-config --libs gtk+-2.0 exiv2`
test: utils.o exiv.o classe.o main.o
	g++ -o test utils.o exiv.o classe.o main.o $(LDFLAGS)
main.o: main.c exiv.h structs.h classe.h utils.h
	g++ $(CFLAGS) main.c -c 
exiv.o: exiv.cpp exiv.h utils.h
	g++ $(CFLAGS) exiv.cpp -c
classe.o: classe.c structs.h utils.h
	g++ $(CFLAGS) classe.c -c
utils.o: utils.c
	g++ $(CFLAGS) utils.c -c
clean:
	rm -f test *.o
