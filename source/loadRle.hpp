#ifndef LOADRLE_H
#define LOADRLE_H

#include <istream>
#include <fstream>
#include <string>
#include <vector>

#include "bits.hpp"
#include "gol.hpp"

void LoadRLE(TileGrid &tileGrid, std::istream &istr)
{
  std::string l, header, info, data;

  while(getline(istr, l, '\n')) {
    switch(l[0]){
      case '#':
        header = l;
        break;
      case 'x':
        info = l;
        break;
      default:
        data += l;
    }
  }

  if(info == "" || data == "") 
  {
    tileGrid = TileGrid(0,0);
    return;
  }

  std::string params;
  int tok = 0;
  int posx, posy;

  while((params = advanceToken(header, tok, ' ')) != "")
  {
    int tok2 = 0;
    std::string varname = advanceToken(params, tok2, '=');
    varname = strip(varname);

    if(varname == "Pos")
    {
      posx = std::stoi(advanceToken(params, tok2, ','));
      posy = std::stoi(advanceToken(params, tok2, '\0'));
    }
    // else if(varname == "Gen") {}
  }

  params = "";
  tok = 0;

  int dim_x = 0;
  int dim_y = 0;
  while((params = advanceToken(info, tok, ',')) != "")
  {
    int tok2 = 0;
    std::string varname = advanceToken(params, tok2, '=');
    varname = strip(varname);

    if(varname == "x")
    {
      dim_x = std::stoi(advanceToken(params, tok2, '\0'));
    }
    else if(varname == "y")
    {
      dim_y = std::stoi(advanceToken(params, tok2, '\0'));
    }
    // else if(varname == "rule") {}
  }

  tileGrid = TileGrid(dim_x, dim_y);
  tileGrid.posx = posx;
  tileGrid.posy = posy;
  int x = 0, y = 0;
  std::string cnum = "";
  int reps = 1;

  for(char c : data) {
    if('0' <= c && c <= '9')
    {
      cnum += c;
    }
    else
    {
      if(cnum == "")
      {
        reps = 1;
      }
      else
      {
        reps = std::stoi(cnum);
        cnum = "";
      }
      switch(c){
        case 'o':
          std::fill_n(&tileGrid.at(x, y), reps, CELL_ON);
          //std::fill_n(tileGrid.data.begin() + y*dim_x + x, reps, CELL_ON);
        case 'b':
          x += reps;
        break;

        case '$':
          x = 0;
          y += reps;
        break;

        case '!':
        case ' ':
        break;

        default:
          ERMSG << "Unrecognized character " << c << " in rle input\n";
      }
    }
  }
}


void ParseRLEData(const std::string &rleString, int* width = nullptr, int* height = nullptr, int* posx = nullptr, int* posy = nullptr, int* rleDataBegin = nullptr)
{
  std::string l, header, info;
  int tok = 0, tokprv = 0;

  while((l = advanceToken(rleString, tok, '\n')) != "")
  {
    switch(l[0])
    {
      case '#':
        header = l;
        break;
      case 'x':
        info = l;
        break;
      default:
        if(rleDataBegin != nullptr) {
          *rleDataBegin = tokprv;
        }
        goto whileloopend;
    }
    tokprv = tok;
  }
  whileloopend:;

  if(info == "")
  {
    if(width  != nullptr) *width  = 0;
    if(height != nullptr) *height = 0;
    ERMSG << "RleError: No pattern dimensions found. Are you sure this is a golly RLE file?\n";
    return;
  }

  std::string params;
  tok = 0;

  while((params = advanceToken(header, tok, ' ')) != "")
  {
    int tok2 = 0;
    std::string varname = advanceToken(params, tok2, '=');
    varname = strip(varname);

    if(varname == "Pos")
    {
      int _posx = std::stoi(advanceToken(params, tok2, ','));
      if(posx != nullptr) *posx = _posx;
      if(posy != nullptr) *posy = std::stoi(advanceToken(params, tok2, '\0'));
    }
    // else if(varname == "Gen") {}
  }

  params = "";
  tok = 0;

  while((params = advanceToken(info, tok, ',')) != "")
  {
    int tok2 = 0;
    std::string varname = advanceToken(params, tok2, '=');
    varname = strip(varname);

    if(varname == "x" && width != nullptr)
    {
      *width = std::stoi(advanceToken(params, tok2, '\0'));
    }
    else if(varname == "y" && height != nullptr)
    {
      *height = std::stoi(advanceToken(params, tok2, '\0'));
    }
    // else if(varname == "rule") {}
  }
}



