all: analyzeRLE genGrid updateLexicon

analyzeRLE: source/analyzeRLE.cpp
	g++ -o analyzeRLE source/analyzeRLE.cpp

genGrid: source/genGrid.c
	gcc -o genGrid source/genGrid.c

updateLexicon: source/updateLexicon
	g++ -o updateLexicon source/updateLexicon.cpp

install:
	sudo apt-get install golly

clean:
	rm -f analyzeRLE genGrid updateLexicon