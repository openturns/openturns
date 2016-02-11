// SWIG file LeastSquaresMetaModelSelection.i

%{
#include "openturns/LeastSquaresMetaModelSelection.hxx"
%}

%include LeastSquaresMetaModelSelection_doc.i

%include openturns/LeastSquaresMetaModelSelection.hxx
namespace OT { %extend LeastSquaresMetaModelSelection { LeastSquaresMetaModelSelection(const LeastSquaresMetaModelSelection & other) { return new OT::LeastSquaresMetaModelSelection(other); } } }
