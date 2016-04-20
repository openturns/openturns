//                                               -*- C++ -*-
/**
 *  @file  HMatrixImplementation.cxx
 *  @brief This file supplies support for HMat
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <cstdlib>
#include <errno.h>

#include "openturns/HMatrixImplementation.hxx"
#include "openturns/Log.hxx"

#ifdef OPENTURNS_HAVE_HMAT
# include <hmat/hmat.h>
#endif


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HMatrixImplementation);

#ifdef OPENTURNS_HAVE_HMAT
static void trampoline_simple(void* user_context, int row, int col, void* result)
{
  HMatrixRealAssemblyFunction* assembly_function = static_cast<HMatrixRealAssemblyFunction*>(user_context);
  NumericalScalar *ptrValue = static_cast<NumericalScalar*>(result);
  *ptrValue = assembly_function->operator()(row, col);
}

/**
  Auxiliary data structure to represente a couple of degrees of freedom.
  This data structure will be sorted by compare_couple_indices so that
  all couples which have the same (point_1,point_2) are stored together.
*/
typedef struct
{
  int point_1;
  int point_2;
  UnsignedInteger dim_1;
  UnsignedInteger dim_2;
} couple_data_t;

static bool compare_couple_indices (const couple_data_t& couple1, const couple_data_t& couple2)
{
  if (couple1.point_1 == couple2.point_1) return couple1.point_2 < couple2.point_2;
  return couple1.point_1 < couple2.point_1;
}

class ParallelBlockData
{
public:
  std::vector<couple_data_t> list_couples_;
  const UnsignedInteger outputDimension_;
  const UnsignedInteger rowOffset_;
  const UnsignedInteger rowCount_;
  const UnsignedInteger colOffset_;
  const UnsignedInteger colCount_;
  int* row_hmat2client_;
  int* row_client2hmat_;
  int* col_hmat2client_;
  int* col_client2hmat_;
  HMatrixTensorRealAssemblyFunction* f_;
  ParallelBlockData(UnsignedInteger outputDimension,
                    UnsignedInteger rowOffset, UnsignedInteger rowCount,
                    UnsignedInteger colOffset, UnsignedInteger colCount)
    : list_couples_(rowCount*colCount)
    , outputDimension_(outputDimension)
    , rowOffset_(rowOffset)
    , rowCount_(rowCount)
    , colOffset_(colOffset)
    , colCount_(colCount)
  {}
};

static void
free_parallel_block_data(void* data)
{
  ParallelBlockData* blockData = static_cast<ParallelBlockData*>(data);
  delete blockData;
}

static void trampoline_hmat_prepare_block(int row_start, int row_count, int col_start, int col_count,
    int *row_hmat2client, int *row_client2hmat, int *col_hmat2client, int *col_client2hmat,
    void *context, hmat_block_info_t * block_info)
{
  HMatrixTensorRealAssemblyFunction* assembly_function = static_cast<HMatrixTensorRealAssemblyFunction*>(context);
  const UnsignedInteger outputDimension = assembly_function->getDimension();

  ParallelBlockData * blockData = new ParallelBlockData(outputDimension,
      row_start, row_count, col_start, col_count);
  blockData->f_ = assembly_function;
  blockData->row_hmat2client_ = row_hmat2client;
  blockData->row_client2hmat_ = row_client2hmat;
  blockData->col_hmat2client_ = col_hmat2client;
  blockData->col_client2hmat_ = col_client2hmat;

  std::vector<couple_data_t> & list_couples = blockData->list_couples_;
  block_info->user_data = blockData;
  block_info->release_user_data = free_parallel_block_data;

  std::vector<couple_data_t>::iterator couple_it = list_couples.begin();
  for (int j = 0; j < col_count; ++j)
  {
    const int c_dof_e = col_hmat2client[j + col_start];
    const int c_point_e = c_dof_e / outputDimension;
    const UnsignedInteger c_dim_e   = c_dof_e % outputDimension;
    for (int i = 0; i < row_count; ++i, ++couple_it)
    {
      const int r_dof_e = row_hmat2client[i + row_start];
      couple_it->point_1 = r_dof_e / outputDimension;
      couple_it->point_2 = c_point_e;
      couple_it->dim_1 = r_dof_e % outputDimension;
      couple_it->dim_2 = c_dim_e;
    }
  }
  // Sort couples
  std::sort(list_couples.begin(), list_couples.end(), compare_couple_indices);
}

