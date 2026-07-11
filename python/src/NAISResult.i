// SWIG file NAISResult.i

%{
#include "openturns/NAISResult.hxx"
%}

%include NAISResult_doc.i

%copyctor OT::NAISResult;

%include openturns/NAISResult.hxx

namespace OT {

%extend NAISResult {
  NAISResult(const SimulationResult & result) {
    return new OT::NAISResult(dynamic_cast<const OT::NAISResult &>(*result.getImplementation()));
  }
}

} // namespace OT
