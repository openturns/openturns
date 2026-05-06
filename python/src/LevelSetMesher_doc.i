%feature("docstring") OT::LevelSetMesher
"Creation of mesh from a level set.

Parameters
----------
discretization : sequence of int
    Discretization of the level set bounding box.
solver : :class:`~openturns.OptimizationAlgorithm`, optional
    Optimization solver used to project the vertices onto the level set.
    It must be able to solve nearest point problems.
    Default is :class:`~openturns.AbdoRackwitz`.

Notes
-----
The meshing algorithm is based either on a :class:`~openturns.Field` or on the
:class:`~openturns.Interval` class.

  * Using a field allows one to mesh a level set reusing previously computed
    values and starting from an arbitrary mesh.
    If the values of the field are of dimension 1 they are supposed to
    be the evaluation of the function defining the level set at the vertices of
    the mesh, otherwise they are recomputed.

  * Using a bounding box (provided by the user or automatically computed) is a
    shortcut. The interval is meshed using the :class:`~openturns.Interval` class
    and the function defining the level set is evaluated at the vertices of the
    resulting mesh to create a field.

  * Then, all the simplices with all vertices outside of the level set are
    rejected, while the simplices with all vertices inside of the level
    set are kept. The remaining simplices are adapted the following way:

      * The mean point of the vertices inside of the level set is computed

      * Each vertex outside of the level set is projected onto the level set using
        a linear interpolation

      * If the *project* flag is *True*, then the projection is refined using first
        a nonlinear equation solver :class:`~openturns.Brent` depending on the
        `LevelSetMesher-SolveEquation` key in :class:`~openturns.ResourceMap` class.
        If this key is set to *False* or if the solver fails, then the projection is
        done using the provided optimization algorithm.
        If it fails then the :class:`~openturns.Cobyla` optimization algorithm is used.



Examples
--------
Create a mesh:

>>> import openturns as ot
>>> mesher = ot.LevelSetMesher([5, 10])
>>> level = 1.0
>>> function = ot.SymbolicFunction(['x0', 'x1'], ['x0^2+x1^2'])
>>> levelSet = ot.LevelSet(function, ot.LessOrEqual(), level)
>>> mesh = mesher.build(levelSet, ot.Interval([-2.0]*2, [2.0]*2))
"

/// ---------------------------------------------------------------------

%feature("docstring") OT::LevelSetMesher::build
"Build the mesh of level set type.

**Available usages**:

    build(*levelSet, project*)

    build(*levelSet, boundingBox, project*)

    build(*levelSet, field, project*)

Parameters
----------
levelSet : :class:`~openturns.LevelSet`
    The level set to be meshed, of dimension equal to the dimension
    of `discretization`.
boundingBox : :class:`~openturns.Interval`
    The bounding box used to mesh the level set. It is automatically computed
    if not provided.
field : :class:`~openturns.Field`
    The field used to mesh the level set.
project : bool
    Flag to tell if the vertices outside of the level set of a simplex partially included into the level set have to be projected onto the level set. Default is *True*.

Returns
-------
mesh : :class:`~openturns.Mesh`
    The mesh built."
   
// ---------------------------------------------------------------------

%feature("docstring") OT::LevelSetMesher::getDiscretization
"Accessor to the discretization.

Returns
-------
discretization : :class:`~openturns.Indices`
    Discretization of the bounding box of the level sets."

// ---------------------------------------------------------------------

%feature("docstring") OT::LevelSetMesher::setDiscretization
"Accessor to the discretization.

Parameters
----------
discretization : sequence of int
    Discretization of the bounding box of the level sets."

// ---------------------------------------------------------------------

%feature("docstring") OT::LevelSetMesher::getOptimizationAlgorithm
"Accessor to the optimization solver.

Returns
-------
solver : :class:`~openturns.OptimizationAlgorithm`
    The optimization solver used to project vertices onto the level set."

// ---------------------------------------------------------------------

%feature("docstring") OT::LevelSetMesher::setOptimizationAlgorithm
"Accessor to the optimization solver.

Parameters
----------
solver : :class:`~openturns.OptimizationAlgorithm`
    The optimization solver used to project vertices onto the level set."
