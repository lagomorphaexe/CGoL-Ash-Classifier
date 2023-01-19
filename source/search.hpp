#ifndef SEARCH_H
#define SEARCH_H

#include "gol.hpp"
#include "bits.hpp"

inline bool cellVecContainsPos(const std::vector<Cell>& vec, int x, int y){
  for(int i = 0; i < vec.size(); ++i)
  {
    if(vec[i].x == x && vec[i].y == y)
    {
      return true;
    }
  }
  return false;
}

// Assumes tileGrid x,y is CELL_ON
// Copies expanded form of x,y in tileGrid into dest
// May be used even if some bits have CELL_CHECKED_BIT flipped.
void expandOutImmutably(TileGrid& dest, const TileGrid& tileGrid, int x, int y, int dist = 1)
{
  // Get all &3n-connected cells
  std::vector<Cell> expand = {Cell(x, y, 1)};
  std::vector<Cell> empty = {};

  // METHOD 1: empty vector, const tileGrid
  for(int chk = 0; chk < expand.size(); ++chk)
  {
    int tx = expand[chk].x,
        ty = expand[chk].y;

    // Push back all valid cells in radius 1
    for(int i = -dist; i <= dist; ++i)
    for(int j = -dist; j <= dist; ++j)
    if(i != 0 || j != 0 && tx+i >= 0 && ty+j >= 0 && tx+i < tileGrid.w && ty+j < tileGrid.h)
    {
      if( ! cellVecContainsPos(expand, tx+i, ty+j) && ! cellVecContainsPos(empty, tx+i, ty+j))
      {
        if( tileGrid.get(tx+i, ty+j)&CELL_ON_BIT == CELL_ON)
        {
          expand.push_back(Cell(tx+i, ty+j, CELL_ON));
        }
        else if(tileGrid.neighbors(tx+i, ty+j) == 3)
        {
          expand.push_back(Cell(tx+i, ty+j, CELL_OFF | CELL_BUILD_BIT));
        }
        else
        {
          empty.push_back(Cell(tx+i, ty+j, CELL_OFF));
        }
      }
    }
  }

  int rng_x[2] = {x,x};
  int rng_y[2] = {y,y};
  for(int i = 0; i < expand.size(); ++i)
  {
    if(expand[i].val > 0)
    {
      stretch(rng_x[0], rng_x[1], expand[i].x);
      stretch(rng_y[0], rng_y[1], expand[i].y);
    }
  }

  dest = TileGrid(1 + rng_x[1] - rng_x[0], 1 + rng_y[1] - rng_y[0]);
  for(int i = 0; i < expand.size(); ++i)
  {
    if(expand[i].val > 0) // Removing would increase performance but would probably introduce bounds issues
    {
      dest.at(expand[i].x - rng_x[0], expand[i].y - rng_y[0]) = expand[i].val;
    }
  }
}

// Assumes tileGrid x,y is CELL_ON
// Copies expanded form of x,y in tileGrid into dest
// turns on the checked bit of tested cells in the process, so can only be used once.
// Should be slightly more efficient as there is no checked vector and vectors are not searched each time.
void expandOutMutablyVec(TileGrid& dest, TileGrid& tileGrid, std::vector<Cell>& known, bool above3N = false, int dist = 1)
{
  // flip bit of all neighboring cells
  for(int i = 0; i < known.size(); ++i)
  {
    tileGrid.at(known[i].x, known[i].y) |= CELL_CHECKED_BIT;
  }

  // iterate thru vector
  for(int chk = 0; chk < known.size(); ++chk)
  {
    int tx = known[chk].x,
        ty = known[chk].y;

    // Push back all valid cells in radius dist
    for(int i = -dist; i <= dist; ++i)
    for(int j = -dist; j <= dist; ++j)
    if((i != 0 || j != 0) && tx+i >= 0 && ty+j >= 0 && tx+i < tileGrid.w && ty+j < tileGrid.h)
    {
      if( (tileGrid.at(tx+i, ty+j)&CELL_CHECKED_BIT) == 0 )
      {
        if( tileGrid.at(tx+i, ty+j)&CELL_ON_BIT )
        {
          known.push_back(Cell(tx+i, ty+j, CELL_ON));
        }
        else
        {
          int nb = tileGrid.neighbors(tx+i, ty+j);
          if( nb == 3 )
          {
            known.push_back(Cell(tx+i, ty+j, CELL_OFF | CELL_BUILD_BIT));
          }
          else if (above3N && nb > 3)
          {
            known.push_back(Cell(tx+i, ty+j, CELL_OFF));
          }
        }
        tileGrid.at(tx+i, ty+j) |= CELL_CHECKED_BIT;
      }
    } 
  }

  int rng_x[2] = {tileGrid.w, 0};
  int rng_y[2] = {tileGrid.h, 0};
  for(int i = 0; i < known.size(); ++i)
  {
    if(known[i].val > 0)
    {
      stretch(rng_x[0], rng_x[1], known[i].x);
      stretch(rng_y[0], rng_y[1], known[i].y);
    }
  }

  dest = TileGrid(1 + rng_x[1] - rng_x[0], 1 + rng_y[1] - rng_y[0]);
  for(int i = 0; i < known.size(); ++i)
  {
    if(known[i].val > 0)
    {
      dest.at(known[i].x - rng_x[0], known[i].y - rng_y[0]) = known[i].val;
    }
  }
}

// Accepts a single point instead of a vector
inline void expandOutMutably(TileGrid& dest, TileGrid& tileGrid, int x, int y, bool above3N = false, int dist = 1)
{
  std::vector<Cell> cellvec = {Cell(x, y, tileGrid.data[y*tileGrid.w + x])};
  expandOutMutablyVec(dest, tileGrid, cellvec, above3N, dist);
}

TileGrid checkNeighborsBits(const TileGrid& tGrid)
{
  TileGrid out(tGrid.w + 2, tGrid.h + 2);

  for(int y = 0; y < tGrid.h; ++y)
  for(int x = 0; x < tGrid.w; ++x)
  {
    if(tGrid.get(x,y) & CELL_ON_BIT)
    {
      out.at(x+1,y+1) = tGrid.get(x,y);
      for(int i = -1; i <= 1; ++i)
      for(int j = -1; j <= 1; ++j)
      {
        out.at(x+i+1, y+j+1) |= CELL_CHECKED_BIT;
      }
    }
  }

  return out.clamp();
}


#endif
