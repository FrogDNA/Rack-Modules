Out of date. To be updated soon...

# FrogDNA Mitosis

A VCV Rack module that revolve around James Conway's "Game of Life".

## Display

Left click on an alive cell to kill it. Left click on a dead cell to revive it.

Click on a row or column header to mute all cells in this row/column

## AUDIO part:

Audio out is an audio output (-5V to 5V)

Audio clock in regulates the Game of Life evolution. Each rising front triggers an update.

1V/oct is what you would expect

PROPHASE / METAPHASE control the relative amplitude of the harmonics (PROPHASE is the slope of the amplitude curve in the frequency domain, METAPHASE is the center of this curve).

DEAD sends a high signal whenever two successive evolution steps yield the same result, i.e. no more evolution occurs. (oscillations of period 2 or more do not trigger this output).

## DATA part:

Send a High signal to LINK input. It triggers data sending.

Data are written at low clock and can be read at high clock.

Data indicate which cells were alive when LINK input was triggered. They are sent x first, then y. Sequence is as such :

(x0, y0, x1, y1, ..., xn, yn)

The other part is to receive data. Plug Busy Out of another module into Busy in, and respectively Data Out into Data in, Data Clk out into Data Clk in.
