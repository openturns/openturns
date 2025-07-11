"""
A quick start guide to the `Point` and `Sample` classes
=======================================================
"""

# %%
# Abstract
# --------
#
# In this example, we present the :class:`~openturns.Point` and :class:`~openturns.Sample` classes, two fundamental objects in the library.
# We present the principles behind these classes and the way to create and use these objects.
# We show how to extract a row or a column with the slicing operator.
# We show how these objects interact with Python variables and with the Numpy module.

# %%
# Introduction
# ------------
#
# Two fundamental objects in the library are:
#
# * :class:`~openturns.Point`: a multidimensional point in :math:`d` dimensions (:math:`\in \Rset^d`) ;
# * :class:`~openturns.Sample`: a multivariate sample made of :math:`n` points in :math:`d` dimensions.
#

# %%
import numpy as np
import openturns as ot


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
# The following statement returns the value of the second component (with index 1).
# Python beginners should remember that Python indices start at zero.

# %%
p[1]

# %%
# The following statement sets the second component.

# %%
p[1] = 2
p

# %%
p.getDimension()

# %%
# The `Sample` class
# ------------------
#
# The :class:`~openturns.Sample` class represents a multivariate sample made of :math:`\sampleSize` points in :math:`\Rset^d`.
#
# * :math:`d` is the *dimension* of the sample,
# * :math:`n` is the *size* of the sample.
#
#
# A `Sample` can be seen as an array of with :math:`n` rows and :math:`d` columns.
#
# *Remark.* The :class:`~openturns.ProcessSample` class can be used to manage a sample of stochastic processes.

# %%
# The script below creates a `Sample` with size :math:`n=5` and dimension :math:`d=3`.

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
data[3, 2] = 32
data

# %%
# Notice that the rendering is different when we use the `print` statement.

# %%
print(data)

# %%
# We can customize the format used to print the floating point numbers
# with the `Sample-PrintFormat` key of the :class:`~openturns.ResourceMap`.

# %%
# Get a row or a column of a `Sample`
# -----------------------------------
#
# As with `numpy` arrays, we can extract a row or a column with the `:` slicing operator.
# As a reminder for Python beginners, *slicing* is the fact of extracting a part of an array with one single statement; this avoids `for` loops and improves performance and readability.

# %%
row = data[3, :]
row

# %%
print(type(row))

# %%
column = data[:, 2]
column

# %%
print(type(column))

# %%
# We see that:
#
# * the `row` is a `Point`,
# * the `column` is a `Sample`.
#
# This is consistent with the fact that, in a dimension :math:`d`, a row is a :math:`d`-dimensional `Point`.

# %%
# The following statement extracts several columns (with indices 0 and 2) and creates a new `Sample`.

# %%
data.getMarginal([0, 2])

# %%
# Set a row or a column of a `Sample`
# -----------------------------------

# %%
# Slicing can also be used to set a `Sample` row or column.

# %%
sample = ot.Sample([[1.0, 2.0], [3.0, 4.0], [5.0, 6.0]])
sample

# %%
# Set the third row: this must be a `Point` or must be convertible to.
p = [8.0, 10.0]
sample[2, :] = p
sample

# %%
# Set the second column: this must be a `Sample` or must be convertible to.
sample = ot.Sample([[1.0, 2.0], [3.0, 4.0], [5.0, 6.0]])
s = ot.Sample([[3.0], [5.0], [7.0]])
sample[:, 1] = s
sample

# %%
# Sometimes, we want to set a column with a list of floats.
# This can be done using the :meth:`~openturns.Sample.BuildFromPoint` static method.
sample = ot.Sample([[1.0, 2.0], [3.0, 4.0], [5.0, 6.0]])
s = ot.Sample.BuildFromPoint([3.0, 5.0, 7.0])
sample[:, 1] = s
sample

# %%
# Create a `Point` or a `Sample` from a Python list
# -------------------------------------------------
#
# The following statement creates a `Point` from a Python list.

# %%
p1 = ot.Point([2, 3])
p1

# %%
p2 = ot.Point(range(2))
p2

# %%
# The first useful *Pythonism* that we will review is the *list comprehension*. This creates a list from a `for` loop.
# This kind of statements is often used in the examples, so that they can be as short as possible.
# In the following statement, we create a point by iterating over the components of a `Point`.

# %%
p3 = ot.Point([i * i for i in p1])
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
sample = ot.Sample([p1, p2, p3])
sample

# %%
# We can loop over the points in a sample, using a list comprehension. In the following example, we compute the Euclidian norm of the points in the previous sample.

# %%
[point.norm() for point in sample]

# %%
# We can also create a `Sample` based on a `Point`, repeated three times.

# %%
sample = ot.Sample([p4] * 3)
sample

# %%
# A nested list of floats is the easiest way to create a non-trivial `Sample`.

# %%
sample = ot.Sample([[0, 1], [2, 3], [4, 5]])
sample

# %%
# Interactions with Numpy
# -----------------------
#
# Classes defined in pure Python modules cannot be used by the library.
# This is why it is useful to know how to convert to and from more basic Python variable types, especially Numpy arrays.

# %%
# The following statement creates a :class:`~openturns.Sample` and converts it into a bidimensional Numpy `array`.

# %%
sample = ot.Sample(5, 3)
array = np.array(sample)
array

# %%
print(type(array))

# %%
# Conversely, the following script creates a Numpy `array`, then converts it into a :class:`~openturns.Sample`.

# %%
array = 3.14 * np.ones((5, 3))
sample = ot.Sample(array)
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
u = np.linspace(0, 1, 5)
u

# %%
# Choice A: we create a `Sample` with size 5 in 1 dimension.

# %%
sample = ot.Sample([[ui] for ui in u])
sample

# %%
# Choice B: we create a `Sample` with size 1 in 5 dimensions.

# %%
sample = ot.Sample([u[i : i + 5] for i in range(len(u) // 5)])
sample

# %%
# When there is an ambiguous case, the library cannot solve the
# issue and an `InvalidArgumentException` is generated.

# %%
# More precisely, the code:
#
# .. code-block::
#
#     sample = ot.Sample(u)
#

# %%
# produces the exception:
#
# .. code-block::
#
#     TypeError: InvalidArgumentException : Invalid array dimension: 1
#

# %%
# In order to solve that problem, we can use the :meth:`~openturns.Sample.BuildFromPoint`
# static method.
sample = ot.Sample.BuildFromPoint([ui for ui in u])
sample
