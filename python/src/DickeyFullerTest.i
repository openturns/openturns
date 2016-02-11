// SWIG file DickeyFullerTest.i

%{
#include "openturns/DickeyFullerTest.hxx"
%}

%include DickeyFullerTest_doc.i

%include openturns/DickeyFullerTest.hxx
namespace OT { %extend DickeyFullerTest { DickeyFullerTest(const DickeyFullerTest & other) { return new OT::DickeyFullerTest(other); } } }
