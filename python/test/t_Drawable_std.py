#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

print("Named colors=", ot.Drawable.GetValidColors())
hexa = ot.Drawable.ConvertFromRGB(31, 63, 127)
assert hexa == "#1f3f7f"
#
hexa = ot.Drawable.ConvertFromRGB(0.1, 0.2, 0.3)
assert hexa == "#1a334d"
#
hexa = ot.Drawable.ConvertFromRGBA(31, 63, 127, 191)
assert hexa == "#1f3f7fbf"
#
hexa = ot.Drawable.ConvertFromRGBA(0.1, 0.2, 0.3, 0.4)
assert hexa == "#1a334d66"
#
hexa = ot.Drawable.ConvertFromHSV(215.0, 0.2, 0.3)
assert hexa == "#3d444d"
#
hexa = ot.Drawable.ConvertFromHSVA(215.0, 0.2, 0.3, 0.4)
assert hexa == "#3d444d66"
#
rgb = ot.Drawable.ConvertFromHSVIntoRGB(215.0, 0.2, 0.3)
ott.assert_almost_equal(rgb, [0.24, 0.265, 0.3])
#
hsv = ot.Drawable.ConvertFromRGBIntoHSV(44.0 / 255.0, 160.0 / 255.0, 72.0 / 255.0)
ott.assert_almost_equal(hsv, (134.48275862068965, 0.725, 0.6274509803921569))
#
hsv = ot.Drawable.ConvertFromRGBIntoHSV(44.0 / 255.0, 72.0 / 255.0, 160.0 / 255.0)
ott.assert_almost_equal(hsv, (225.51724137931038, 0.725, 0.6274509803921569))
#
hsv = ot.Drawable.ConvertFromRGBIntoHSV(160.0 / 255.0, 44.0 / 255.0, 72.0 / 255.0)
ott.assert_almost_equal(hsv, (345.51724137931035, 0.725, 0.6274509803921569))
#
print("BuildDefaultPalette=", ot.Drawable.BuildDefaultPalette(10))
print("BuildRainbowPalette=", ot.Drawable.BuildRainbowPalette(10))
print("BuildTableauPalette=", ot.Drawable.BuildTableauPalette(10))
print("BuildDefaultPalette=", ot.Drawable.BuildDefaultPalette(20))
print("BuildTableauPalette=", ot.Drawable.BuildTableauPalette(20))
#
print("Line styles=", ot.Drawable.GetValidLineStyles())
print("Point styles=", ot.Drawable.GetValidPointStyles())
print("Fill styles=", ot.Drawable.GetValidFillStyles())
#
# Get/set the ResourceMap
name = ot.ResourceMap.GetAsString("Drawable-DefaultPaletteName")
assert name == "Tableau"
#
palette = ot.Drawable.BuildDefaultPalette(4)
assert palette == ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728"]
#
ot.ResourceMap.SetAsString("Drawable-DefaultPaletteName", "Rainbow")
palette = ot.Drawable.BuildDefaultPalette(4)
assert palette == ["#ff0000", "#ccff00", "#00ff66", "#0066ff"]
#
ot.ResourceMap.SetAsString("Drawable-DefaultPaletteName", "Tableau")
palette = ot.Drawable.BuildDefaultPalette(4)
assert palette == ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728"]
