// SWIG file LeastSquaresMetaModelSelectionFactory.i

%{
#include "LeastSquaresMetaModelSelectionFactory.hxx"
%}

%include LeastSquaresMetaModelSelectionFactory_doc.i

%nodefaultctor LeastSquaresMetaModelSelectionFactory;

%include LeastSquaresMetaModelSelectionFactory.hxx
namespace OT { %extend LeastSquaresMetaModelSelectionFactory { LeastSquaresMetaModelSelectionFactory(const LeastSquaresMetaModelSelectionFactory & other) { return new OT::LeastSquaresMetaModelSelectionFactory(other); } } }