void LoadNextRLESegment(TileGrid &tileGrid, const std::string &rleString, int read_line_begin = 0, int max_lines_read = -1, int lines_kept = 0)
{

  // Get data from RLE
  int dim_x = 0, dim_y = 0, posx = 0, posy = 0, data = 0;
  ParseRLEData(rleString, &dim_x, &dim_y, &posx, &posy, &data);


  if(dim_x == 0 || dim_y == 0) {
    WARNM << "Created tilegrid of size zero from rle string of length " << rleString.length() << "\n";
    tileGrid = TileGrid(0,0);
    tileGrid.posx = posx;
    tileGrid.posy = posy;
    return;
  }

  // Logic regarding heights of tilegrid segment
  if(max_lines_read == -1 || read_line_begin + max_lines_read > dim_y)
    max_lines_read = dim_y - read_line_begin;

  if(lines_kept > tileGrid.h)
    lines_kept = tileGrid.h;
  
  // Buffer to carry on lines from old to new tileGrid
  TileGrid storedBuffer = TileGrid(dim_x, lines_kept);
  for(int y = 0; y < lines_kept; ++y)
  for(int x = 0; x < dim_x; ++x)
  {
    storedBuffer.at(x, y) = tileGrid.get(x, tileGrid.h - lines_kept + y);
  }

  tileGrid = TileGrid(dim_x, max_lines_read + lines_kept);
  tileGrid.posx = posx;
  tileGrid.posy = posy;
  for(int y = 0; y < lines_kept; ++y)
  for(int x = 0; x < dim_x; ++x)
  {
    tileGrid.at(x, y) = storedBuffer.get(x, y);
  }

  int x = 0, y = 0;
  std::string cnum = "";
  int reps = 1;

  // data parsing
  for(int i = data; i < rleString.length(); ++i) {
    char c = rleString[i];
    if('0' <= c && c <= '9')
    {
      cnum += c;
    }
    else
    {
      if(cnum == "")
      {
        reps = 1;
      }
      else
      {
        reps = std::stoi(cnum);
        cnum = "";
      }
      switch(c){
        case 'o':
          if(y >= read_line_begin)
          {
            std::fill_n(tileGrid.data.begin() + (y - read_line_begin + lines_kept)*dim_x + x, reps, CELL_ON);
          }
        case 'b':
          x += reps;
        break;

        case '$':
          x = 0;
          y += reps;
          if(y >= read_line_begin + max_lines_read)
          {
            return;
          }
        break;

        case '!':
        case ' ':
        case '\n':
        break;

        default:
          ERMSG << "Unrecognized character " << c << " in rle input\n";
      }
    }
  }
}

void LoadRawRLE(TileGrid &tileGrid, const std::string &text)
{
  std::istringstream is(text);

  LoadRLE(tileGrid, is);
}

void LoadFileRLE(TileGrid &tileGrid, const std::string &fileName)
{
  std::ifstream f;
  f.open(fileName);

  LoadRLE(tileGrid, f);
}

std::string ToRLE(const TileGrid &tileGrid, bool includeHeader = true) {
  if(tileGrid.data.size() == 0)
  {
    ERMSG << "Cannot convert TileGrid with dimension of size 0 to RLE format.\n";
  }

  std::string out;
  if(includeHeader)
  {
    out = "x = " + std::to_string(tileGrid.w) + ", y = " + std::to_string(tileGrid.h) + ", rule = B3/S23\n";
  }

  int ctr = 0;
  byte val;
  for( int y = 0; y < tileGrid.h; ++y )
  {
    ctr = 0;
    val = tileGrid.get(0, y) & CELL_ON_BIT;
    for( int x = 0; x < tileGrid.w; ++x )
    {
      if(( val ^ (tileGrid.get(x, y) & CELL_ON_BIT)) == 0) // if there is no difference between the CELL_ON_BIT of val and tileGrid(x,y)
      {
        ctr++;
      }
      else
      {
        if( ctr > 1 )
        {
          out += std::to_string(ctr);
        }
        out += val ? 'o' : 'b';

        val = tileGrid.get(x, y) & CELL_ON_BIT;
        ctr = 1; 
      }
    }
    if(val == 1)
    {
      if( ctr > 1)
      {
        out += std::to_string(ctr);
      }
      out += val ? 'o' : 'b';
    }
    if(y != tileGrid.h - 1)
    {
      out += '$';
    }
    else
    {
      out += '!';
    }
  }

  return out;
}

// File format:
// NAME; X,Y; RLEDATA     if has period 1
// NAME; PERIOD
// X1,Y1; RLEDATA1
// X2,Y2; RLEDATA2
// ...                    if oscillates
void LoadLexicon(std::vector<LexiconEntry> &llex, const std::string &fileName)
{
  std::ifstream f;
  f.open(fileName);
  std::string line, lexen = "";
  
  if(!f.is_open())
  {
    ERMSG << "File " << fileName << " failed to open while loading lexicon.\n";
    return;
  }

  while(getline(f, line))
  {
    if(line == "" && lexen != "")
    {
      llex.push_back(LexiconEntry(lexen));
      lexen = "";
    }
    else if(line[0] != '#')
    {
      lexen += line + '\n';
    }
  }
  if(lexen != "")
  {
    llex.push_back(LexiconEntry(lexen));
  }
}

