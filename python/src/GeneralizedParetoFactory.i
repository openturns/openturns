// SWIG file GeneralizedParetoFactory.i

%{
#include "GeneralizedParetoFactory.hxx"
%}

%include GeneralizedParetoFactory_doc.i

%include GeneralizedParetoFactory.hxx
namespace OT { %extend GeneralizedParetoFactory { GeneralizedParetoFactory(const GeneralizedParetoFactory & other) { return new OT::GeneralizedParetoFactory(other); } } }
