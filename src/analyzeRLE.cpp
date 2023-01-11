// TODO: Make sure Harbor is still recognized as a periodic life

#include <iostream>
#include <vector>
#include <sstream>

#include "analyze.hpp"
#include "loadRle.hpp"
#include "bits.hpp"
#include "_config.h"
 
TileGrid Universe;
std::vector<LexiconEntry> LifeLexicon;

const char * helpText = R"ESC(Analyze RLE -- a tool to classify ash in RLE files.
Use: ./analyzeRLE [flags] [RLEfile.rle]
If an option expecting an argument is specified but no value is given, the value in the default column is assumed.

OPTION           ABBR    DEFAULT       INFO
--lexicon=FNAME  -l      lexicon.txt   Uses a lexicon of known still life from file FNAME.
--counts=FNAME   -c      counts.txt    Uses file FNAME to document the number of times each ash shows up.
--ashdump=FNAME  -d      ashDump.txt   Dumps newly found ash into FNAME to be later manually parsed by ./updateLexicon.
--readonly       -r                    Starts in readonly mode, where no files are modified.
--help           -h                    Prints this help menu.
)ESC";

int main(int argc, char *argv[]){

    //std::ios_base::sync_with_stdio(false);

    /* Argument Parameters */
    std::string rleFile = "COUNT.rle", lexiconFile = "lexicon.txt", countFile, newAshFile;
    bool readonly = false, help = false;
    
    /* Argument Processing */
    for(int i = 1; i < argc; ++i)
    {
        int strtok = 0;
        std::string flag = advanceToken(argv[i], strtok, '='),
                   value = advanceToken(argv[i], strtok, '\0');
         
        if(flag == argv[i] && flag[0] != '-') rleFile = flag;
        optionalCmdlineParam<std::string>(flag, lexiconFile, "--lexicon",  value, "-l", value);
        optionalCmdlineParam<std::string>(flag, countFile,   "--counts",   value, "-c", "counts.txt");
        optionalCmdlineParam<std::string>(flag, newAshFile,  "--ashdump",  value, "-d", "ashDump.txt");
        optionalCmdlineParam<bool>       (flag, help,        "--help",     true,  "-h", true);
        optionalCmdlineParam<bool>       (flag, readonly,    "--readonly", true,  "-r", true);
    }

    if(help)
    {
        std::cout << helpText;
        return 0;
    }

    /* Lexicon Loading */
    LoadLexicon(LifeLexicon, lexiconFile);
    std::cout << "Lexicon loaded. \t\t(size: " << LifeLexicon.size() << ")\n";

    if(countFile != "")
    {
        LoadCounts(LifeLexicon, countFile);
        std::cout << "Counts loaded.\n";
        SortLexicon(LifeLexicon);
    }
        
    std::vector<TileGrid> newAsh{};
    if(newAshFile != "") {
        LoadNewAsh(newAsh, newAshFile);
        std::cout << "Unprocessed Ash loaded. \t(size: " << newAsh.size() << ")\n";
    }
#ifdef SEGMENTED_READ

    // load RLE string
    std::ifstream f(rleFile);
    std::stringstream rleBuffer;
    rleBuffer << f.rdbuf();
    std::string rleData = rleBuffer.str();

    int width, height;
    ParseRLEData(rleData, &width, &height);

    if(width == 0 || height == 0)
    {
        // This is appended to the error message thrown by the ParseRLEData function
        ERMSG << "  File " << rleFile << "\n";
        return 1;
    }

    std::cout << "RLE file read successfully. \t(size: " << width << " x " << height << ")\n";
    std::cout << "Beginning search...\n\n";

    // Units are in lines
    int segmentStart = 0;
    int segmentSize = segment_maxVectorSize / width;
    
    if (segmentSize - segment_linesBuffered <= 0)
    {
        ERMSG << "SegmentError: Buffer is larger than segment read, exiting.\n"
              << "Segment Size = " << segmentSize << " = " << segment_maxVectorSize << " / " << width << " is less than buffer size " << segment_linesBuffered << '\n'
              << "To troubleshoot, either decrease the width of the data of the size of the buffer.\n";
        return 0;
    }

    int firstpass = true;

    while(segmentStart < height)
    {
        // Take next segment of universe. On the first passthrough, don't take the buffer from the end of the last segment.
        if(firstpass)
        {
            LoadNextRLESegment(Universe, rleData, segmentStart, segmentSize, 0);
        }
        else
        {
            LoadNextRLESegment(Universe, rleData, segmentStart + segment_linesBuffered, segmentSize - segment_linesBuffered, segment_linesBuffered);
        }
        Universe.posy += segmentStart;

        int maxBoundY;
        clrln(g_linesize);
        std::cout << "Cut new segment from line " << segmentStart << " to ";

        if(segmentStart + segmentSize > height)
        {
            maxBoundY = height - segmentStart;
            std::cout << height << '\n';
        }
        else
        {
            maxBoundY = segmentSize - segment_linesBuffered;
            std::cout << segmentStart + segmentSize << '\n';
        }
        printProgressBar(segmentStart, height, g_linesize - 30);

        for(int y = 0; y < maxBoundY; ++y)
        {
            if((y + segmentStart) % pow(10, ilog(height) - 2) == 0)
            {
                printProgressBar(y + segmentStart, height, g_linesize - 30);
            }
            for(int x = 0; x < width; ++x)
            {
                // Make sure that the cell is not checked and is on
                if((Universe.get(x,y) & (CELL_CHECKED_BIT | CELL_ON_BIT)) == CELL_ON_BIT)
                {
                    AnalyzeCell(Universe, LifeLexicon, x, y, &newAsh);
                }
            }
        }

        segmentStart += maxBoundY;
        
        firstpass = false;
    }

    printProgressBar(height, height, g_linesize - 30);
    std::cout << "\n";
#else
    LoadFileRLE(Universe, rleFile);
    std::cout << "Loaded " << Universe.w << " by " << Universe.h << " grid\n";
    DMESG << "Top left corner at x=" << Universe.posx << ", y=" << Universe.posy << "\n";

    for(int y = 0; y < Universe.h; ++y)
    for(int x = 0; x < Universe.w; ++x)
    {
        if((Universe.get(x,y) & (CELL_CHECKED_BIT | CELL_ON_BIT)) == CELL_ON_BIT)
        {
            AnalyzeCell(Universe, LifeLexicon, x, y, &newAsh);
        }
    }
#endif

    if(readonly) return 0;
    /* Write operations */
    SaveLexicon(LifeLexicon, lexiconFile, true);
    if(countFile != "")
    {
        SaveCounts(LifeLexicon, countFile, true);
    }
    if(newAshFile != "")
    {
        SaveNewAsh(newAsh, newAshFile);
    } 

    return 0;
}
