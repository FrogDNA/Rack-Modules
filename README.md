# FrogDNA Mitosis

A VCV Rack module that revolve around James Conway's "Game of Life".

## Display

Left click on an alive cell to kill it. Left click on a dead cell to revive it.

TODO : click on a row or column header to mute all cells in this row/column

## AUDIO part:

Audio out is an audio output (-5V to 5V)

Audio clock in regulates the Game of Life evolution. Each rising front triggers an update.

WAY is how life finds a way ;) (the input port is 1V/oct, the parameter knob controls a fine tuning)

## DATA part:

Send a High signal to LINK input. It triggers data sending.

Data are written at low clock and can be read at high clock.

Data indicate which cells were alive when LINK input was triggered. They are sent x first, then y. Sequence is as such :

(x0, y0, x1, y1, ..., xn, yn)

The other part is to receive data. Plug Busy Out of another module into Busy in, and respectively Data Out into Data in, Data Clk out into Data Clk in.
