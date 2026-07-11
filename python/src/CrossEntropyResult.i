// SWIG file CrossEntropyResult.i

%{
#include "openturns/CrossEntropyResult.hxx"
%}

%include CrossEntropyResult_doc.i

%copyctor OT::CrossEntropyResult;

%include openturns/CrossEntropyResult.hxx

namespace OT {

%extend CrossEntropyResult {
  CrossEntropyResult(const SimulationResult & result) {
    return new OT::CrossEntropyResult(dynamic_cast<const OT::CrossEntropyResult &>(*result.getImplementation()));
  }
}

} // namespace OT
