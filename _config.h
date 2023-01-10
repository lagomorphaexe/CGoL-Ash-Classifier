#ifndef CONFIG_H
#define CONFIG_H
/* This file contains parameters for the ash classifier program. */
/* In future releases, these may be implemented as command line parameters */

// This define lets GoL displays use filled unicode rectangles instead of standard ascii characters.
#define TILEGRID_UNICODES

// This define enables segmented mode, where input files are read in multiple pass throughs instead of all at once.
// Sometimes, if the buffer is too small, this mode may identify recognized shapes as new.
#define SEGMENTED_READ

// Segment options
#ifdef SEGMENTED_READ
  extern const long segment_maxVectorSize = 1'000'000'000; // the maximum vector size, in bytes, to allocate.
  extern const int  segment_linesBuffered = 20;            // the size of the buffer between reads. Should be greater than the maximum dimension of a still life in the lexicon.
#endif

extern const int g_linesize = 100;

#endif