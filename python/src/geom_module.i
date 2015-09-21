// SWIG file geom_module.i

%module(package="openturns", docstring="Geometrical classes.") geom
//%feature("autodoc","1");

%{
#include "OTconfig.hxx"
#include "OTCommon.hxx"
#include "OTGeom.hxx"
#include "OTStat.hxx"
#include "OTType.hxx"
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
%include Mesh.i
%include RegularGrid.i
%include MeshFactoryImplementation.i
%include MeshFactory.i
%include IntervalMesher.i
