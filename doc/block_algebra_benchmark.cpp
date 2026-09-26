#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>
#include "openturns/OTprivate.hxx"
#include "openturns/DataContainer.hxx"
#include "openturns/AlgebraEngine.hxx"

using namespace OT;

static double elapsed_ms(std::chrono::high_resolution_clock::time_point t0,
                         std::chrono::high_resolution_clock::time_point t1)
{
  return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

static DataContainer makeRandomMatrix(UnsignedInteger rows, UnsignedInteger cols, UnsignedInteger seed = 42)
{
  DataContainer A(rows, cols, 0.0);
  // Simple LCG for reproducibility
  unsigned long long state = seed;
  for (UnsignedInteger j = 0; j < cols; ++j)
    for (UnsignedInteger i = 0; i < rows; ++i)
    {
      state = state * 6364136223846793005ULL + 1442695040888963407ULL;
      A(i, j) = (static_cast<double>((state >> 33) & 0x7FFFFFFF) / 0x7FFFFFFF) * 2.0 - 1.0;
    }
  return A;
}

static DataContainer makeSPDMatrix(UnsignedInteger n, UnsignedInteger seed = 42)
{
  DataContainer A = makeRandomMatrix(n, n, seed);
  // A^T * A + n*I ensures positive definiteness
  DataContainer G = AlgebraEngine::ComputeGram(A, true);
  for (UnsignedInteger i = 0; i < n; ++i)
    G(i, i) += n;
  return G;
}

struct BenchResult
{
  std::string method;
  UnsignedInteger n;
  UnsignedInteger blockSize;
  double time_ms;
};

static std::vector<BenchResult> results;

static void report()
{
  std::cout << std::endl;
  std::cout << std::left << std::setw(42) << "Method"
            << std::right << std::setw(8) << "N"
            << std::setw(8) << "BlkSz"
            << std::setw(12) << "Time(ms)"
            << std::setw(10) << "Speedup"
            << std::endl;
  std::cout << std::string(80, '-') << std::endl;

  std::string currentMethod;
  double denseTime = 0.0;
  for (const auto & r : results)
  {
    if (r.method != currentMethod)
    {
      if (!currentMethod.empty()) std::cout << std::endl;
      currentMethod = r.method;
      denseTime = 0.0;
    }
    if (r.blockSize == 0)
      denseTime = r.time_ms;

    double speedup = (r.blockSize > 0 && denseTime > 0) ? denseTime / r.time_ms : 0;
    std::cout << std::left << std::setw(42) << r.method
              << std::right << std::setw(8) << r.n
              << std::setw(8) << (r.blockSize == 0 ? std::string("dense") : std::to_string(r.blockSize))
              << std::setw(12) << std::fixed << std::setprecision(2) << r.time_ms
              << std::setw(10) << (speedup > 0 ? std::to_string(speedup).substr(0, 5) + "x" : "---")
              << std::endl;
  }
  std::cout << std::endl;
}

static void benchMatrixProduct(UnsignedInteger n)
{
  DataContainer A = makeRandomMatrix(n, n);
  DataContainer B = makeRandomMatrix(n, n);
  std::string label = "MatrixProduct";

  // Dense
  auto t0 = std::chrono::high_resolution_clock::now();
  DataContainer C = AlgebraEngine::MatrixProduct(A, B);
  auto t1 = std::chrono::high_resolution_clock::now();
  results.push_back({label, n, 0, elapsed_ms(t0, t1)});

  // Block variants
  for (UnsignedInteger bs : {64, 128, 256, 512, 1024})
  {
    t0 = std::chrono::high_resolution_clock::now();
    C = AlgebraEngine::MatrixProductBlockwise(A, B, bs);
    t1 = std::chrono::high_resolution_clock::now();
    results.push_back({label, n, bs, elapsed_ms(t0, t1)});
  }
}

static void benchCholesky(UnsignedInteger n)
{
  DataContainer A = makeSPDMatrix(n);
  std::string label = "Cholesky";

  auto t0 = std::chrono::high_resolution_clock::now();
  DataContainer L = AlgebraEngine::ComputeCholesky(A);
  auto t1 = std::chrono::high_resolution_clock::now();
  results.push_back({label, n, 0, elapsed_ms(t0, t1)});

  for (UnsignedInteger bs : {64, 128, 256, 512, 1024})
  {
    t0 = std::chrono::high_resolution_clock::now();
    L = AlgebraEngine::ComputeCholeskyBlockwise(A, bs);
    t1 = std::chrono::high_resolution_clock::now();
    results.push_back({label, n, bs, elapsed_ms(t0, t1)});
  }
}

static void benchQR(UnsignedInteger n)
{
  DataContainer A = makeRandomMatrix(n, n);
  std::string label = "QR";

  auto t0 = std::chrono::high_resolution_clock::now();
  DataContainer Q, R;
  AlgebraEngine::ComputeQR(A, Q, R);
  auto t1 = std::chrono::high_resolution_clock::now();
  results.push_back({label, n, 0, elapsed_ms(t0, t1)});

  for (UnsignedInteger bs : {64, 128, 256, 512, 1024})
  {
    t0 = std::chrono::high_resolution_clock::now();
    AlgebraEngine::ComputeQRBlockwise(A, Q, R, false, bs);
    t1 = std::chrono::high_resolution_clock::now();
    results.push_back({label, n, bs, elapsed_ms(t0, t1)});
  }
}

static void benchLU(UnsignedInteger n)
{
  DataContainer A = makeRandomMatrix(n, n);
  std::string label = "LU";

  auto t0 = std::chrono::high_resolution_clock::now();
  DataContainer L, U;
  AlgebraEngine::ComputeLU(A, L, U);
  auto t1 = std::chrono::high_resolution_clock::now();
  results.push_back({label, n, 0, elapsed_ms(t0, t1)});

  for (UnsignedInteger bs : {64, 128, 256, 512, 1024})
  {
    t0 = std::chrono::high_resolution_clock::now();
    AlgebraEngine::ComputeLUBlockwise(A, L, U, bs);
    t1 = std::chrono::high_resolution_clock::now();
    results.push_back({label, n, bs, elapsed_ms(t0, t1)});
  }
}

static void benchSolve(UnsignedInteger n)
{
  DataContainer A = makeSPDMatrix(n);
  DataContainer b = makeRandomMatrix(n, 1);
  std::string label = "SolveSPD";

  auto t0 = std::chrono::high_resolution_clock::now();
  DataContainer x = AlgebraEngine::SolveLinearSystemSPD(A, b);
  auto t1 = std::chrono::high_resolution_clock::now();
  results.push_back({label, n, 0, elapsed_ms(t0, t1)});

  for (UnsignedInteger bs : {64, 128, 256, 512, 1024})
  {
    t0 = std::chrono::high_resolution_clock::now();
    x = AlgebraEngine::SolveLinearSystemBlockwise(A, b, bs);
    t1 = std::chrono::high_resolution_clock::now();
    results.push_back({label, n, bs, elapsed_ms(t0, t1)});
  }
}

static void benchInverse(UnsignedInteger n)
{
  DataContainer A = makeRandomMatrix(n, n);
  std::string label = "Inverse";

  auto t0 = std::chrono::high_resolution_clock::now();
  DataContainer inv = AlgebraEngine::Inverse(A);
  auto t1 = std::chrono::high_resolution_clock::now();
  results.push_back({label, n, 0, elapsed_ms(t0, t1)});

  for (UnsignedInteger bs : {64, 128, 256, 512, 1024})
  {
    t0 = std::chrono::high_resolution_clock::now();
    inv = AlgebraEngine::InverseBlockwise(A, bs);
    t1 = std::chrono::high_resolution_clock::now();
    results.push_back({label, n, bs, elapsed_ms(t0, t1)});
  }
}

static void benchDeterminant(UnsignedInteger n)
{
  DataContainer A = makeRandomMatrix(n, n);
  std::string label = "Determinant";

  auto t0 = std::chrono::high_resolution_clock::now();
  Scalar det = AlgebraEngine::ComputeDeterminant(A);
  auto t1 = std::chrono::high_resolution_clock::now();
  results.push_back({label, n, 0, elapsed_ms(t0, t1)});
  (void)det;

  for (UnsignedInteger bs : {64, 128, 256, 512, 1024})
  {
    t0 = std::chrono::high_resolution_clock::now();
    det = AlgebraEngine::ComputeDeterminantBlockwise(A, bs);
    t1 = std::chrono::high_resolution_clock::now();
    results.push_back({label, n, bs, elapsed_ms(t0, t1)});
    (void)det;
  }
}

static void benchTriangularSolve(UnsignedInteger n)
{
  DataContainer A = makeRandomMatrix(n, n);
  // Make it lower triangular
  for (UnsignedInteger j = 0; j < n; ++j)
    for (UnsignedInteger i = 0; i < j; ++i)
      A(i, j) = 0.0;
  // Ensure non-zero diagonal
  for (UnsignedInteger i = 0; i < n; ++i)
    A(i, i) += n;

  DataContainer b = makeRandomMatrix(n, 1);
  std::string label = "TriangularSolve";

  auto t0 = std::chrono::high_resolution_clock::now();
  DataContainer x = AlgebraEngine::SolveLinearSystemTriangular(A, b, true, false);
  auto t1 = std::chrono::high_resolution_clock::now();
  results.push_back({label, n, 0, elapsed_ms(t0, t1)});

  for (UnsignedInteger bs : {64, 128, 256, 512, 1024})
  {
    t0 = std::chrono::high_resolution_clock::now();
    x = AlgebraEngine::SolveLinearSystemTriangularBlockwise(A, b, true, false, bs);
    t1 = std::chrono::high_resolution_clock::now();
    results.push_back({label, n, bs, elapsed_ms(t0, t1)});
  }
}

static void benchGram(UnsignedInteger n)
{
  DataContainer A = makeRandomMatrix(n, n / 2);
  std::string label = "Gram(A^T*A)";

  auto t0 = std::chrono::high_resolution_clock::now();
  DataContainer G = AlgebraEngine::ComputeGram(A, true);
  auto t1 = std::chrono::high_resolution_clock::now();
  results.push_back({label, n, 0, elapsed_ms(t0, t1)});

  for (UnsignedInteger bs : {64, 128, 256, 512, 1024})
  {
    t0 = std::chrono::high_resolution_clock::now();
    G = AlgebraEngine::ComputeGramBlockwise(A, true, bs);
    t1 = std::chrono::high_resolution_clock::now();
    results.push_back({label, n, bs, elapsed_ms(t0, t1)});
  }
}

static void benchRectSolve(UnsignedInteger n)
{
  // Overdetermined: 2n x n * x = b
  DataContainer A = makeRandomMatrix(2 * n, n);
  DataContainer b = makeRandomMatrix(2 * n, 1);
  std::string label = "RectSolve(2n x n)";

  auto t0 = std::chrono::high_resolution_clock::now();
  DataContainer x = AlgebraEngine::SolveLinearSystemRectangular(A, b);
  auto t1 = std::chrono::high_resolution_clock::now();
  results.push_back({label, 2 * n, 0, elapsed_ms(t0, t1)});

  for (UnsignedInteger bs : {64, 128, 256, 512})
  {
    t0 = std::chrono::high_resolution_clock::now();
    x = AlgebraEngine::SolveLinearSystemRectangularBlockwise(A, b, bs);
    t1 = std::chrono::high_resolution_clock::now();
    results.push_back({label, 2 * n, bs, elapsed_ms(t0, t1)});
  }
}

int main()
{
  std::vector<UnsignedInteger> sizes = {128, 256, 512, 1024};

  std::cout << "=== AlgebraEngine Block Method Benchmark ===" << std::endl;
  std::cout << "Comparing block variants against LAPACK dense (single block)" << std::endl;
  std::cout << "Block sizes tested: 64, 128, 256, 512, 1024" << std::endl;
  std::cout << "Matrix sizes (N): ";
  for (auto s : sizes) std::cout << s << " ";
  std::cout << std::endl;

  for (auto n : sizes)
  {
    std::cout << "\n--- N = " << n << " ---" << std::endl;
    benchMatrixProduct(n);
    benchCholesky(n);
    benchQR(n);
    benchLU(n);
    benchSolve(n);
    benchInverse(n);
    benchDeterminant(n);
    benchTriangularSolve(n);
    benchGram(n);
    benchRectSolve(n);
  }

  report();
  return 0;
}
