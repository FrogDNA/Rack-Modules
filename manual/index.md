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

You can click the row and column headers to "mute" a whole row or column. Muting individual cells is not possible, and it would not have much sense given how this module outputs (more info on that [here](later). When muting a line, you will notice the header becoming darker, and the line will have a distinctive color.

When a line is muted, you will also notice a change in the scale box.

## The scale box

![alt_text](https://github.com/FrogDNA/Rack-Modules/blob/main/manual/scale_box.png "The scale box")

This box lets you mute all rows or columns producing a certain tone. When all such lines are muted, the button turns green. When only some of them are muted, the button is yellow.



