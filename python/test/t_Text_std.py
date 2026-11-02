#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()

# Instantiate one distribution object
dim = 2
meanPoint = ot.Point(dim, 1.0)
meanPoint[0] = 0.5
meanPoint[1] = -0.5
sigma = ot.Point(dim, 1.0)
sigma[0] = 2.0
sigma[1] = 3.0
R = ot.CorrelationMatrix(dim)
for i in range(1, dim):
    R[i, i - 1] = 0.5

distribution = ot.Normal(meanPoint, sigma, R)

# Test for sampling
size = 30
sample = distribution.getSample(size)

# Create an empty graph
myGraph = ot.Graph("Normal sample", "x1", "x2")
myGraph.setLegendPosition("upper right")

# Display extrema indices
x1 = [x[0] for x in sample[:, 0]]
x2 = [x[0] for x in sample[:, 1]]
idx = [0] * 4
idx[0] = x1.index(min(x1))
idx[1] = x1.index(max(x1))
idx[2] = x2.index(min(x2))
idx[3] = x2.index(max(x2))

sample.add(distribution.getSample(5))
labels = ot.Description(sample.getSize(), ".")
for i in range(4):
    labels[idx[i]] = str(idx[i])

position = ot.Description(sample.getSize(), "top")
position[idx[0]] = "right"
position[idx[1]] = "left"
position[idx[2]] = "top"
position[idx[3]] = "bottom"

myText = ot.Text(sample, labels)
myText.setColor("red")
myText.setTextPositions(position)
myText.setTextSize(1.0)

myGraph.add(myText)

# Deterministic coverage of Text constructors and accessors (no prints)
data = ot.Sample([[0.0, 0.0], [1.0, 1.0]])
labels = ot.Description(["a", "b"])
defaultText = ot.Text()
assert defaultText.getTextAnnotations().getSize() == 0
text = ot.Text(data, labels)
assert text.getTextAnnotations() == labels
assert text.getTextPositions() == ot.Description(["top", "top"])
ott.assert_almost_equal(text.getTextSize(), 0.75)
ott.assert_almost_equal(text.getRotation(), 0.0)
textLeft = ot.Text(data, labels, "left", "legend")
assert textLeft.getTextPositions() == ot.Description(["left", "left"])
assert "Text" in repr(textLeft)
twoSamples = ot.Text(ot.Sample([[0.0], [1.0]]), ot.Sample([[2.0], [3.0]]), labels)
ott.assert_almost_equal(twoSamples.getData(), [[0.0, 2.0], [1.0, 3.0]])
twoPoints = ot.Text([0.0, 1.0], [2.0, 3.0], labels)
ott.assert_almost_equal(twoPoints.getData(), [[0.0, 2.0], [1.0, 3.0]])
assert ot.Text.IsValidTextPosition("top")
assert ot.Text.IsValidTextPosition("bottom")
assert ot.Text.IsValidTextPosition("left")
assert ot.Text.IsValidTextPosition("right")
assert not ot.Text.IsValidTextPosition("nowhere")
with ott.assert_raises(Exception):
    ot.Text(data, labels, "nowhere")
with ott.assert_raises(Exception):
    ot.Text(data, ot.Description(["only-one"]))
with ott.assert_raises(Exception):
    ot.Text(ot.Sample(2, 3), labels)
with ott.assert_raises(Exception):
    ot.Text(
        ot.Sample([[0.0], [1.0]]),
        ot.Sample([[2.0], [3.0], [4.0]]),
        ot.Description(["a", "b", "c"]),
    )
with ott.assert_raises(Exception):
    text.setTextAnnotations(ot.Description(["only-one"]))
with ott.assert_raises(Exception):
    text.setTextPositions(ot.Description(["top"]))
with ott.assert_raises(Exception):
    text.setTextPositions(ot.Description(["top", "nowhere"]))
text.setTextPositions(ot.Description(["left", "right"]))
assert text.getTextPositions() == ot.Description(["left", "right"])
text.setTextSize(2.5)
ott.assert_almost_equal(text.getTextSize(), 2.5)
text.setRotation(0.5)
ott.assert_almost_equal(text.getRotation(), 0.5)

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("text_std.xml"))
study.add("t", text)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("text_std.xml"))
loadedStudy.load()
loaded = ot.Text()
loadedStudy.fillObject("t", loaded)
assert loaded.getTextAnnotations() == text.getTextAnnotations()
assert loaded.getTextPositions() == text.getTextPositions()
ott.assert_almost_equal(loaded.getTextSize(), text.getTextSize())
ott.assert_almost_equal(loaded.getRotation(), text.getRotation())
os.remove("text_std.xml")
