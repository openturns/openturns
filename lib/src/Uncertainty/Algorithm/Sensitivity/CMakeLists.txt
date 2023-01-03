#                                               -*- cmake -*-
#
#  CMakeLists.txt
#
#  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

ot_add_source_file (ANCOVA.cxx)
ot_add_source_file (FAST.cxx)
ot_add_source_file (JansenSensitivityAlgorithm.cxx)
ot_add_source_file (MartinezSensitivityAlgorithm.cxx)
ot_add_source_file (MauntzKucherenkoSensitivityAlgorithm.cxx)
ot_add_source_file (SaltelliSensitivityAlgorithm.cxx)
ot_add_source_file (SobolIndicesAlgorithm.cxx)
ot_add_source_file (SobolIndicesAlgorithmImplementation.cxx)
ot_add_source_file (SobolIndicesExperiment.cxx)
ot_add_source_file (HSICStat.cxx)
ot_add_source_file (HSICStatImplementation.cxx)
ot_add_source_file (HSICUStat.cxx)
ot_add_source_file (HSICVStat.cxx)
ot_add_source_file (HSICEstimator.cxx)
ot_add_source_file (HSICEstimatorImplementation.cxx)
ot_add_source_file (HSICEstimatorConditionalSensitivity.cxx)
ot_add_source_file (HSICEstimatorGlobalSensitivity.cxx)
ot_add_source_file (HSICEstimatorTargetSensitivity.cxx)


ot_install_header_file (OTSensitivity.hxx)
ot_install_header_file (ANCOVA.hxx)
ot_install_header_file (FAST.hxx)
ot_install_header_file (JansenSensitivityAlgorithm.hxx)
ot_install_header_file (MartinezSensitivityAlgorithm.hxx)
ot_install_header_file (MauntzKucherenkoSensitivityAlgorithm.hxx)
ot_install_header_file (SaltelliSensitivityAlgorithm.hxx)
ot_install_header_file (SobolIndicesAlgorithm.hxx)
ot_install_header_file (SobolIndicesAlgorithmImplementation.hxx)
ot_install_header_file (SobolIndicesExperiment.hxx)
ot_install_header_file (HSICStat.hxx)
ot_install_header_file (HSICStatImplementation.hxx)
ot_install_header_file (HSICUStat.hxx)
ot_install_header_file (HSICVStat.hxx)
ot_install_header_file (HSICEstimator.hxx)
ot_install_header_file (HSICEstimatorImplementation.hxx)
ot_install_header_file (HSICEstimatorConditionalSensitivity.hxx)
ot_install_header_file (HSICEstimatorGlobalSensitivity.hxx)
ot_install_header_file (HSICEstimatorTargetSensitivity.hxx)

