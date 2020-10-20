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
Save/load a study
=================
"""
# %% 

# %%
# The objective of this example is to demonstrate how to save the structures created within a script session to disk in order to be able to load them in a future session.
#
# There are several possible ways to achieve this:
#
# - with the standard pickle module
# - with openturns's Study

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import os
import pickle
ot.Log.Show(ot.Log.NONE)

# %%
# create objects to save
distribution = ot.Normal(4.0, 3.0)
function = ot.SymbolicFunction(['x1', 'x2'], ['x1 + x2'])

# %%
# **With the pickle module**
#
# The objects are retrieved in the same order they are stored.

# %%
# save objects
with open('study.pkl', 'wb') as f:
    pickle.dump(distribution, f)
    pickle.dump(function, f)

# %%
# load saved objects
with open('study.pkl', 'rb') as f:
    loaded_distribution = pickle.load(f)
    loaded_function = pickle.load(f)
str(loaded_distribution), str(loaded_function)

# %%
# **With OpenTURNS' Study**
#
# In order to be able to manipulate the objects contained in a Study, it is necessary to:
#
# - create the same empty structure in the new study,
# - fill this new empty structure with the content of the loaded structure, identified with its name or its id.
#
# Each object is identified whether with:
#
# - its name: it is useful to give names to the objects we want to save. If no name has been given by the user, we can use the name by default. The name of each object saved can be checked in the file.XML created or by printing the study in the python interface (with the command print).
# - its id number : this id number is unique for each object. It is useful to distinguish objects of same type which names are identical, equal to the default name given ('Unnamed'). This id number may be checked by printing the study loaded in the python interface (with the command print): be careful, this print operation must be performed after having loaded the study (the id number may be different from the one indicated in the file.XML associated to the study).

# %%
# Create a Study Object
study = ot.Study()

# %%
# Associate it to an XML file
fileName = 'study.xml'
study.setStorageManager(ot.XMLStorageManager(fileName))

# %%
# Add an object to the study; at this point it is not written to disk yet
study.add('distribution', distribution)
study.add('function', function)

# %%
# Save the study; this writes into the file
study.save()

# %%
# Create a new study associated to the same file
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(fileName))

# %%
# Load the file and all its objects
study.load()

# %%
# Check the content of the myStudy
print("Study = " , study)

# %%
# List names of stored objects
study.printLabels()

# %%
# Check our 'distribution' labelled object was loaded
study.hasObject('distribution')

# %%
# Load the objects; we must create a void object of the desired type (or parent type)
distributionLoaded = ot.Normal()
functionLoaded = ot.Function()
study.fillObject('distribution', loaded_distribution)
study.fillObject('function', loaded_function)
str(loaded_distribution), str(loaded_function)
