#include "gol.hpp"
#include "loadRle.hpp"

#include <fstream>

const char * helpText = R"ESC(Update Lexicon -- a subprogram of Analyze RLE.
Analyzes the RLE data in an ash dump and prompts the user what to add to the lexicon.
For each ash, an answer of [y] adds the entry to the lexicon. [n] discards the entry.
An answer of [*] ...
The ash dump file will be cleared unless the --no-clear flag is given.
Use: ./updateLexicon [flags]

OPTION           ABBR    DEFAULT       INFO
--ashdump=FNAME  -d      ashDump.txt   Thentrye file from which to read dumped ash.
--lexicon=FNAME  -l      lexicon.txt   The lexicon to append approved new still life to.
--no-clear       -r                    Whether to not clear the ashdump file.
--help           -h                    Prints this menu.
)ESC";

void promptEntry(std::vector<LexiconEntry> &llex, const std::string &entry)
{
  TileGrid t;
  LoadRawRLE(t, entry);
  std::cout << t << "Is this new ash? [y/*/n]: ";
  std::string ans, name, data;
  getline(std::cin, ans);
  
  switch(ans[0]) {

    case '*':
      std::cout << "Enter the actual rle data for this ash: ";
      getline(std::cin, name);
      data += name;
      data += '\n';
      getline(std::cin, name);
      data += name;
      LoadRawRLE(t, data);

    case 'y':
      std::cout << "Enter a name for this ash: ";
      getline(std::cin, name);
      llex.push_back(LexiconEntry(t));
      llex[llex.size() - 1].name = name;
      break;

    case 'n':
      break;
  }
}

int main(int argc, char* argv[])
{
  /* Argument Parameters */
  std::string ashDumpFile = "ashDump.txt", lexiconFile = "lexicon.txt";
  bool noClear = false, help = false;
    
  /* Argument Processing */
  for(int i = 1; i < argc; ++i)
  {
    int strtok = 0;
    std::string flag = advanceToken(argv[i], strtok, '='),
               value = advanceToken(argv[i], strtok, '\0');

    optionalCmdlineParam<std::string>(flag, ashDumpFile, "--ashdump",  value, "-d", value);
    optionalCmdlineParam<std::string>(flag, lexiconFile, "--lexicon",  value, "-l", value);
    optionalCmdlineParam<bool>       (flag, noClear, "--no-clear", true, "-r", true);
    optionalCmdlineParam<bool>       (flag, help,    "--help",     true, "-h", true);
  }

  if(help)
  {
    std::cout << helpText;
    return 0;
  }

  std::ifstream ifs(ashDumpFile);
  std::string line, entry;
  std::vector<LexiconEntry> llex;

  LoadLexicon(llex, lexiconFile);

  while(getline(ifs, line))
  {
    if(line == "" && entry != "")
    {
      promptEntry(llex, entry);
      entry = "";
    }
    else
    {
      entry += line;
      entry += '\n';
    }
  }
  if(entry != "")
  {
    promptEntry(llex, entry);
  }
  ifs.close();

  SaveLexicon(llex, lexiconFile);
  
  if(noClear)
    return 0;
  
  std::ofstream ofs(ashDumpFile, std::ofstream::out | std::ofstream::trunc);
  ofs.close();
}
