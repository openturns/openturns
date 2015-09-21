// SWIG file graph_module.i

%module(package="openturns", docstring="Graphical output.") graph
//%feature("autodoc","1");

%{
#include "OTconfig.hxx"
#include "OTCommon.hxx"
#include "OTType.hxx"
#include "OTStat.hxx"
#include "OTGraph.hxx"
#include "OTGeom.hxx"
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
%include DrawableImplementation.i
%include Drawable.i
%include GraphImplementation.i
%include Graph.i
%include BarPlot.i
%include Cloud.i
%include Contour.i
%include Curve.i
%include Pairs.i
%include Pie.i
%include Polygon.i
%include PolygonArray.i
%include Staircase.i

/* At last we include template definitions */
%include BaseGraphTemplateDefs.i
