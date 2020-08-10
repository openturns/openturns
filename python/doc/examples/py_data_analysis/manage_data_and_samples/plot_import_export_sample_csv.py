# ---
# jupyter:
#   jupytext:
#     text_representation:
#       extension: .py
#       format_name: percent
#       format_version: '1.3'
#       jupytext_version: 1.5.1
#   kernelspec:
#     display_name: Python 3
#     language: python
#     name: python3
# ---

# %%
"""
Import / export a sample via a CSV file
=======================================
"""
# %% 

# %%
# In this example we are going to import and export a data sample from/to a CSV file.

# %%
from __future__ import print_function
import openturns as ot

# %%
# create a sample from a 2-d gaussian distribution
sample = ot.Normal(2).getSample(5)
sample.setDescription(['u1', 'u2'])

# %%
# write a CSV file
# warning the default separator is ';'
sample.exportToCSVFile('sample.csv', ',')

# %%
# print the content of the written file
with open('sample.csv', 'r') as f:
    for line in f.readlines():
        print(line, end='')

# %%
# read the previous CSV file
sample = ot.Sample.ImportFromCSVFile('sample.csv', ',')
print(sample)
