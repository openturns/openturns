// SWIG file BasisSequenceFactory.i

%{
#include "openturns/BasisSequenceFactory.hxx"
%}

%include BasisSequenceFactory_doc.i

OTTypedInterfaceObjectHelper(BasisSequenceFactory)

%copyctor OT::BasisSequenceFactory;

%include openturns/BasisSequenceFactory.hxx