// File format:
// NAME; COUNT     with newline separation
// If a count is given to a shape not in the lexicon, a degenerate lexicon entry is created that only stores a name and count.
void LoadCounts(std::vector<LexiconEntry> &llex, const std::string &fileName)
{
  std::ifstream f;
  f.open(fileName);
  std::string line;

  if(!f.is_open())
  {
    ERMSG << "File " << fileName << " failed to open while loading counts.\n";
    return;
  }

  while(getline(f, line))
  {
    if(line[0] != '#')
    {
      int tok = 0;
      std::string name = advanceToken(line, tok, ';');
      bool nameFound = false;

      for(int i = 0; i < llex.size(); ++i)
      {
        if(llex[i].name == name)
        {
          llex[i].count = std::stoi(advanceToken(line, tok, '\0'));
          nameFound = true;
          break;
        }
      }
      if(!nameFound)
      {
        LexiconEntry t;
        t.name = name;
        t.count += std::stoi(advanceToken(line, tok, '\0'));
        llex.push_back(t);
      }
    }
  }

  f.close();
}

void LoadNewAsh(std::vector<TileGrid> &newAsh, const std::string &fileName)
{
  std::ifstream f(fileName);
  std::string line, entry;

  if(!f.is_open())
  {
    ERMSG << "File " << fileName << " failed to open while loading new ash file\n";
    return;
  }

  while(getline(f, line))
  {
    if(line == "")
    {
      if(entry != "")
      {
        TileGrid t;
        LoadRawRLE(t, entry);
        entry = "";
        newAsh.push_back(t);
      }
    }
    else
    {
      entry += line;
      entry += "\n";
    }
  }

  f.close();
}

// See LoadLexicon for an explanation of lexicon file formatting.
void SaveLexicon(const std::vector<LexiconEntry> &llex, const std::string &fileName, bool checkDuplicates = false)
{

  /*if(checkDuplicates)
  {
    for(int i = 0; i < llex.size(); ++i)
    {
      for(int j = i + 1; j < llex.size(); ++j)
      {
        if(llex[j] == llex[i])
      }
    }
  }*/

  std::ofstream f;
  f.open(fileName);

  bool first = true;

  for(LexiconEntry le : llex)
  {
    if(le.period > 0)
    {
      if(!first)
      {
        f << "\n\n";
      }
      else
      {
        first = false;
      }
      f << le.toString();
    }
  }
}

// See LoadCounts for an explanation of count file formatting.
// Only saves counts greater than zero.
void SaveCounts(const std::vector<LexiconEntry> &llex, const std::string &fileName, bool sort = false)
{
  std::ofstream f(fileName);

  bool first = true;

  for(LexiconEntry le : llex)
  {
    if(le.count > 0)
    {
      if(!first)
      {
        f << '\n';
      }
      else
      {
        first = false;
      }
      f << le.name << "; " << le.count;
    }
  }
  f.close();
}

void SaveNewAsh(const std::vector<TileGrid> &newAsh, const std::string &fileName)
{
  std::ofstream f(fileName);
  for(int i = 0; i < newAsh.size(); ++i)
  {
    f << ToRLE(newAsh[i]) << "\n\n";
  }
  f.close();
}

// Gol-class functions that depend on RLE encoding defined in this file
LexiconEntry::LexiconEntry(std::string str) {
  int strtokLine = 0;
  int strtokPunct = 0;
  
  std::string header = advanceToken(str, strtokLine, '\n');
  if(header == "") return;

  name = advanceToken(header, strtokPunct, ';');
  std::string next = advanceToken(header, strtokPunct, ',');

  if(strtokPunct >= header.length())
  {
    // Oscillator, p > 1, next is period
    period = std::stoi(next);
    cells = std::vector<TileGrid>(period);

    for(int i = 0; i < period; ++i)
    {
      std::string frame = advanceToken(str, strtokLine, '\n');
      strtokPunct = 0;

      std::string RLEdata = "x = " + advanceToken(frame, strtokPunct, ',');
      RLEdata += ", y = " + advanceToken(frame, strtokPunct, ';');
      RLEdata += ", rule = B3/S23\n";
      RLEdata += advanceToken(frame, strtokPunct, '\0');
      
      LoadRawRLE(cells[i], RLEdata);
    }
  }
  else
  {
    // Still life, p = 1, next is beginning of size qualifier
    period = 1;
    cells = std::vector<TileGrid>(1);

    std::string RLEdata = "x = " + next; 
    RLEdata += ", y = " + advanceToken(header, strtokPunct, ';');
    RLEdata += ", rule = B3/S23\n" ;
    RLEdata += advanceToken(header, strtokPunct, '\0');

    LoadRawRLE(cells[0], RLEdata);
  }
}

std::string LexiconEntry::toString() {
  std::string out = name;
  if(period > 1)
  {
    out += "; ";
    out += std::to_string(period);
    for(int i = 0; i < period; ++i)
    {
      out += '\n';
      out += std::to_string(cells[i].w);
      out += ",";
      out += std::to_string(cells[i].h);
      out += "; ";
      out += ToRLE(cells[i], false);
    }
  }
  else if(period == 1)
  {
    out += "; ";
    out += std::to_string(cells[0].w);
    out += ",";
    out += std::to_string(cells[0].h);
    out += "; ";
    out += ToRLE(cells[0], false);
  }
  else
  {
    return "";
  }
  return out;
}

#endif
