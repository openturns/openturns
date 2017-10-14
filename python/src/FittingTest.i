// SWIG file FittingTest.i


// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::TestResult & bestResultOut ($*ltype temp) %{ temp = OT::TestResult(); $1 = &temp; %}
%typemap(argout) OT::TestResult & bestResultOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::TestResult(*$1), SWIG_TypeQuery("OT::TestResult *"), SWIG_POINTER_OWN |  0 )); %}

%{
#include "openturns/FittingTest.hxx"
%}

%include FittingTest_doc.i

%include openturns/FittingTest.hxx
