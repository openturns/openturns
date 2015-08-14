#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    print("Named colors=", Drawable.GetValidColors())
    print("RGB colors (31, 63, 127)=", Drawable.ConvertFromRGB(31, 63, 127))
    print("RGB colors (0.1, 0.2, 0.3)=",
          Drawable.ConvertFromRGB(0.1, 0.2, 0.3))
    print("RGBA colors (31, 63, 127, 191)=",
          Drawable.ConvertFromRGBA(31, 63, 127, 191))
    print("RGBA colors (0.1, 0.2, 0.3, 0.4)=",
          Drawable.ConvertFromRGBA(0.1, 0.2, 0.3, 0.4))
    print("HSV colors (215.0, 0.2, 0.3)=",
          Drawable.ConvertFromHSV(215.0, 0.2, 0.3))
    print("HSVA colors (215.0, 0.2, 0.3, 0.4)=",
          Drawable.ConvertFromHSVA(215.0, 0.2, 0.3, 0.4))
    print("HSV (215.0, 0.2, 0.3) to RGB=",
          Drawable.ConvertFromHSVIntoRGB(215.0, 0.2, 0.3))
    print("Line styles=", Drawable.GetValidLineStyles())
    print("Point styles=", Drawable.GetValidPointStyles())
    print("Fill styles=", Drawable.GetValidFillStyles())

except:
    import sys
    print("t_Drawable_std.py", sys.exc_info()[0], sys.exc_info()[1])
