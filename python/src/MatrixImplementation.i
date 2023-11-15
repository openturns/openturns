// SWIG file MatrixImplementation.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::MatrixImplementation & ROut ($*ltype temp) %{ temp = OT::MatrixImplementation(); $1 = &temp; %}
%typemap(argout) OT::MatrixImplementation & ROut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::MatrixImplementation(*$1), SWIG_TypeQuery("OT::MatrixImplementation *"), SWIG_POINTER_OWN |  0 )); %}

%apply OT::MatrixImplementation & ROut { OT::MatrixImplementation & uOut, OT::MatrixImplementation & vTOut, OT::MatrixImplementation & vOut };

%typemap(in, numinputs=0) OT::ComplexMatrixImplementation & vOut ($*ltype temp) %{ temp = OT::ComplexMatrixImplementation(); $1 = &temp; %}
%typemap(argout) OT::ComplexMatrixImplementation & vOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::ComplexMatrixImplementation(*$1), SWIG_TypeQuery("OT::ComplexMatrixImplementation *"), SWIG_POINTER_OWN |  0 )); %}

%typemap(in, numinputs=0) OT::SquareComplexMatrix & vOut ($*ltype temp) %{ temp = OT::SquareComplexMatrix(); $1 = &temp; %}
%typemap(argout) OT::SquareComplexMatrix & vOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::SquareComplexMatrix(*$1), SWIG_TypeQuery("OT::SquareComplexMatrix *"), SWIG_POINTER_OWN |  0 )); %}

%typemap(in, numinputs=0) OT::SquareMatrix & vOut ($*ltype temp) %{ temp = OT::SquareMatrix(); $1 = &temp; %}
%typemap(argout) OT::SquareMatrix & vOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::SquareMatrix(*$1), SWIG_TypeQuery("OT::SquareMatrix *"), SWIG_POINTER_OWN |  0 )); %}

%typemap(in, numinputs=0) OT::Scalar & signOut ($*ltype temp) %{ temp = 0.; $1 = &temp; %}
%typemap(argout) OT::Scalar & signOut %{ $result = SWIG_Python_AppendOutput($result, OT::convert< OT::Scalar, OT::_PyFloat_ >(*$1) ); %}

%{
#include "openturns/MatrixImplementation.hxx"
%}

%ignore OT::MatrixImplementation;

%rename(__stride__) OT::MatrixImplementation::stride;
%rename(MatrixImplementation_operator___eq__) OT::operator ==(const MatrixImplementation & lhs, const MatrixImplementation & rhs);

%copyctor OT::MatrixImplementation;

%include openturns/MatrixImplementation.hxx
