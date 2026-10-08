import meep as mp

# a cell represents a boundary that the wave will travel in
# in this case, a source at one end and watch the fields propagate down the waveguide in the x direction and travels along the y direction
# units are in micrometers
cell = mp.Vector3(16, 8, 0)

