// SWIG file VisualTest.i

%{
#include "openturns/VisualTest.hxx"
%}

%include VisualTest_doc.i

%nodefaultctor VisualTest;

%include openturns/VisualTest.hxx

%pythoncode %{
def Pairs(data, title=None, labels=None, color=None, pointStyle=None):
    openturns.common.Log.Warn('class Pairs is deprecated in favor of VisualTest.DrawPairs')
    return openturns.metamodel.VisualTest.DrawPairs(data)
%}
