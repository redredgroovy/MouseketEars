#!python3

import math
import numpy
import sys

gRingPixels =  [32, 24, 16,  8, 1]
gRingRadii =   [40, 30, 20, 10, 0]
gMatrixWidth = 17

def main():
  if len(gRingPixels) != len(gRingRadii):
    print("gRingPixels and gRingRadii arrays must contain the same number of elements")
    sys.exit(1)

  maxRadius = max(gRingRadii)

  x_coords = "const uint8_t HW_V_COORDS_X[] = {\n"
  y_coords = "const uint8_t HW_V_COORDS_Y[] = {\n"

  for index in range(0, len(gRingPixels)):
    x_values = []
    y_values = []

    for pixel in range(0, gRingPixels[index]):
      angle = pixel / gRingPixels[index] * math.pi * 2
      x_length = 0 - round(gRingRadii[index] * math.sin(angle)) 
      y_length = round(gRingRadii[index] * math.cos(angle)) 
      x_values.append(round(numpy.interp(x_length, [-maxRadius,maxRadius], [0,gMatrixWidth-1])))
      y_values.append(round(numpy.interp(y_length, [-maxRadius,maxRadius], [0,gMatrixWidth-1])))

    x_coords += "\t" + ", ".join(str(x) for x in x_values) + ",\n"
    y_coords += "\t" + ", ".join(str(y) for y in y_values) + ",\n"

  print(x_coords + "};")
  print(y_coords + "};")

if __name__ == "__main__":
  main()