static void trampoline_compute(void* v_data,
                               int row_start, int row_count, int col_start, int col_count,
                               void* block)
{
  ParallelBlockData * blockData = static_cast<ParallelBlockData*>(v_data);
  UnsignedInteger rowBlockBegin = blockData->rowOffset_;
  UnsignedInteger colBlockBegin = blockData->colOffset_;
  std::vector<couple_data_t> & list_couples = blockData->list_couples_;

  const UnsignedInteger outputDimension = blockData->outputDimension_;
  int lastPoint1 = -1, lastPoint2 = -1;
  const int firstRowIndex(rowBlockBegin + row_start);
  const int firstColumnIndex(colBlockBegin + col_start);
  CovarianceMatrix localMat(outputDimension);
  NumericalScalar * result = static_cast<NumericalScalar*>(block);
  for (std::vector<couple_data_t>::const_iterator cit = list_couples.begin(); cit != list_couples.end(); ++cit)
  {
    const int r_point_e = cit->point_1;
    const int c_point_e = cit->point_2;
    const UnsignedInteger r_dim_e = cit->dim_1;
    const UnsignedInteger c_dim_e = cit->dim_2;
    const int r_dof_i = blockData->row_client2hmat_[outputDimension * r_point_e + r_dim_e];
    if (r_dof_i < firstRowIndex || r_dof_i >= firstRowIndex + (int) row_count) continue;
    const int c_dof_i = blockData->col_client2hmat_[outputDimension * c_point_e + c_dim_e];
    if (c_dof_i < firstColumnIndex || c_dof_i >= firstColumnIndex + (int) col_count) continue;

    if (lastPoint1 != r_point_e || lastPoint2 != c_point_e)
    {
      memset( &localMat.getImplementation()->operator[](0), 0, outputDimension * outputDimension * sizeof(NumericalScalar) );
      blockData->f_->compute( r_point_e,  c_point_e, &localMat );
      lastPoint1 = r_point_e;
      lastPoint2 = c_point_e;
    }
    const int pos = (c_dof_i - firstColumnIndex) * row_count + (r_dof_i - firstRowIndex);
    result[pos] = localMat(r_dim_e, c_dim_e);
  }
}
#endif   /* OPENTURNS_HAVE_HMAT */

HMatrixClusterTree::~HMatrixClusterTree()
{
#ifdef OPENTURNS_HAVE_HMAT
  hmat_delete_cluster_tree(static_cast<hmat_cluster_tree_t*>(hmatClusterTree_));
#endif
}

HMatrixImplementation::HMatrixImplementation()
  : hmatInterface_(NULL)
  , hmatClusterTree_(NULL)
  , hmat_(NULL)
{
  // Nothing to do
}

HMatrixImplementation::HMatrixImplementation(void* ptr_hmat_interface, void* ptr_hmat_cluster_tree, int cluster_size, void* ptr_hmatrix)
  : hmatInterface_(ptr_hmat_interface)
  , hmatClusterTree_(new HMatrixClusterTree(ptr_hmat_cluster_tree, cluster_size))
  , hmat_(ptr_hmatrix)
{
  // Nothing to do
}

HMatrixImplementation::HMatrixImplementation(const HMatrixImplementation& other)
  : PersistentObject(other)
  , hmatInterface_(other.hmatInterface_)
  , hmatClusterTree_(NULL)
  , hmat_(NULL)
{
#ifdef OPENTURNS_HAVE_HMAT
  hmat_cluster_tree_t* ptr_other_ct = static_cast<hmat_cluster_tree_t*>(other.hmatClusterTree_.get()->get());
  hmatClusterTree_ = new HMatrixClusterTree(hmat_copy_cluster_tree(ptr_other_ct), other.hmatClusterTree_.get()->getSize());
  hmat_cluster_tree_t* ptr_ct = static_cast<hmat_cluster_tree_t*>(hmatClusterTree_.get()->get());

  hmat_interface_t* ptr_interface = static_cast<hmat_interface_t*>(hmatInterface_);
  hmat_matrix_t* hmat_copy = ptr_interface->copy(static_cast<hmat_matrix_t*>(other.hmat_));
  ptr_interface->set_cluster_trees(hmat_copy, ptr_ct, ptr_ct);
  hmat_ = hmat_copy;
#endif
}

/* Virtual constructor */
HMatrixImplementation * HMatrixImplementation::clone() const
{
  return new HMatrixImplementation(*this);
}

HMatrixImplementation::~HMatrixImplementation()
{
#ifdef OPENTURNS_HAVE_HMAT
  if (hmatInterface_ != NULL && hmat_ != NULL)
    static_cast<hmat_interface_t*>(hmatInterface_)->destroy(static_cast<hmat_matrix_t*>(hmat_));
#endif
}

