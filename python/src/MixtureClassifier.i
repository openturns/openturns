// SWIG file MixtureClassifier.i

%{
#include "openturns/MixtureClassifier.hxx"
%}

%include MixtureClassifier_doc.i

%include openturns/MixtureClassifier.hxx
namespace OT { %extend MixtureClassifier { MixtureClassifier(const MixtureClassifier & other) { return new OT::MixtureClassifier(other); } } }
