#                                               -*- cmake -*-
#
#  CMakeLists.txt
#
#  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
#
#  This library is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this library.  If not, see <http://www.gnu.org/licenses/>.
#
#

# Register current directory files
ot_add_current_dir_to_include_dirs ()

ot_add_source_file (WeightedExperimentImplementation.cxx)
ot_add_source_file (WeightedExperiment.cxx)
ot_add_source_file (FixedExperiment.cxx)
ot_add_source_file (GaussProductExperiment.cxx)
ot_add_source_file (BootstrapExperiment.cxx)
ot_add_source_file (MonteCarloExperiment.cxx)
ot_add_source_file (ImportanceSamplingExperiment.cxx)
ot_add_source_file (LHSResult.cxx)
ot_add_source_file (SimulatedAnnealingLHS.cxx)
ot_add_source_file (MonteCarloLHS.cxx)
ot_add_source_file (OptimalLHSExperiment.cxx)
ot_add_source_file (SpaceFillingPhiP.cxx)
ot_add_source_file (SpaceFillingMinDist.cxx)
ot_add_source_file (SpaceFillingC2.cxx)
ot_add_source_file (SpaceFillingImplementation.cxx)
ot_add_source_file (SpaceFilling.cxx)
ot_add_source_file (LinearProfile.cxx)
ot_add_source_file (GeometricProfile.cxx)
ot_add_source_file (TemperatureProfileImplementation.cxx)
ot_add_source_file (TemperatureProfile.cxx)
ot_add_source_file (LHSExperiment.cxx)
ot_add_source_file (LowDiscrepancyExperiment.cxx)
ot_add_source_file (SplitterImplementation.cxx)
ot_add_source_file (KFoldSplitter.cxx)
ot_add_source_file (LeaveOneOutSplitter.cxx)
ot_add_source_file (TensorProductExperiment.cxx)
ot_add_source_file (SmolyakExperiment.cxx)

ot_install_header_file (OTWeightedExperiments.hxx)
ot_install_header_file (WeightedExperimentImplementation.hxx)
ot_install_header_file (WeightedExperiment.hxx)
ot_install_header_file (FixedExperiment.hxx)
ot_install_header_file (GaussProductExperiment.hxx)
ot_install_header_file (BootstrapExperiment.hxx)
ot_install_header_file (MonteCarloExperiment.hxx)
ot_install_header_file (ImportanceSamplingExperiment.hxx)
ot_install_header_file (LHSResult.hxx)
ot_install_header_file (SimulatedAnnealingLHS.hxx)
ot_install_header_file (MonteCarloLHS.hxx)
ot_install_header_file (OptimalLHSExperiment.hxx)
ot_install_header_file (SpaceFillingPhiP.hxx)
ot_install_header_file (SpaceFillingMinDist.hxx)
ot_install_header_file (SpaceFillingC2.hxx)
ot_install_header_file (SpaceFillingImplementation.hxx)
ot_install_header_file (SpaceFilling.hxx)
ot_install_header_file (LinearProfile.hxx)
ot_install_header_file (GeometricProfile.hxx)
ot_install_header_file (TemperatureProfileImplementation.hxx)
ot_install_header_file (TemperatureProfile.hxx)
ot_install_header_file (LHSExperiment.hxx)
ot_install_header_file (LowDiscrepancyExperiment.hxx)
ot_install_header_file (SplitterImplementation.hxx)
ot_install_header_file (KFoldSplitter.hxx)
ot_install_header_file (LeaveOneOutSplitter.hxx)
ot_install_header_file (TensorProductExperiment.hxx)
ot_install_header_file (SmolyakExperiment.hxx)
