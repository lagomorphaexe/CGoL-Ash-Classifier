#ifndef GOL_H
#define GOL_H

#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "bits.hpp"
#include "_config.h"

// Cell bits for identifying cell states
#define CELL_OFF 0
#define CELL_ON 1

#define CELL_ON_BIT 1       // is the cell on
#define CELL_CHECKED_BIT 2  // is cell already checked by a search function
#define CELL_BUILD_BIT 4    // is cell going to be turned on next iteration

// Position bits for uniquely identifying rotations of a shape
#define POS_VALID_BIT 1     // Is this a valid position (all false to show null position)
#define POS_MIR_X_BIT 2     // X-mirrored
#define POS_MIR_Y_BIT 4     // Y-mirrored
#define POS_90DEG_BIT 8     // Rotated (transposed)

typedef unsigned char byte;

struct Cell {
  int x, y;
  byte val;

  Cell(int _x, int _y, byte _v = CELL_OFF) : x(_x), y(_y), val(_v) {}
};

struct TileGrid {
  int w = 0, h = 0, posx = 0, posy = 0;
  std::vector<byte> data;

  TileGrid(){}

  TileGrid(int width, int height) : w(width), h(height), data(std::vector<byte>(w*h, CELL_OFF)) {}

  TileGrid(int width, int height, std::vector<byte> inpdata) : w(width), h(height), data(inpdata) {}

  inline byte& at(int x, int y) {
    return data[y*w + x];
  }

  inline byte get(int x, int y) const {
    return data[y*w + x];
  }

  inline int neighbors(int x, int y) const {
    int out = 0;
    for(int i = -1; i <= 1; ++i)
    for(int j = -1; j <= 1; ++j)
    {
      if((i != 0 || j != 0) && x+i >= 0 && x+i < w && y+j >= 0 && y+j < h && (get(x+i, y+j)&CELL_ON_BIT) )
      {
        out++;
      }
    }
    return out;
  }

  bool isEquivNormal(const TileGrid &tG, bool collageCheckBitLogic = false, byte *rotationInfo = nullptr) const {
    // Unrotated case
    if(tG.w == w && tG.h == h)
    {
      for(int mir_x = 0; mir_x < 2; ++mir_x)
      for(int mir_y = 0; mir_y < 2; ++mir_y)
      {
        for(int y = 0; y < h; ++y)
        for(int x = 0; x < w; ++x)
        {
          int adjX = mir_x ? w-1-x : x,
              adjY = mir_y ? h-1-y : y;
          
          byte val1 = get(x,y),
               val2 = tG.get(adjX, adjY);

          if(
            collageCheckBitLogic
            ?
              checkBitLogic(val1, val2)
            :
              ((get(x, y)&CELL_ON_BIT) != (tG.get(adjX, adjY)&CELL_ON_BIT))
          )
          {
            goto Loop_Norm_Equiv_Failed;
          }
        }
        if(rotationInfo != nullptr)
        {
          *rotationInfo = (mir_x ? POS_MIR_X_BIT : 0) | (mir_y ? POS_MIR_Y_BIT : 0);
        }
        return true;
        Loop_Norm_Equiv_Failed:;
      }
    }

    return false;
  }

  bool isEquivRotated(const TileGrid &tG, bool collageCheckBitLogic = false, byte *rotationInfo = nullptr) const {
    // 90° rotated case
    if(tG.w == h && tG.h == w)
    {
      for(int mir_x = 0; mir_x < 2; ++mir_x)
      for(int mir_y = 0; mir_y < 2; ++mir_y)
      {
        for(int y = 0; y < h; ++y)
        for(int x = 0; x < w; ++x)
        {
          int adjX = (mir_y ? (h - 1 - y) : y),
              adjY = (mir_x ? (w - 1 - x) : x);
          
          byte val1 = get(x,y),
               val2 = tG.get(adjX, adjY);

          if(
            collageCheckBitLogic
            ?
              checkBitLogic(val1, val2)
            :
              ((get(x,y)&CELL_ON_BIT) != (tG.get(adjX, adjY)&CELL_ON_BIT))
          )
          {
            goto Loop_Rot_Equiv_Failed;
          }
        }
        if(rotationInfo != nullptr)
        {
          *rotationInfo = POS_90DEG_BIT | (mir_x ? POS_MIR_X_BIT : 0) | (mir_y ? POS_MIR_Y_BIT : 0);
        }
        return true;
        Loop_Rot_Equiv_Failed:;
      }
    }

    return false;
  }

