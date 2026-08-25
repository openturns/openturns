%feature("docstring") OT::DataContainer
"Unified data storage with pluggable backends.

This class is experimental.

DataContainer separates data storage from algebra and statistical services,
enabling zero-copy interoperability between different container types.

Parameters
----------
size : int, optional
    Number of elements. Default is 0.
value : float, optional
    Default value for all elements. Default is 0.0.
nbRows : int, optional
    Number of rows (for 2D containers).
nbColumns : int, optional
    Number of columns (for 2D containers).
elementsValues : sequence of float, optional
    Initial values.
layout : int, optional
    Storage layout: ``DataContainer.COLUMN_MAJOR`` (default) or ``DataContainer.ROW_MAJOR``."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::getSize
"Get the number of rows.

Returns
-------
size : int
    Number of rows."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::getDimension
"Get the number of columns.

Returns
-------
dimension : int
    Number of columns."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::getNbRows
"Get the number of rows (alias for getSize).

Returns
-------
nbRows : int
    Number of rows."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::getNbColumns
"Get the number of columns (alias for getDimension).

Returns
-------
nbColumns : int
    Number of columns."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::getLayout
"Get the storage layout.

Returns
-------
layout : int
    ``DataContainer.ROW_MAJOR`` or ``DataContainer.COLUMN_MAJOR``."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::setDimension
"Set the number of columns.

Parameters
----------
dimension : int
    New number of columns. Must satisfy size = nbRows * dimension."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::isEmpty
"Check if the container is empty.

Returns
-------
isEmpty : bool
    True if size is 0."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::resize
"Resize the container.

Parameters
----------
newSize : int
    New number of elements."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::makeUnique
"Ensure exclusive ownership of the storage (copy-on-write)."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::erase
"Erase a range of elements from the flat buffer.

Shifts the elements after the range to the left and shrinks the container.
Not allowed on non-owning views created by subView() or the external pointer constructor.

Parameters
----------
firstFlat : int
    Index of the first erased element.
lastFlat : int
    Index after the last erased element."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::clear
"Clear all data, resetting the container to empty.

The storage is released and replaced by a fresh empty backend,
detaching from any shared or viewed memory."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::isView
"Check if the container is a non-owning view over external memory.

Returns
-------
isView : bool
    True if the container does not own its data."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::subView
"Return a non-owning view over a contiguous row range.

The view shares the underlying memory: mutations through either
container are visible to the other. The view is only valid while
the parent container keeps exclusive ownership of a compatible buffer.

Parameters
----------
startRow : int
    First row of the view.
nrows : int
    Number of rows of the view.

Returns
-------
view : DataContainer
    Non-owning view of shape (nrows, nbColumns)."

// ---------------------------------------------------------------------

%feature("docstring") OT::DataContainer::stride
"Get the stride for a given dimension.

Parameters
----------
dim : int
    Dimension index (0 or 1).

Returns
-------
stride : int
    Stride in number of elements."
