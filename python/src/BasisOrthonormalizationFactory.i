// SWIG file BasisOrthonormalizationFactory.i

%{
#include "openturns/BasisOrthonormalizationFactory.hxx"
%}

%include BasisOrthonormalizationFactory_doc.i

%copyctor OT::BasisOrthonormalizationFactory;

%typemap("out") const SquareMatrix & { $result = SWIG_NewPointerObj(new SquareMatrix(*$1), SWIGTYPE_p_OT__SquareMatrix, SWIG_POINTER_OWN); }

%include openturns/BasisOrthonormalizationFactory.hxx
