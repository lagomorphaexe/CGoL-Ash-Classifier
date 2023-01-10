#ifndef ANALYZE_H
#define ANALYZE_H

#include "search.hpp"
#include "loadRle.hpp"

// Flip Cell Checked Bit on every connected subset of tGrid that is in lexicon
TileGrid Collage(const TileGrid &tGrid, std::vector<LexiconEntry> &llex)
{
  TileGrid nbtGrid = TileGrid(tGrid.w + 2, tGrid.h + 2);
  for(int y = 0; y < tGrid.h; ++y)
  for(int x = 0; x < tGrid.w; ++x)
  {
    nbtGrid.at(x+1, y+1) = tGrid.get(x, y);
  }
  nbtGrid.setBuildBits();

  // Go over lexicon backwards.
  // Reason: in most cases lexicon will be ordered with most common ash at top. More common ash is smaller, generally.
  // Collaging with a subset of ash A first disables the possibility to collage with A, so the less common supersets are collaged first.
  // This could also have been done with size/dimension comparisons or a map of subsets, but both of those are not efficient, so this will have to do for now.
  int i = llex.size();
  while(i --> 0) 
  {
    for(const TileGrid &frame : llex[i].cells)
    {
      
      TileGrid nbframe = checkNeighborsBits(frame);
      nbframe.setBuildBits();
      // Data on rotation to flip checked bits correctly
      byte rotData;

      for(int y = 0; y < 1 + nbtGrid.h - nbframe.h; ++y)
      for(int x = 0; x < 1 + nbtGrid.w - nbframe.w; ++x)
      {
        if(nbframe.isEquivNormal(nbtGrid.subset(x, y, nbframe.w, nbframe.h), true, &rotData)) // Are checked nbframe bits in nbtGrid subset, Also checks for build bit equivalency.
        {
          llex[i].count++;
          for(int wry = 0; wry < nbframe.h; ++wry)
          for(int wrx = 0; wrx < nbframe.w; ++wrx)
          {
            int adjPosX = ((rotData & POS_MIR_X_BIT) ? (nbframe.w - 1 - wrx) : wrx);
            int adjPosY = ((rotData & POS_MIR_Y_BIT) ? (nbframe.h - 1 - wry) : wry);
            if(nbframe.get(adjPosX, adjPosY) & CELL_CHECKED_BIT)
            {
              nbtGrid.at(x + wrx, y + wry) |= CELL_CHECKED_BIT;
            }
          }
        }
      }

      for(int y = 0; y < 1 + nbtGrid.h - nbframe.w; ++y)
      for(int x = 0; x < 1 + nbtGrid.w - nbframe.h; ++x)
      {
        if(nbframe.isEquivRotated(nbtGrid.subset(x, y, nbframe.h, nbframe.w), true, &rotData))
        {
          llex[i].count++;
          for(int wry = 0; wry < nbframe.w; ++wry)
          for(int wrx = 0; wrx < nbframe.h; ++wrx)
          {
            int vecPosX = ((rotData & POS_MIR_X_BIT) ? (nbframe.w - 1 - wry) : wry);
            int vecPosY = ((rotData & POS_MIR_Y_BIT) ? (nbframe.h - 1 - wrx) : wrx);
            if(nbframe.get(vecPosX, vecPosY) & CELL_CHECKED_BIT)
            {
              nbtGrid.at(x + wrx, y + wry) |= CELL_CHECKED_BIT;
            }
          }
        }
      }
      
    }
  }
  //nbtGrid.clearBuildBits();
  return nbtGrid.clamp(CELL_ON_BIT);
}

// Classify cell as either in lexicon or as new shape
void AnalyzeCell(TileGrid &tGrid, std::vector<LexiconEntry> &llex, int x, int y, std::vector<TileGrid> *newAshDump)
{
  std::vector<Cell> cells = {Cell(x, y, CELL_ON)};
  TileGrid shape;

  /*// Basic check (removed bc of possibilities of mishandling multi-island ash)
  expandOutMutablyVec(shape, tGrid, cells);
  for(LexiconEntry le : llex)
  {
    for(TileGrid frame : le.cells)
    {
      if(shape.isEquiv(frame))
      {
        le.count++;
        return;
      }
    }
  }*/

  // Check involving all nearby cells (even resting, >= 4n ones)
  expandOutMutablyVec(shape, tGrid, cells, true);
  for(LexiconEntry &le : llex)
  {
    for(const TileGrid &frame : le.cells)
    {
      if(shape.clamp().isEquiv(frame))
      {
        le.count++;
        return;
      }
    }
  }

  expandOutMutablyVec(shape, tGrid, cells, true, 2);

  // Collage shape with existing entries
  shape = Collage(shape, llex);
  
  bool fullyCollaged = true;

  for(int y = 0; y < shape.h; ++y)
  for(int x = 0; x < shape.w; ++x)
  {
    // Not sure if should include neighbor check here as well. Probably not since Collage should prevent against it.
    if((shape.get(x,y)&CELL_CHECKED_BIT) == 0 && 
       (shape.get(x,y)&CELL_ON_BIT) == 1 )
    {
      fullyCollaged = false;
    }
  }
  if(fullyCollaged)
  {
    return;
  }

  // check if shape is a subset of any lex entry
  /*std::vector<LexiconEntry> shapeLE = {LexiconEntry(shape, 1)};
  for(LexiconEntry &le : llex)
  {
    for(const TileGrid &frame : le.cells)
    {
    }
  }*/


  // New shape assumed

  //shape = shape.clamp();
  //DMESG << tGrid.posx;

  // Only 
  bool isNeverSeenBefore = true;
  if(newAshDump != NULL)
  {
    for(const TileGrid& newAsh : *newAshDump)
    {
      if(newAsh.isEquiv(shape))
      {
        isNeverSeenBefore = false;
        break;
      }
    }
  }
  if(isNeverSeenBefore)
  {
    clrln(g_linesize);
    std::cout << "Possible new shape discovered near x:" << tGrid.posx + x << ", y:" << tGrid.posy + y;
    std::cout << shape;
    std::cout << ToRLE(shape) << '\n';
    newAshDump->push_back(shape);
  }
}

#endif
