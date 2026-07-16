// SWIG file MulticollinearityAnalysis.i

%{
#include "openturns/MulticollinearityAnalysis.hxx"
%}

%include MulticollinearityAnalysis_doc.i

%typemap(in, numinputs=0) OT::PointWithDescription & lmgOut ($*ltype temp) %{ $1 = &temp; %}
%typemap(argout) OT::PointWithDescription & lmgOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj($1->clone(), SWIG_TypeQuery("OT::PointWithDescription *"), SWIG_POINTER_OWN)); %};

%typemap(in, numinputs=0) OT::PointWithDescription & pmvdOut ($*ltype temp) %{ $1 = &temp; %}
%typemap(argout) OT::PointWithDescription & pmvdOut %{ $result = OT::AppendOutput($result, SWIG_NewPointerObj($1->clone(), SWIG_TypeQuery("OT::PointWithDescription *"), SWIG_POINTER_OWN)); %};

%include openturns/MulticollinearityAnalysis.hxx
