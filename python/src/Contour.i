// SWIG file Contour.i

%typemap(out) OT::Scalar getVmin{
  $result = $1 != $1 ? Py_None:PyFloat_FromDouble($1);
}
%typemap(out) OT::Scalar getVmax{
  $result = $1 != $1 ? Py_None:PyFloat_FromDouble($1);
}

%{
#include "openturns/Contour.hxx"
%}

%include Contour_doc.i

%copyctor OT::Contour;

%include openturns/Contour.hxx