  inline bool isEquiv(const TileGrid &tG, bool collageCheckBitLogic = false, byte *rotationInfo = nullptr) const {
    if(isEquivNormal(tG, collageCheckBitLogic, rotationInfo))
    {
      return true;
    }

    if(isEquivRotated(tG, collageCheckBitLogic, rotationInfo))
    {
      return true;
    }
    return false;
  }

  // bitmasks clamps in respect to certain cell bits. E.g., a bit mask of CELL_ON_BIT will clamp the collection of ON cells.
  TileGrid clamp(byte keepBitMask = CELL_ON_BIT | CELL_CHECKED_BIT, bool keepOthers = true) const {
    int rng_x[2] = {w-1, 0};
    int rng_y[2] = {h-1, 0};

    for(int y = 0; y < h; ++y)
    for(int x = 0; x < w; ++x)
    {
      if(data[y*w + x] & keepBitMask)
      {
        stretch(rng_x[0], rng_x[1], x);
        stretch(rng_y[0], rng_y[1], y);
      }
    }

    TileGrid out (1 + rng_x[1] - rng_x[0], 1 + rng_y[1] - rng_y[0]);
    if(out.h < 0 || out.w < 0)
    {
      ERMSG << "Tilegrid initialized with negative dimensions.\n";
    }
    int c = 0; // honestly you could just use (y-rng_y[0])*out.w+(x-rng_x[0]) instead of c, but c is more concise.
    for(int y = rng_y[0]; y <= rng_y[1]; ++y)
    for(int x = rng_x[0]; x <= rng_x[1]; ++x)
    {
      if(keepOthers || (get(x,y) & keepBitMask))
      {
        out.data[c] = get(x,y);
      }
      c++;
    }

    return out;
  }

  // advance tilegrid by one generation according to rule B3/S23
  TileGrid next() const {
    TileGrid itergrid(w+2, h+2);

    for(int y = 0; y < h; ++y)
    for(int x = 0; x < w; ++x)
    {
      if(get(x,y)&CELL_ON_BIT)
      {
        for(int i = -1; i <= 1; ++i)
        for(int j = -1; j <= 1; ++j)
        if(i != 0 || j != 0)
        {
          itergrid.at(x+i+1, y+j+1)++;
        }
      }
    }

    for(int y = 0; y < h+2; ++y)
    for(int x = 0; x < w+2; ++x)
    {
      // B3/S23
      // Survive
      if(y > 0 && y < h+1 && x > 0 && x < w+1 && data[(y-1)*w + (x-1)]&CELL_ON_BIT)
      {
        itergrid.at(x,y) = (itergrid.get(x,y) == 2 || itergrid.get(x,y) == 3) ? CELL_ON : CELL_OFF;
      }
      else // Build
      {
        itergrid.at(x,y) = (itergrid.get(x,y) == 3) ? CELL_ON : CELL_OFF;
      }
    }

    return itergrid.clamp();
  }

  TileGrid subset(int x, int y, int s_w, int s_h) const {
    TileGrid tg(s_w, s_h);
    for(int ty = 0; ty < s_h; ++ty)
    for(int tx = 0; tx < s_w; ++tx)
    {
      tg.at(tx, ty) = get(tx + x, ty + y);
    }
    return tg;
  }

  // TODO
  // should return both a rotation byte and x and y sizes. 
  byte isSubsetOf(const TileGrid& sub);

