// SWIG file MixtureClassifier.i

%{
#include "MixtureClassifier.hxx"
%}

%include MixtureClassifier_doc.i

%include MixtureClassifier.hxx
namespace OT { %extend MixtureClassifier { MixtureClassifier(const MixtureClassifier & other) { return new OT::MixtureClassifier(other); } } }
