// SWIG file CovarianceModelImplementation.i

%{
#include "CovarianceModelImplementation.hxx"
%}

%include CovarianceModelImplementation_doc.i

%include CovarianceModelImplementation.hxx
namespace OT{ %extend CovarianceModelImplementation { CovarianceModelImplementation(const CovarianceModelImplementation & other) { return new OT::CovarianceModelImplementation(other); } } }
