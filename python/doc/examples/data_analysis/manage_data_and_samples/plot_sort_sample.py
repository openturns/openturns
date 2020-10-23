"""
Sort a sample
=============
"""
# %%
# In this example we present useful methods of the `Sample` object such as marginals extraction and various sorting strategies.


# %%
from __future__ import print_function
import openturns as ot
ot.Log.Show(ot.Log.NONE)
ot.RandomGenerator.SetSeed(0)


# %%
# We start by defining the distribution of a regular non-biased dice.
dice_distribution = ot.UserDefined([[i] for i in range(1,7)])

# %%
# We consider now an experiment with two independent dices and build the corresponding random vector :
two_dices_distribution = ot.ComposedDistribution([dice_distribution]*2)

# %%
# We now build a sample of size :math:`n=5` from this distribution :
n = 5
sample = two_dices_distribution.getSample(n)
print( sample ) 

# %%
# Useful methods
# --------------

# %%
# We have access to the marginals by providing a list of the wanted indices :

# the first marginal
sample_dice1 = sample.getMarginal([0])

# the second marginal
sample_dice2 = sample.getMarginal([1])

# %%
# Suppose we are interested in the sum of the two dices. This can be done by summing the two samples `dice1` and `dice2`.
# Provided the dimensions are the same we can add samples with the `+` operator and produce a new sample :
experiment = sample_dice1 + sample_dice2

# %%
# Note that the `+=` operator is defined as well.

# %%
# We can concatenate two samples having the same size with the `stack` method :
sample_dice1.stack(sample_dice2)
print(sample_dice1)

# %%
# We can split a sample in two by giving an index (here 2).
remaining = sample_dice1.split(2)
print( sample_dice1 )
print( remaining )


# %%
# Sorting samples
# ---------------


# %%
# We can extract any marginal and sort it by ascending order by specifying the index :
sorted_marginal = sample.sort(1)
print( sorted_marginal )

# %%
# We can sort the sample in place, that is whithout creating a new sample, as well with sortInPlace. When the dimension is greater than one the sort is made according to the first marginal.
sample.sortInPlace()
print( sample )

# %%
# We can sort the rows according to the second marginal with the `sortAccordingToAComponent` : 
another_sample = sample.sortAccordingToAComponent(1)
print( another_sample )

# %%
# There is also a `sortAccordingToAComponentInPlace` method that does the same without creating a new sample.

# %%
# We can sort and remove the duplicates at the same time
print( sample_dice2 )
print( sample_dice2.sortUnique() )

# %%
# We note that the sample is smaller as expected. Sorting in place is also possible :
sample_dice2.sortUniqueInPlace()
print( sample_dice2 )

# %%
# Let's try with the sample in dimension 2 :
sampleUnique = sample.sortUnique()
print( sampleUnique )

# %%
# Nothing happens here because pairs are already unique !







