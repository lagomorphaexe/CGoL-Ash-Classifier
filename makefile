all: analyzeRLE genGrid updateLexicon

analyzeRLE: source/analyzeRLE.cpp
	g++ -o analyzeRLE -O3 src/analyzeRLE.cpp

genGrid: source/genGrid.c
	gcc -o genGrid -O3 src/genGrid.c

updateLexicon: source/updateLexicon.cpp
	g++ -o updateLexicon -O3 src/updateLexicon.cpp

install:
	sudo apt-get install golly

clean:
	rm -f analyzeRLE genGrid updateLexicon
