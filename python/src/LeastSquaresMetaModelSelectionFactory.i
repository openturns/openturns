// SWIG file LeastSquaresMetaModelSelectionFactory.i

%{
#include "openturns/LeastSquaresMetaModelSelectionFactory.hxx"
%}

%include LeastSquaresMetaModelSelectionFactory_doc.i

%nodefaultctor LeastSquaresMetaModelSelectionFactory;

%include openturns/LeastSquaresMetaModelSelectionFactory.hxx
namespace OT { %extend LeastSquaresMetaModelSelectionFactory { LeastSquaresMetaModelSelectionFactory(const LeastSquaresMetaModelSelectionFactory & other) { return new OT::LeastSquaresMetaModelSelectionFactory(other); } } }
