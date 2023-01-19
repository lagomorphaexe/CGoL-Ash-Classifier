all: analyzeRLE genGrid updateLexicon stats

analyzeRLE: source/analyzeRle.cpp
	g++ -o analyzeRLE -O5 source/analyzeRLE.cpp

genGrid: source/analyzeRle.cpp
	gcc -o genGrid -O5 source/genGrid.c

updateLexicon: source/updateLexicon.cpp
	g++ -o updateLexicon -O5 source/updateLexicon.cpp

stats: source/stats.cpp
	g++ -o stats -O5 source/stats.cpp

install:
	sudo apt-get install golly mpg123

clean:
	rm -f analyzeRLE genGrid updateLexicon
