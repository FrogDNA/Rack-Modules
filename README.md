# FrogDNA

VCV Rack modules that revolve around James Conway's "Game of Life".

## GameOfLife

### Display

Left click on an alive cell to kill it. Left click on a dead cell to revive it.

### AUDIO part:

SND out is an audio output (-5V to 5V)

CLK in regulates the Game of Life evolution. Each rising front triggers an update.

WAY is how life finds a way ;) (the input port is 1V/oct, the parameter knob controls a fine tuning)

### DATA part:

Send a High signal to LINK input. It triggers data sending.

Data are written at low clock and can be read at high clock.

Data indicate which cells were alive when LINK input was triggered. They are sent x first, then y. Sequence is as such :

(x0, y0, x1, y1, ..., xn, yn)

to decode the voltages as integers, divide the voltage by 5 and multiply it by 30 (number of cells). (positionX = voltage * NUMCELLSX / 5.0f )
