// SWIG file graph_module.i

%module(package="openturns", docstring="Graphical output.") graph
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTCommon.hxx"
#include "openturns/OTType.hxx"
#include "openturns/OTStat.hxx"
#include "openturns/OTGraph.hxx"
#include "openturns/OTGeom.hxx"
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
%include GridLayout.i
%include Pie.i
%include Polygon.i
%include Curve.i
%include PolygonArray.i
%include Staircase.i
%include Text.i

/* At last we include template definitions */
%include BaseGraphTemplateDefs.i
