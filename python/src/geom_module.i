// SWIG file geom_module.i

%module(package="openturns", docstring="Geometrical classes.") geom
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTCommon.hxx"
#include "openturns/OTGeom.hxx"
#include "openturns/OTStat.hxx"
#include "openturns/OTType.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

/* Base/Common */
%import common_module.i

/* Base/Type */
%import typ_module.i
%import BaseTypTemplateDefs.i

/* Base/Graph */
%import graph_module.i
%import BaseGraphTemplateDefs.i

/* Base/Geom */
%include Domain.i
%include BipartiteGraph.i
%include MeshImplementation.i
%include Mesh.i
%include RegularGrid.i
%include IntervalMesher.i
%include MeshDomain.i
