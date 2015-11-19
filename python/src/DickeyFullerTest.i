// SWIG file DickeyFullerTest.i

%{
#include "DickeyFullerTest.hxx"
%}

%include DickeyFullerTest_doc.i

%include DickeyFullerTest.hxx
namespace OT { %extend DickeyFullerTest { DickeyFullerTest(const DickeyFullerTest & other) { return new OT::DickeyFullerTest(other); } } }
