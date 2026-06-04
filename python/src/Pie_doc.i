%feature("docstring") OT::Pie
"Pie.

Available constructors:
    Pie(*data*)

    Pie(*data, labels*)

    Pie(*data, labels, center, radius, palette*)

Parameters
----------
data : sequence of float
    Sequence giving the percentiles of the pie.
labels : sequence of str
    Names of each group. If not specified, by default equal to the description
    of the probabilistic input vector.
center : sequence of float
    Center of the pie inside the bounding box. If not specified, by default it
    is (0,0).
radius : positive float
    Radius of the pie. If not specified, by default it is 1.
palette : sequence of str
    Names of the colors. If not specified, colors are successively taken from
    the list given by :meth:`buildDefaultPalette`, in the same order.

Examples
--------
>>> import openturns as ot
>>> # Create the Pie
>>> myPie = ot.Pie([0.4, 0.3, 0.2, 0.1], ['a0', 'a1', 'a2', 'a3'])
>>> # Create an empty graph
>>> myGraph = ot.Graph('A Pie', ' ', ' ', True, '')
>>> myGraph.add(myPie)"

// ---------------------------------------------------------------------

%feature("docstring") OT::Pie::buildDefaultLabels
"Build default labels by taking the percentiles."

// ---------------------------------------------------------------------

%feature("docstring") OT::Pie::buildDefaultPalette
"Build default palette.

Call :meth:`BuildDefaultPalette` with :math:`n` the size of *data* as argument."

// ---------------------------------------------------------------------

%feature("docstring") OT::Pie::IsValidColorPalette
"Test the validity of a color palette.

Parameters
----------
palette : sequence of str
    Names of the colors.

Returns
-------
validity : bool
    *True* if *palette* is valid or *False* otherwise.

Examples
--------
>>> import openturns as ot
>>> pie = ot.Pie([0.5,0.2,0.3])
>>> print(pie.IsValidColorPalette(['#ff0000','#ccff00','#00ff66']))
True
>>> print(pie.IsValidColorPalette(['green','red','darkorange']))
True
>>> print(pie.IsValidColorPalette(['green','red','darkpink']))
False"
