%feature("docstring") OT::RegularGrid
R"RAW(Regular Grid.

Available constructors:
    RegularGrid(*start, step, n*)

    RegularGrid(*mesh*)

Parameters
----------
start : float
    The start time stamp of the grid.
step : float, positive
    The step between to consecutive time stamps.
n : int
    The number of time stamps in the grid, including the start and the end time stamps.
mesh : :class:`~openturns.Mesh`
    The mesh must be in :math:`\Rset`, regular and sorted in the increasing order.

Notes
-----
The time stamps of the regular grid are: :math:`(t_0, \dots, t_{n-1})` where :math:`t_{k} = t_0 + k \Delta` for :math:`0 \leq k \leq n-1` and :math:`\Delta >0` the step.

See also
--------
Mesh

Examples
--------
>>> import openturns as ot
>>> myRegularGrid = ot.RegularGrid(0.0, 0.1, 100)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::RegularGrid::getStart
"Accessor to the start time stamp.

Returns
-------
start : float
    The start point :math:`t_0` of the grid."

// ---------------------------------------------------------------------

%feature("docstring") OT::RegularGrid::getEnd
R"RAW(Accessor to the first time stamp after the last time stamp of the grid.

Returns
-------
endPoint : float
    The first point that follows the last point of the grid: :math:`t_{n-1} + \Delta`. The end point is not in the grid.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::RegularGrid::getStep
R"RAW(Accessor to the step.

Returns
-------
step : float
    The step :math:`\Delta` between two consecutive time stamps.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::RegularGrid::getN
"Accessor to the number of time stamps in the grid.

Returns
-------
n : int
    The number :math:`n` of time stamps in the grid."

// ---------------------------------------------------------------------

%feature("docstring") OT::RegularGrid::getValue
R"RAW(Accessor to the time stamps at a gien index.

Parameters
----------
k : int, :math:`0 \leq k \leq n-1`.
    Index of a time stamp. 

Returns
-------
value : float
    The time stamp :math:`t_{k}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::RegularGrid::getValues
"Accessor to all the time stamps.

Returns
-------
values : :class:`~openturns.Point`
    The collection of the time stamps."

// ---------------------------------------------------------------------

%feature("docstring") OT::RegularGrid::follows
"Check if the given grid follows the current one.

Parameters
----------
newGrid : :class:`~openturns.RegularGrid`
    A new regular grid.

Returns
-------
answer : bool
    The answer is *True* if the *newGrid* directly follows the current one."

