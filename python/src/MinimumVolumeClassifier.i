// SWIG file MinimumVolumeClassifier.i

%{
#include "openturns/MinimumVolumeClassifier.hxx"
%}

%include MinimumVolumeClassifier_doc.i

%include openturns/MinimumVolumeClassifier.hxx
namespace OT { %extend MinimumVolumeClassifier { MinimumVolumeClassifier(const MinimumVolumeClassifier & other) { return new OT::MinimumVolumeClassifier(other); } } }
