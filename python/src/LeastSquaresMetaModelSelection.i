// SWIG file LeastSquaresMetaModelSelection.i

%{
#include "LeastSquaresMetaModelSelection.hxx"
%}

%include LeastSquaresMetaModelSelection_doc.i

%include LeastSquaresMetaModelSelection.hxx
namespace OT { %extend LeastSquaresMetaModelSelection { LeastSquaresMetaModelSelection(const LeastSquaresMetaModelSelection & other) { return new OT::LeastSquaresMetaModelSelection(other); } } }
