// SWIG file GeneralizedParetoFactory.i

%{
#include "openturns/GeneralizedParetoFactory.hxx"
%}

%include GeneralizedParetoFactory_doc.i

%include openturns/GeneralizedParetoFactory.hxx
namespace OT { %extend GeneralizedParetoFactory { GeneralizedParetoFactory(const GeneralizedParetoFactory & other) { return new OT::GeneralizedParetoFactory(other); } } }
