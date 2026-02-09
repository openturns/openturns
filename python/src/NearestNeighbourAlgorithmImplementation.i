// SWIG file NearestNeighbourAlgorithmImplementation.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & distanceOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & distanceOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"),  SWIG_POINTER_OWN)); %}

%{
#include "openturns/NearestNeighbourAlgorithmImplementation.hxx"
%}

%include NearestNeighbourAlgorithmImplementation_doc.i

%copyctor OT::NearestNeighbourAlgorithmImplementation;

%include openturns/NearestNeighbourAlgorithmImplementation.hxx
