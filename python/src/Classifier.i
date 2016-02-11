// SWIG file Classifier.i

%{
#include "openturns/Classifier.hxx"
%}

%include Classifier_doc.i

OTTypedInterfaceObjectHelper(Classifier)

%include openturns/Classifier.hxx
namespace OT { %extend Classifier { Classifier(const Classifier & other) { return new OT::Classifier(other); } } }

