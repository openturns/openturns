// SWIG file AggregatedProcess.i

%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/AggregatedProcess.hxx"
%}

%include AggregatedProcess_doc.i

%template(ProcessCollection) OT::Collection<OT::Process>;

%include openturns/AggregatedProcess.hxx

%extend OT::Collection<OT::Process>
{

OT::Collection<OT::Process> (PyObject * pyObj)
{
  return OT::buildCollectionFromPySequence<OT::Process>( pyObj );
}

}

namespace OT {

%extend AggregatedProcess {

AggregatedProcess(PyObject * pyObj)
{
  OT::Pointer<OT::Collection<OT::Process> > p_coll =  OT::buildCollectionFromPySequence<OT::Process>( pyObj );
  return new OT::AggregatedProcess( *p_coll );
}

AggregatedProcess(const AggregatedProcess & other)
{
  return new OT::AggregatedProcess(other);
}

} // AggregatedProcess
 
} // OT
