all: analyzeRLE genGrid updateLexicon

analyzeRLE: source/analyzeRLE.cpp
	g++ -o analyzeRLE -O5 src/analyzeRLE.cpp

genGrid: source/genGrid.c
	gcc -o genGrid -O5 src/genGrid.c

updateLexicon: source/updateLexicon.cpp
	g++ -o updateLexicon -O5 src/updateLexicon.cpp

install:
	sudo apt-get install golly

clean:
	rm -f analyzeRLE genGrid updateLexicon
