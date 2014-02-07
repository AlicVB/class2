# Fichier executable test
CFLAGS     += `pkg-config --cflags gtk+-2.0 exiv2`
LDFLAGS    += `pkg-config --libs gtk+-2.0 exiv2`
classe: utils.o exiv.o classe.o main.o
	g++ -g -o classe utils.o exiv.o classe.o main.o $(LDFLAGS)
main.o: main.c exiv.h structs.h classe.h utils.h
	g++ -g $(CFLAGS) main.c -c 
exiv.o: exiv.cpp exiv.h utils.h
	g++ -g $(CFLAGS) exiv.cpp -c
classe.o: classe.c structs.h utils.h exiv.h
	g++ -g $(CFLAGS) classe.c -c
utils.o: utils.c
	g++ -g $(CFLAGS) utils.c -c
clean:
	rm -f test *.o
