// SWIG file MatrixImplementation.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::MatrixImplementation & R ($*ltype temp) %{ temp = OT::MatrixImplementation(); $1 = &temp; %}
%typemap(argout) OT::MatrixImplementation & R %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::MatrixImplementation(*$1), SWIG_TypeQuery("OT::MatrixImplementation *"), SWIG_POINTER_OWN |  0 )); %}
OT::MatrixImplementation OT::MatrixImplementation::computeQR(OT::MatrixImplementation & R, const OT::Bool fullQR = false, const OT::Bool keepIntact = true);
%apply OT::MatrixImplementation & R { OT::MatrixImplementation & u, OT::MatrixImplementation & vT };
OT::MatrixImplementation OT::MatrixImplementation::computeSVD(OT::MatrixImplementation & u, OT::MatrixImplementation & vT, const Bool fullSVD = false, const Bool keepIntact = true);

%typemap(in, numinputs=0) OT::Scalar & sign ($*ltype temp) %{ temp = 0.; $1 = &temp; %}
%typemap(argout) OT::Scalar & sign %{ $result = SWIG_Python_AppendOutput($result, OT::convert< OT::Scalar, OT::_PyFloat_ >(*$1) ); %}
OT::Scalar OT::MatrixImplementation::computeLogAbsoluteDeterminant(OT::Scalar & sign, const OT::Bool keepIntact = true);
OT::Scalar OT::MatrixImplementation::computeLogAbsoluteDeterminantSym(OT::Scalar & sign, const OT::Bool keepIntact = true);

%{
#include "openturns/MatrixImplementation.hxx"
%}

%rename(MatrixImplementation_operator___eq__) OT::operator ==(const MatrixImplementation & lhs, const MatrixImplementation & rhs);

%include openturns/MatrixImplementation.hxx
namespace OT{ %extend MatrixImplementation { MatrixImplementation(const MatrixImplementation & other) { return new OT::MatrixImplementation(other); } } }

%clear OT::MatrixImplementation & R;
%clear OT::Scalar & sign;
