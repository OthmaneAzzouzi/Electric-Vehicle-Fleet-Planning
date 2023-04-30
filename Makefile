# Makefile pour INF3105 / TP5
OPTIONS = -Wall

#Si ancien GNU gcc (g++), il peut être nécessaire d'ajouter -std=c++11

# Choisir l'une des deux configurations : -g -O0 pour débogage et -O2 pour la remise.
#OPTIONS = -g -O0 -Wall
OPTIONS = -O2 -Wall

# Syntaxe : cible : dépendance1 dépendance2 ...
# Ensuite, la ou les ligne(s) débutant par une tabulation (\t) donne les commandes pour construire une cible

program :  program.o reseau.o dateheure.o es.o
	g++ ${OPTIONS} -o program program.o reseau.o dateheure.o es.o

tp5.o: tp5.cpp reseau.h dateheure.h
	g++ ${OPTIONS} -c program.cpp

reseau.o: reseau.cpp reseau.h dateheure.h
	g++ ${OPTIONS} -c reseau.cpp

es.o: es.cpp reseau.h dateheure.h
	g++ ${OPTIONS} -c es.cpp

dateheure.o: dateheure.cpp dateheure.h
	g++ ${OPTIONS} -c dateheure.cpp

valideur :  valideur.cpp reseau.o dateheure.o es.o
	g++ ${OPTIONS} -o valideur valideur.cpp reseau.o dateheure.o es.o

clean:
	rm -rf program *.o

