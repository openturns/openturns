// SWIG file CovarianceModelImplementation.i

%{
#include "openturns/CovarianceModelImplementation.hxx"
%}

%include CovarianceModelImplementation_doc.i

%include openturns/CovarianceModelImplementation.hxx
namespace OT{ %extend CovarianceModelImplementation { CovarianceModelImplementation(const CovarianceModelImplementation & other) { return new OT::CovarianceModelImplementation(other); } } }
