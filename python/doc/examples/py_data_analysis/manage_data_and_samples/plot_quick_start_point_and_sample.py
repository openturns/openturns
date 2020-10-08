"""
A quick start guide to the `Point` and `Sample` classes
=======================================================
"""
# %%
# Abstract
# --------
#
# In this example, we present the `Point` and `Sample` classes, two fundamental objects in the library. We present the principles behind these classes and the way to create and use these objects. We show how to extract a row or a column with the slicing operator. We show how these objects interacts with Python variables and with the `numpy` module.

# %%
# Introduction
# ------------
#
# Two fundamental objects in the library are:
#
# * `Point`: a multidimensional point in :math:`D` dimensions (:math:`\in \mathbb{R}^D`) ;
# * `Sample`: a multivariate sample made of :math:`N` points in :math:`D` dimensions.
#

# %%
import openturns as ot
ot.Log.Show(ot.Log.NONE)

# %%
# The `Point` class
# -----------------
#
# In this section, we see how to: 
#
# * create a point in :math:`\mathbb{R}^3`, 
# * access its components, 
# * update its components.
#

# %%
# By default, points are filled with zeros. 

# %%
p = ot.Point(3)
p

# %%
# The following statement returns the value of the second component (with index 1). Python beginners should remember that Python indices start at zero. 

# %%
p[1]

# %%
# The following statements sets the second component. 

# %%
p[1] = 2
p

# %%
p.getDimension ()

# %%
# The `Sample` class
# ------------------
#
# The `Sample` class represents a multivariate sample made of :math:`N` points in :math:`\mathbb{R}^D`.
#
# * :math:`D` is the *dimension* of the sample, 
# * :math:`N` is the *size* of the sample. 
#
#
# A `Sample` can be seen as an array of with :math:`N` rows and :math:`D` columns.
#
# *Remark.* The `ProcessSample` class can be used to manage a sample of stochastic processes. 

# %%
# The script below creates a `Sample` with size :math:`N=5` and dimension :math:`D=3`.

# %%
data = ot.Sample(5, 3)
data

# %%
data.getSize()

# %%
data.getDimension()

# %%
# The following statement sets the third component (with index 2) of the fourth point (with index 3) in the `Sample`.

# %%
data [3, 2] = 32
data

# %%
# Get a row or a column of a `Sample`
# -----------------------------------
#
# As with `numpy` arrays, we can extract a row or a column with the `:` slicing operator. As a reminder for Python beginners, *slicing* is the fact of extracting a part of an array with one single statement; this avoids `for` loops and improves performance and readability. 

# %%
row = data [3, :]
row

# %%
type ( row )

# %%
column = data [:, 2]
column

# %%
type ( column )

# %%
# We see that:
#
# * the `row` is a `Point`,
# * the `column` is a `Sample`.
#
# This is consistent with the fact that, in a dimension :math:`D` `Sample`, a row is a :math:`D`-dimensional `Point`.

# %%
# The following statement extracts several columns (with indices 0 and 2) and creates a new `Sample`. 

# %%
data.getMarginal ([0 , 2])

# %%
# Create a `Point` or a `Sample` from a Python list
# -------------------------------------------------
#
# The following statement creates a `Point` from a Python list.

# %%
p1 = ot.Point ([2 , 3])
p1

# %%
p2 = ot.Point(range(2))
p2

# %%
# The first useful *Pythonism* that we will review is the *list comprehension*. This creates a list from a `for` loop. This kind of statements is often used in the the examples, so that they can be as short as possible.
#
# In the following statement, we create a point by iterating over the components of a `Point`. 

# %%
p3 = ot.Point ([i*i for i in p1])
p3

# %%
# The second useful *Pythonism* is the repetition with the `*` operator.
#
# The following statements creates a list with three 5s. 

# %%
p4 = [5] * 3
p4

# %%
# We can also create a `Sample` from a list of `Point`.

# %%
sample = ot.Sample ([p1 , p2 , p3 ])
sample

# %%
# We can loop over the points in a sample, using a list comprehension. In the following example, we compute the Euclidian norm of the points in the previous sample. 

# %%
[point.norm() for point in sample]

# %%
# We can also create a `Sample` based on a `Point`, repeated three times.

# %%
sample = ot.Sample ([p4] * 3)
sample

# %%
# A nested list of floats is the easiest way to create a non-trivial `Sample`.

# %%
sample = ot.Sample ([[0 , 1], [2, 3], [4, 5]])
sample

# %%
# Interactions with Numpy
# -----------------------
#
# The Python classes defined in Python modules are unknown to OpenTURNS and hence cannot be used by the library. This is why it is useful to know how to convert to and from more basic Python variable types, especially Numpy arrays.

# %%
# The following statement creates a `Sample` and converts it into a bidimensional Numpy `array`.

# %%
import numpy as np
sample = ot.Sample (5, 3)
array = np.array (sample)
array

# %%
type(array)

# %%
# Conversely, the following script creates a Numpy `array`, then converts it into a `Sample`.

# %%
array = 3.14 * np.ones((5 , 3))
sample = ot.Sample ( array )
sample

# %%
sample.getSize()

# %%
sample.getDimension()

# %%
# There is an ambiguous situation: a `Sample` based on several scalar values. 
#
# For example, is a `Sample` based on 5 values:
#
# * a `Sample` with size 5 in 1 dimension or
# * a `Sample` with size 1 in 5 dimensions?
#
# In order to solve the case, we can use the second input argument of the `Sample` constructor, which specifies the dimension.
#
# The following statement creates an array containing 5 values from 0 to 1.

# %%
u = np.linspace (0, 1, 5)
u

# %%
# Choice A: we create a `Sample` with size 5 in 1 dimension.

# %%
sample = ot.Sample ([[ui] for ui in u])
sample

# %%
# Choice B: we create a `Sample` with size 1 in 5 dimensions.

# %%
sample = ot.Sample ([u[i:i+5] for i in range(len(u)//5)])
sample

# %%
# If we do not set the optional `size` parameter, the library cannot solve the case and an InvalidArgumentException is generated.
#

# %%
# Generates an expected exception
# TypeError: InvalidArgumentException : Invalid array dimension: 1
# sample = ot.Sample (u)
