// SWIG file CorrelationAnalysis.i

%{
#include "openturns/CorrelationAnalysis.hxx"
%}

%include CorrelationAnalysis_doc.i

%copyctor OT::CorrelationAnalysis;

%typemap(in, numinputs=0) OT::PointWithDescription & lmgOut ($*ltype temp) %{ $1 = &temp; %}
%typemap(argout) OT::PointWithDescription & lmgOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj($1->clone(), SWIG_TypeQuery("OT::PointWithDescription *"), SWIG_POINTER_OWN)); %};

%typemap(in, numinputs=0) OT::PointWithDescription & pmvdOut ($*ltype temp) %{ $1 = &temp; %}
%typemap(argout) OT::PointWithDescription & pmvdOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj($1->clone(), SWIG_TypeQuery("OT::PointWithDescription *"), SWIG_POINTER_OWN)); %};

%include openturns/CorrelationAnalysis.hxx
