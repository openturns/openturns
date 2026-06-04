%define OT_LeaveOneOutSplitter_doc
"Leave-one-out splitter.

Generates train/test indices to split samples in train/test sets.
Each sample is used once as a test set while the remaining samples form the training set.

Parameters
----------
N : int
    Size of the set of indices in which the indices are chosen

Examples
--------
>>> import openturns as ot
>>> sample = ot.Normal().getSample(10)
>>> splitter = ot.LeaveOneOutSplitter(sample.getSize())
>>> for indicesTrain, indicesTest in splitter:
...     sampleTrain, sampleTest = sample[indicesTrain], sample[indicesTest]
"
%enddef
%feature("docstring") OT::LeaveOneOutSplitter
OT_LeaveOneOutSplitter_doc
