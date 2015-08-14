// SWIG file MatrixImplementation.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::MatrixImplementation & R ($*ltype temp) %{ temp = OT::MatrixImplementation(); $1 = &temp; %}
%typemap(argout) OT::MatrixImplementation & R %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::MatrixImplementation(*$1), SWIG_TypeQuery("OT::MatrixImplementation *"), SWIG_POINTER_OWN |  0 )); %}
OT::MatrixImplementation OT::MatrixImplementation::computeQR(OT::MatrixImplementation & R, const OT::Bool fullQR = false, const OT::Bool keepIntact = true);
%apply OT::MatrixImplementation & R { OT::MatrixImplementation & u, OT::MatrixImplementation & vT };
OT::MatrixImplementation OT::MatrixImplementation::computeSVD(OT::MatrixImplementation & u, OT::MatrixImplementation & vT, const Bool fullSVD = false, const Bool keepIntact = true);

%typemap(in, numinputs=0) OT::NumericalScalar & sign ($*ltype temp) %{ temp = 0.; $1 = &temp; %}
%typemap(argout) OT::NumericalScalar & sign %{ $result = SWIG_Python_AppendOutput($result, OT::convert< OT::NumericalScalar, OT::_PyFloat_ >(*$1) ); %}
OT::NumericalScalar OT::MatrixImplementation::computeLogAbsoluteDeterminant(OT::NumericalScalar & sign, const OT::Bool keepIntact = true);
OT::NumericalScalar OT::MatrixImplementation::computeLogAbsoluteDeterminantSym(OT::NumericalScalar & sign, const OT::Bool keepIntact = true);

%{
#include "MatrixImplementation.hxx"
%}

%rename(MatrixImplementation_operator___eq__) OT::operator ==(const MatrixImplementation & lhs, const MatrixImplementation & rhs);

%include MatrixImplementation.hxx
namespace OT{ %extend MatrixImplementation { MatrixImplementation(const MatrixImplementation & other) { return new OT::MatrixImplementation(other); } } }
