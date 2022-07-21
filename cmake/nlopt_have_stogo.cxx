#include <iostream>
#include <stdexcept>
#include <nlopt.hpp>

double myvfunc(const std::vector<double> &x, std::vector<double> &grad, void * /*my_func_data*/)
{
  if (!grad.empty()) {
    grad[0] = 2.0*x[0];
    grad[1] = 2.0*x[1];
  }
  return x[1]*x[1]+x[0]*x[0];
}

int main(int /*argc*/, const char* /*argv*/[])
{
  int n = 2;
  nlopt::opt opt(nlopt::GD_STOGO, n);
  std::vector<double> lb(n, -5.0);
  std::vector<double> ub(n, 5.0);
  opt.set_lower_bounds(lb);
  opt.set_upper_bounds(ub);
  opt.set_min_objective(myvfunc, NULL);
  opt.set_maxeval(100);
  std::vector<double> x(n, 1.0);
  double minf = 0.0;
  try {
    nlopt::result result = opt.optimize(x, minf);
    if ((int)result != NLOPT_SUCCESS)
      throw std::runtime_error("stogo error");
    return 0;
  }
  catch (std::exception & exc) {
    std::cout << "nlopt failed: " << exc.what() << std::endl;
    return 1;
  }
}
