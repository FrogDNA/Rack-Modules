# FrogDNA Mitosis

Chaotic sequencer based on James Conway's Game of Life

![alt text](https://github.com/FrogDNA/Rack-Modules/blob/main/manual/mitosis.png "Mitosis module in full")

## Introduction, or "What this module does"

This module is a sequencer. Its main outputs, on the big right panel, deliver 1V/Oct outputs from -4V to +5V. The reference point is assumed to be 0V = C4, but you are obviously free to use any other reference. Those outputs are determined by the state of the grid. Rows and columns are processed independantly from one another. Pink-background panels and buttons modify row outputs, and olive-background panel and buttons modify column outputs.

The grid is comprised of 108 rows and 108 columns. Each row and column corresponds to 1 semitone. The outputs are assigned in ascending order, each row containing at least 1 living cell will generate an output corresponding to its number. For instance, if row 0 contains at least 1 living cell, first output will hold the value -4V. If row 1-10 contain no living cell, and row 11 contains at least one, output 2 will hold the value -3V.

## The grid

![alt text](https://github.com/FrogDNA/Rack-Modules/blob/main/manual/grid.png "Mitosis module grid")

This is the grid. Each darker cell is dead, and each lighter cell is alive. You can find more info about the game of life [there](https://www.conwaylife.com/wiki/Conway%27s_Game_of_Life).
Click on a dead cell to make it alive, click on a living cell to kill it.

You can click the row and column headers to "mute" a whole row or column. Muting individual cells is not possible, and it would not have much sense given how this module outputs. When muting a line, you will notice the header becoming darker, and the line will have a distinctive color.

When a line is muted, you will also notice a change in the scale box. Note that muting a line does not affect the game of life evolution. Muted living cells continue to behave like other living cells to this respect, and the same is true for muted dead cells.

## The seed

![alt_text](https://github.com/FrogDNA/Rack-Modules/blob/main/manual/seed.png "The seed")

Push the button or trigger the input to load the grid with a random pattern. The light turns on to indicate that this is the last loaded state.

## The saves

![alt_text](https://github.com/FrogDNA/Rack-Modules/blob/main/manual/save_space.png "The save/load space")

Trigger the input with the save icon to save the current grid state, or trigger the other input to load previously saved state. The light turns on to indicate the last save/load space used.

## The scale box

![alt_text](https://github.com/FrogDNA/Rack-Modules/blob/main/manual/scale_box.png "The scale box")

This box lets you mute all rows or columns producing a certain tone. When all such lines are muted, the button turns green. When only some of them are muted, the button is yellow.

## The output area

![alt_text](https://github.com/FrogDNA/Rack-Modules/blob/main/manual/output_area.png "The output area")

The pink panel corresponds to row outputs. The olive panel to column outputs. They work the same way. In each panel, there are 10 rows of outputs. Each row has 2 ports. The leftmost port holds the 1V/Oct output. The rightmost port holds 10 V if the port is holding an actual output, 0V if not. This is especially important in a very sparse grid or if many cells are muted. In that case, it could be that less than 10 lines are holding at least 1 living cell. In that case, less than 10 outputs will be holding a 1V/oct.

The lights panel on top indicate how many outputs actually hold a meaningful voltage. If 3 lights are on, it means for instance that the 3 top outputs hold a meaningful voltage, which is 1V/Oct on the left port and 10V on the right port. The other outputs will have 0V on the left port and 0V on the right port.

## The top bar

![alt_text](https://github.com/FrogDNA/Rack-Modules/blob/main/manual/top_bar.png "The top bar")

### Clock input

Trigger this to update the grid according to the game of life rules

### Reset input

Trigger this to reset the grid to the last loaded or saved state.

### Dead output

This output will hold 10V if the grid has reached an already reached state, within the step limit defined by the Dead parameter.
!#alt_text](https://github.com/FrogDNA/Rack-Modules/blob/main/manual/dead_parameter.png "The dead parameter")

### Loop switch

If off (default), the grid is limited by its edges. If on, left and right edges communicate, the same for top and bottom edges.

### CV inputs

These outputs offset the first line/column to be taken into account for the vOct output. For instance, if sending 1V in the CV input for rows, the first row to be checked for a living cell will be row 12.