  bool contains(const TileGrid& sub) const {
    if(sub.w <= w && sub.h <= h)
    {
      for(int y = 0; y < 1 + h - sub.h; ++y)
      for(int x = 0; x < 1 + w - sub.w; ++x)
      {
        if(sub.isEquiv(subset(x, y, sub.w, sub.h)))
        {
          return true;
        }
      }
    }

    if(sub.h <= w && sub.w <= h)
    {
      for(int y = 0; y < 1 + h - sub.w; ++y)
      for(int x = 0; x < 1 + w - sub.h; ++x)
      {
        if(sub.isEquivRotated(subset(x, y, sub.h, sub.w)))
        {
          return true;
        }
      }
    }

    return false;
  }

  void setBuildBits() {
    for(int y = 0; y < h; ++y)
    for(int x = 0; x < w; ++x)
    {
      if(!(get(x,y) & CELL_ON_BIT) && neighbors(x,y) == 3)
      {
        at(x,y) |= CELL_BUILD_BIT;
      }
      else
      {
        at(x,y) &= ~CELL_BUILD_BIT;
      }
    }
  }

  void clearBuildBits() {
    for(int i = 0; i < data.size(); ++i)
    {
      data[i] &= ~CELL_BUILD_BIT;
    }
  }

private:
  bool checkBitLogic(byte v1, byte v2) const {
    if((v1 & CELL_CHECKED_BIT) == 0)
      return false;
    
    if((v1 & CELL_ON_BIT) != (v2 & CELL_ON_BIT))
      return true;
    
    if((v2 & CELL_ON_BIT) && (v2 & CELL_CHECKED_BIT))
      return true;
    
    if((v1 & CELL_BUILD_BIT) != (v2 & CELL_BUILD_BIT))
      return true;
    
    
    return false;
  }
};

struct LexiconEntry {
  std::string name;
  int count = 0;
  int period = 0;
  std::vector<TileGrid> cells;

  LexiconEntry() {}

  // Set maxPeriod to 1 or below to create a one-frame lexicon entry
  LexiconEntry(TileGrid tG, int maxPeriod = 200) {

    cells = {tG};

    for(int i = 1; i <= 200; ++i)
    {
      tG = tG.next();
      if(tG.isEquiv(cells[0]))
      {
        period = i;
        break;
      }
      else
      {
        cells.push_back(tG);
      }
    }
  
  }  

  // Require RLE parsing and are defined in loadRLE.hpp
  LexiconEntry(std::string str);
  std::string toString();
};

// Compare function based on counts of each entry 
bool cmpLexen(const LexiconEntry &a, const LexiconEntry &b)
{
  return a.count > b.count;
}
// Sorts from greatest to least count
void SortLexicon(std::vector<LexiconEntry> &llex)
{
  std::sort(llex.begin(), llex.end(), cmpLexen);
}

std::ostream& operator<<(std::ostream &os, const TileGrid &tgrid) {
  //os << "TileGrid " << tgrid.w << " x " << tgrid.h;
  for(int y = 0; y < tgrid.h; ++y) {
    os << '\n';
    for(int x = 0; x < tgrid.w; ++x) {
      byte val = tgrid.get(x,y);
      
      if(val < 8)
      {
        switch((val & (CELL_CHECKED_BIT | CELL_BUILD_BIT)) >> 1)
        {
          case 0:                     break;
          case 1: os << "\033[1;31m"; break;
          case 2: os << "\033[1;34m"; break;
          case 3: os << "\033[1;35m"; break;
        }

        if(val&CELL_ON_BIT)
        {
          os << "█";
        }
        else
        {
          os << "░";
        }
        os << "\033[0m";
      }
      else
      {
        os << "?";
      }
    }
  }
  os << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream &os, const LexiconEntry &lexen)
{
  os << "Lexicon Entry " << lexen.name << "\n";
  if(lexen.count > 0)
  {
    os << "Count: " << lexen.count << "\n";
  }
  os << "Apparent Period: " << lexen.period << "\n";

  for(int i = 0; i < lexen.period; ++i)
  {
    os << lexen.cells[i] << "\n";
  }

  return os;
}

#endif