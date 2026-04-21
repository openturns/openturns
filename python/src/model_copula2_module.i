// SWIG file model_copula_module.i

%module(package="openturns", docstring="Copulas.") model_copula2
#pragma SWIG nowarn=302

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTBase.hxx"
#include "openturns/OTAlgorithm.hxx"
#include "openturns/OTModel.hxx"
#include "openturns/OTDistribution.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

%import base_module.i

%import model_copula_module.i

%include AliMikhailHaqCopula.i
%include AliMikhailHaqCopulaFactory.i
%include ClaytonCopula.i
%include ClaytonCopulaFactory.i
%include FarlieGumbelMorgensternCopula.i
%include FarlieGumbelMorgensternCopulaFactory.i
%include FrankCopula.i
%include FrankCopulaFactory.i
%include IndependentCopula.i
%include IndependentCopulaFactory.i
%include GumbelCopula.i
%include GumbelCopulaFactory.i
%include MarshallOlkinCopula.i
%include MinCopula.i
%include NormalCopula.i
%include NormalCopulaFactory.i
%include PlackettCopula.i
%include PlackettCopulaFactory.i

