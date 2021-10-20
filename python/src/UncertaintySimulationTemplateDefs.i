// SWIG file UncertaintySimulationTemplateDefs.i

%template(RootStrategyImplementationPointer) OT::Pointer<OT::RootStrategyImplementation>;
%template(SamplingStrategyImplementationPointer) OT::Pointer<OT::SamplingStrategyImplementation>;
%template(HSICStatImplementationPointer) OT::Pointer<OT::HSICStatImplementation>;
%template(HSICEstimatorImplementationPointer) OT::Pointer<OT::HSICEstimatorImplementation>;
OTDefaultCollectionConvertFunctions(CovarianceModel)
