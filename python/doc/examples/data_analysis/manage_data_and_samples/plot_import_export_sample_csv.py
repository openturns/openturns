"""
Import / export a sample via a CSV file
=======================================
"""
# %%
# In this example we are going to import and export a data sample from/to a CSV file.

# %%
import openturns as ot
ot.Log.Show(ot.Log.NONE)

# %%
# Create a sample from a 2-d gaussian distribution.
sample = ot.Normal(2).getSample(5)
sample.setDescription(['u1', 'u2'])

# %%
# Write a CSV file.
#
# Warning: the default separator is ';'.
sample.exportToCSVFile('sample.csv', ',')

# %%
# Print the content of the written file.
with open('sample.csv', 'r') as f:
    for line in f.readlines():
        print(line, end='')

# %%
# Read the previous CSV file.
sample = ot.Sample.ImportFromCSVFile('sample.csv', ',')
print(sample)
