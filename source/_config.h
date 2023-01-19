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
  #define SEGMENT_MAX_VECTORSIZE 2000000000UL//2GB  // the maximum vector size, in bytes, to allocate.
  #define SEGMENT_LINES_BUFFERED 20                 // the size of the buffer between reads. Should be greater than the maximum dimension of a still life in the lexicon.
#endif

#define CONSOLE_LINESIZE 100
#define PROGRESSBAR_SPEED 3

#endif
