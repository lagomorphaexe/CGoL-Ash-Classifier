all: analyzeRLE genGrid updateLexicon

analyzeRLE: source/analyzeRLE.cpp
	g++ -o analyzeRLE src/analyzeRLE.cpp

genGrid: source/genGrid.c
	gcc -o genGrid src/genGrid.c

updateLexicon: source/updateLexicon
	g++ -o updateLexicon src/updateLexicon.cpp

install:
	sudo apt-get install golly

clean:
	rm -f analyzeRLE genGrid updateLexicon