void
HMatrixImplementation::assemble(const HMatrixRealAssemblyFunction& f, char symmetry)
{
#ifdef OPENTURNS_HAVE_HMAT
  int sym = 0;
  switch(symmetry)
  {
    case 'N':
    case 'n':
      break;
    case 'L':
    case 'l':
      sym = 1;
      break;
    default:
      throw InvalidArgumentException(HERE) << "Error: invalid symmetry flag '" << symmetry << "', must be either 'N' or 'L'";
  }

  static_cast<hmat_interface_t*>(hmatInterface_)->assemble_simple_interaction(static_cast<hmat_matrix_t*>(hmat_), const_cast<HMatrixRealAssemblyFunction*>(&f), &trampoline_simple, sym);
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

UnsignedInteger HMatrixImplementation::getNbRows() const
{
#ifdef OPENTURNS_HAVE_HMAT
  if (hmatClusterTree_.isNull()) return 0;
  return hmatClusterTree_.get()->getSize();
#else
  return 0;
#endif
}

UnsignedInteger HMatrixImplementation::getNbColumns() const
{
#ifdef OPENTURNS_HAVE_HMAT
  if (hmatClusterTree_.isNull()) return 0;
  return hmatClusterTree_.get()->getSize();
#else
  return 0;
#endif
}

void HMatrixImplementation::assemble(const HMatrixTensorRealAssemblyFunction& f, char symmetry)
{
#ifdef OPENTURNS_HAVE_HMAT
  int sym = 0;
  switch(symmetry)
  {
    case 'N':
    case 'n':
      break;
    case 'L':
    case 'l':
      sym = 1;
      break;
    default:
      throw InvalidArgumentException(HERE) << "Error: invalid symmetry flag '" << symmetry << "', must be either 'N' or 'L'";
  }
  static_cast<hmat_interface_t*>(hmatInterface_)->assemble(
    static_cast<hmat_matrix_t*>(hmat_),
    const_cast<HMatrixTensorRealAssemblyFunction*>(&f),
    &trampoline_hmat_prepare_block, &trampoline_compute, sym);
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

void HMatrixImplementation::factorize(const String& method)
{
#ifdef OPENTURNS_HAVE_HMAT
  hmat_settings_t settings;
  hmat_get_parameters(&settings);
  hmat_factorization_t fact_method = hmat_factorization_lu;
  if (method == "LDLt")
    fact_method = hmat_factorization_ldlt;
  else if (method == "LLt")
    fact_method = hmat_factorization_llt;
  else if (method != "LU")
    LOGWARN( OSS() << "Unknown factorization method: " << method << ". Valid values are: LU, LDLt, or LLt.");

  try
  {
    static_cast<hmat_interface_t*>(hmatInterface_)->factorize(static_cast<hmat_matrix_t*>(hmat_), fact_method);
  }
  catch (std::exception& ex)
  {
    // hmat::LapackException is not yet exported
    throw InternalException(HERE) << ex.what();
  }
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

void HMatrixImplementation::gemv(char trans, NumericalScalar alpha, const NumericalPoint& x, NumericalScalar beta, NumericalPoint& y) const
{
#ifdef OPENTURNS_HAVE_HMAT
  // gemv() below reorders x indices, thus x is not constant.
  NumericalPoint xcopy(x);
  static_cast<hmat_interface_t*>(hmatInterface_)->gemv(trans, &alpha, static_cast<hmat_matrix_t*>(hmat_), &xcopy[0], &beta, &y[0], 1);
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

void HMatrixImplementation::gemm(char transA, char transB, NumericalScalar alpha, const HMatrixImplementation& a, const HMatrixImplementation& b, NumericalScalar beta)
{
#ifdef OPENTURNS_HAVE_HMAT
  static_cast<hmat_interface_t*>(hmatInterface_)->gemm(transA, transB, &alpha, static_cast<hmat_matrix_t*>(a.hmat_), static_cast<hmat_matrix_t*>(b.hmat_), &beta, static_cast<hmat_matrix_t*>(hmat_));
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

void HMatrixImplementation::transpose()
{
#ifdef OPENTURNS_HAVE_HMAT
  static_cast<hmat_interface_t*>(hmatInterface_)->transpose(static_cast<hmat_matrix_t*>(hmat_));
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

NumericalScalar HMatrixImplementation::norm() const
{
#ifdef OPENTURNS_HAVE_HMAT
  return static_cast<hmat_interface_t*>(hmatInterface_)->norm(static_cast<hmat_matrix_t*>(hmat_));
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

NumericalPoint HMatrixImplementation::getDiagonal() const
{
#ifdef OPENTURNS_HAVE_HMAT
  NumericalPoint diag(hmatClusterTree_.get()->getSize());
  static_cast<hmat_interface_t*>(hmatInterface_)->extract_diagonal(static_cast<hmat_matrix_t*>(hmat_), &diag[0], diag.getDimension());
  return diag;
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

NumericalPoint HMatrixImplementation::solve(const NumericalPoint& b, Bool trans) const
{
  if (trans) throw NotYetImplementedException(HERE) << "transposed not yet supported in HMatrixImplementation::solve";
#ifdef OPENTURNS_HAVE_HMAT
  NumericalPoint result(b);
  static_cast<hmat_interface_t*>(hmatInterface_)->solve_systems(static_cast<hmat_matrix_t*>(hmat_), &result[0], 1);
  return result;
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

Matrix HMatrixImplementation::solve(const Matrix& m, Bool trans) const
{
  if (trans) throw NotYetImplementedException(HERE) << "transposed not yet supported in HMatrixImplementation::solve";
#ifdef OPENTURNS_HAVE_HMAT
  Matrix result(m);
  static_cast<hmat_interface_t*>(hmatInterface_)->solve_systems(static_cast<hmat_matrix_t*>(hmat_), &result(0, 0), result.getNbColumns());
  return result;
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

NumericalPoint HMatrixImplementation::solveLower(const NumericalPoint& b, Bool trans) const
{
#ifdef OPENTURNS_HAVE_HMAT
  int t = trans;
  NumericalPoint result(b);
  static_cast<hmat_interface_t*>(hmatInterface_)->solve_lower_triangular(static_cast<hmat_matrix_t*>(hmat_), t, &result[0], 1);
  return result;
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

Matrix HMatrixImplementation::solveLower(const Matrix& m, Bool trans) const
{
#ifdef OPENTURNS_HAVE_HMAT
  int t = trans;
  Matrix result(m);
  static_cast<hmat_interface_t*>(hmatInterface_)->solve_lower_triangular(static_cast<hmat_matrix_t*>(hmat_), t, &result(0, 0), result.getNbColumns());
  return result;
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

std::pair<size_t, size_t> HMatrixImplementation::compressionRatio() const
{
#ifdef OPENTURNS_HAVE_HMAT
  hmat_info_t mat_info;
  static_cast<hmat_interface_t*>(hmatInterface_)->get_info(static_cast<hmat_matrix_t*>(hmat_), &mat_info);
  return std::pair<size_t, size_t>(mat_info.compressed_size, mat_info.uncompressed_size);
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

std::pair<size_t, size_t> HMatrixImplementation::fullrkRatio() const
{
#ifdef OPENTURNS_HAVE_HMAT
  hmat_info_t mat_info;
  static_cast<hmat_interface_t*>(hmatInterface_)->get_info(static_cast<hmat_matrix_t*>(hmat_), &mat_info);
  return std::pair<size_t, size_t>(mat_info.full_size, mat_info.rk_size);
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

void HMatrixImplementation::dump(const String & name) const
{
#ifdef OPENTURNS_HAVE_HMAT
  static_cast<hmat_interface_t*>(hmatInterface_)->dump_info(static_cast<hmat_matrix_t*>(hmat_), const_cast<char*>(name.c_str()));
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

Bool HMatrixImplementation::setKey(const String & name, const String & value)
{
#ifdef OPENTURNS_HAVE_HMAT
  hmat_settings_t settings;
  hmat_get_parameters(&settings);
  if (name == "compression")
  {
    if (value == "SVD")
      settings.compressionMethod = hmat_compress_svd;
    else if (value == "ACAfull")
      settings.compressionMethod = hmat_compress_aca_full;
    else if (value == "ACApartial")
      settings.compressionMethod = hmat_compress_aca_partial;
    else if (value == "ACA+")
      settings.compressionMethod = hmat_compress_aca_plus;
    else
      LOGWARN( OSS() << "Unknown compression method: " << value << ". Valid values are: SVD, ACAfull, ACApartial or ACA+");
  }
  else if (name == "assembly-epsilon")
  {
    std::istringstream iss( value );
    iss >> settings.assemblyEpsilon;
  }
  else if (name == "recompression-epsilon")
  {
    std::istringstream iss( value );
    iss >> settings.recompressionEpsilon;
  }
  else if (name == "max-parallel-leaves")
  {
    std::istringstream iss( value );
    iss >> settings.maxParallelLeaves;
  }
  else
    LOGWARN( OSS() << "Unknown parameter: " << name);

  hmat_set_parameters(&settings);
  return true;
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

/* String converter */
String HMatrixImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class= " << HMatrixImplementation::GetClassName();
  return oss;
}

/* String converter */
String HMatrixImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "class= " << HMatrixImplementation::GetClassName();
  return oss;
}

END_NAMESPACE_OPENTURNS
