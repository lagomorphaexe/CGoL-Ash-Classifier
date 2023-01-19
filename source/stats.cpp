#include <iostream>
#include <fstream>
#include "gol.hpp"
#include "loadRle.hpp"

const char * helpText = R"ESC(Stats -- a subprogram of Analyze RLE.
Prints out information about all the analyses done. 
Use: ./stats [flags]
OPTION           ABBR  INFO
--filter=NAME     -f   Filters by a given name.
--counts=NAME     -c   Takes input from a count file (default count.txt)
--lexicon=NAME    -l   Takes input from a lexicon file (default lexicon.txt)
--display         -d   Displays each still life as a tile grid.
--humanize        -H   Displays each count in a more human-readable way.                
--help            -h   Prints this menu.
)ESC";

int g_cswp_ctr = 0;
const std::vector<std::string> g_cswp_clrs = {"\033[0m", "\033[40m"};
inline void cswp()
{
  std::cout << g_cswp_clrs[g_cswp_ctr++];
  g_cswp_ctr %= g_cswp_clrs.size();
}

std::string toLower(std::string s)
{
  for(char& c : s)
  {
    if('A' <= c && c <= 'Z')
    {
      c += 'a' - 'A';
    }
  }
  return s;
}

int main(int argc, char *argv[])
{  
  /* Argument Parameters */
  std::string filter,
              countFile   = "counts.txt",
              lexiconFile = "lexicon.txt";
  bool humanizef = false,
       help      = false,
       display   = false,
       oscill    = false;

  /* Argument Processing */
  for(int i = 1; i < argc; ++i)
  {
    int strtok = 0;
    std::string flag = advanceToken(argv[i], strtok, '='),
               value = advanceToken(argv[i], strtok, '\0');

    optionalCmdlineParam<std::string>(flag, filter,      "--filter",  toLower(value), "-f", toLower(value));
    optionalCmdlineParam<std::string>(flag, countFile,   "--counts",  value,          "-s", value);
    optionalCmdlineParam<std::string>(flag, lexiconFile, "--lexicon", value,          "-l", value);
    optionalCmdlineParam<bool>(flag, humanizef,  "--humanize",   true, "-H", true);
    optionalCmdlineParam<bool>(flag, display,    "--display",    true, "-d", true);
    optionalCmdlineParam<bool>(flag, oscill,     "--oscill",     true, "-o", true);
    optionalCmdlineParam<bool>(flag, help,       "--help",       true, "-h", true);
  }

  if(help)
  {
    std::cout << helpText;
    return 0;
  }

  int tot_count = 0,
      max_namelen = 0,
      max_countlen = 0,
      shape_count = 0;
  
  std::vector<LexiconEntry> Lexicon;
  LoadLexicon(Lexicon, lexiconFile);
  LoadCounts(Lexicon, countFile);

  SortLexicon(Lexicon);

  for(LexiconEntry& le : Lexicon)
  {
    le.count ++; // when a shape is found once, its count is zero. This enables better precision for rare shapes.
    tot_count += le.count;


    if((filter != "" && toLower(le.name).find(filter) == std::string::npos) || 
       (oscill && le.realPeriod() == 1))
      continue;

    shape_count ++;
    if(le.name.length() > max_namelen)
    {
      max_namelen = le.name.length();
    }

    if(humanizef)
    {
      int hslen = humanize(le.count).length();
      if(hslen > max_countlen)
      {
        max_countlen = hslen;
      }
    }
    else
    {
      int slen = std::to_string(le.count).length();
      if(slen > max_countlen)
      {
        max_countlen = slen;
      }
    }
  }

  max_countlen += 5;
  max_namelen += 5;

  std::cout << shape_count << " entries; total " << (humanizef ? humanize(tot_count) : std::to_string(tot_count)) << ".";
  cswp();
  std::cout << "\n";
  coutAdj("NAME", max_namelen);
  coutAdj("COUNT", max_countlen);
  std::cout << "     PERCENTAGE";
  cswp();
  std::cout << "\n";
  for(const LexiconEntry& le : Lexicon)
  {
    if((filter != "" && toLower(le.name).find(filter) == std::string::npos) || 
       (oscill && le.realPeriod() == 1))
      continue;

    coutAdj(le.name, max_namelen);

    if(humanizef)
      coutAdj(humanize(le.count), 10);
    else          
      coutAdj(std::to_string(le.count), max_countlen);
    
    if(le.realPeriod() != 1)
      coutAdj("p" + std::to_string(le.realPeriod()), 5);
    else
      std::cout << "     ";


    std::cout << (100 * (long)le.count / (double)tot_count) << "%";
    if(display && le.cells.size() > 0)
    {
      std::cout << le.cells[0];
    }
    cswp();
    std::cout << "\n";
  }
  std::cout << "\033[0m";
}
