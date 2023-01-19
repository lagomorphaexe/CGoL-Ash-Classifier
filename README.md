<h1>General information</h1>
This utility allows for the analysis of the distribution of still lifes from random ash. To compile from source, run `make all`. If golly is not installed, run `make install` beforehand.

Run each command with the `--help` flag to get more info on usage.
- `./genGrid      `	- generates a RLE file of random cells, at 50% density.
- `./analyzeRLE   `	- analyzes a RLE file that is in a stable state.
- `./updateLexicon`	- updates the lexicon file (requires user input).

To run a comprehensive generation and search, run `sh main.sh`. The parameters in the bash file are mostly self-explanatory or can be derived from the usage of each C-compiled binary. The first half of this file serves to increase a counter each time the script is run. To set this counter to a desired value, run `sh main.sh NUM`.

<h1>Lexicon Formatting</h1>
For ash which is present in golly's lexicon, the name of that ash is used.
For other ash, it is a combination of such known ashes or given a new name
entirely.

Notations:
* *\[Cis/Trans\]*: Specifies whether two shapes are oriented in the same way (generally in a Cis configuration they have the longer straight line of cells on the same side)
* *\[Pro/Meta/Para\]*: Specifies whether two shapes are joined either in front of each other or side by side, or from the back. Para can be used for parallel, and meta for perpendicular joinings.
* *Long*: Specifies that a shape is extended from its predecessor. Can be chained, and for chains of n>2 the notation *Long^n* is used.
* *On/And*: Specifies an induction coil system.
* *Shifted*: Specifies a shift from the normal unnamed unshifted system.
* *R-*: Specifies a common shape has an extra cell on its side somewhere, e.g. R-Bee. A nonstandard cell may be called *R'-*, when more than one is present use *2R-*
* Conjunction Operators
  * *-J-*: Specifies two shapes are joined, going from front to back. E.g. *Eater -J-* implies the joining occurs on the back tail of the eater. If joining occurs with a cell in between, use *-J.-* notation. If the joined life is reversed, use *-J:-*.
  * *-S-*: Specifies siamese (overlap) between the two shapes. If in multiple locations, *-SS-* may be used to differentiate. Again *-S:-* has a similar use.
  * *-X-*: Specifies the shapes are overlapped in some way but are missing some cells. In practice a catch-all for weird still life.
  * *-T-*: Joined by a 3x1 tail. *-T2-* denotes a 2x1 tail, etc.
* Shift Operators
  * *(U)*: When the placed in position with the first shape on the left facing upwards, the other shape is *up* from an unshifted position. Note these go after Cis/Trans notation
  * *(D)*: Same as *(U)* but a *down* position.

<h1>TODO</h1>

* All: Optimize TileGrid toString to return shortest possible RLE String
* All: Have SaveLexicon not write duplicate entries
* Stats: Add filter option based on RLE subset
