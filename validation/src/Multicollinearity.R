# This script computes variance-based importance measures (LMG and PMVD) in the context of linear regression
# The test cases are borrowed from https://gitlab.com/LauraClouvel/toydata/

library(relaimpo)
library(sensitivity)
library(AmesHousing)
library(caret)
library(mlbench)
library(mvtnorm)

#
# Compute a measure for the given variables
#
# Parameters
# - x: a data.frame containing the input variables
# - y: a vector containing the output variable
# - type: the measure type ("LMG" or "PMVD")
# - package: the name of the package that will compute the measure ("relaimpo" or "sensitivity")
#
# Output
#   A vector containing the measure for each input variable
#
compute_measure <- function(x, y, type, package)
{
  if (type=="LMG")
  {
    if (package=="relaimpo")
    {
      res <- calc.relimp(y, x, type="lmg")
      return(res$lmg)
    }
    else if (package=="sensitivity")
    {
      res <- lmg(x, y)
      return(res$lmg[,1])
    }
    else
      stop("Unknown package")
  }
  else if (type=="PMVD")
  {
    if (package=="relaimpo")
    {
      res <- calc.relimp(y, x, type="pmvd")
      return(res$pmvd)
    }
    else if (package=="sensitivity")
    {
      res <- pmvd(x, y)
      return(res$pmvd[,1])
    }
    else
      stop("Unknown package")
  }
  else
    stop("Unknown type")
}

header <- function(text)
{
  cat(strrep("-", 50), "\n")
  cat(text, "\n")
  cat(strrep("-", 50), "\n")
  cat("\n")
}

get_data1 <- function()
{
  header("Case #1 - Linear model: Y = X1^2 + X2 + X3")
  n <- 100
  x <- data.frame(X1 = runif(n, 0.5, 1.5), X2 = runif(n, 1.5, 4.5), X3 = runif(n, 4.5, 13.5))
  y <- with(x, X1^2 + X2 + X3)
  return(list(x=x, y=y, rep=100))
}

get_data2 <- function()
{
  header("Case #2 - Linear model: Y = X1^2 + X2 + X3 + X4")
  n <- 100
  x <- data.frame(X1 = runif(n, 0.5, 1.5), X2 = runif(n, 1.5, 4.5), X3 = runif(n, 4.5, 13.5))
  x <- data.frame(x, X4 = x$X3 + rnorm(n))
  y <- with(x, X1^2 + X2 + X3 + X4)
  return(list(x=x, y=y, rep=100))
}

get_data3 <- function()
{
  header("Case #3 - Linear model: Y = X1 + eta")
  n <- 100
  xx <- rmvnorm(n, mean=c(0,0), sigma=matrix(c(1,0.9,0.9,1), ncol=2))
  x <- data.frame(X1 = xx[,1], X2 = xx[,2])
  y <- with(x, X1 + rnorm(n,0,0.1))
  return(list(x=x, y=y, rep=100))
}

get_data4 <- function()
{
  header("Case #4 - Linear model: Y = X1 + 2X2 + eta")
  n <- 100
  xx <- rmvnorm(n, mean=c(0,0), sigma=matrix(c(1,0.9,0.9,1), ncol=2))
  x <- data.frame(X1 = xx[,1], X2 = xx[,2])
  y <- with(x, X1 + 2 * X2 + rnorm(n,0,0.1))
  return(list(x=x, y=y, rep=100))
}

get_data5 <- function()
{
  header("Case #5 - Air quality")
  a = airquality
  b = a[ !is.na(a[,1]) & !is.na(a[,2]),]
  y <- b$Ozone
  x <- b[,-1]
  return(list(x=x, y=y, rep=100))
}

get_data6 <- function()
{
  header("Case #6 - Boston housing")
  data(BostonHousing2)
  a = BostonHousing2
  b <- a[,-c(1,2,3,4,5,10)]
  y <- b$cmedv
  x <- b[,-1]
  return(list(x=x, y=y, rep=5))
}

get_data7 <- function()
{
  header("Case #7 - Cars")
  data(cars)
  a <- cars
  b <- a[,-c(13,15,18)]
  y <- b$Price
  x <- b[,-1]
  return(list(x=x, y=y, rep=2))
}

get_data8 <- function()
{
  header("Case #8 - Ames Sale House Prices")
  a <- make_ames()
  y <- a$Sale_Price
  x <- a[c("Second_Flr_SF", "First_Flr_SF", "Total_Bsmt_SF", "Year_Built", "Year_Remod_Add", "Bedroom_AbvGr", "Kitchen_AbvGr", "Mas_Vnr_Area", "TotRms_AbvGrd", "Garage_Cars")]
  return(list(x=data.frame(x), y=y, rep=5))
}

#
# Compute a measure for the given test case, and display the value and the calculation time
#
# Parameters
# - case: the case number (between 1 and 8)
# - type: the measure type ("LMG" or "PMVD")
# - package: the name of the package that will compute the measure ("relaimpo" or "sensitivity")
#
compute <- function(case, type, package)
{
  # Get the data for the test case
  set.seed(123456)
  data <- switch(case, get_data1(), get_data2(), get_data3(), get_data4(), get_data5(), get_data6(), get_data7(), get_data8())
  if (is.null(data))
    stop("Invalid case number")

  # Compute the measure and the calculation time
  time1 <- Sys.time()
  for (i in 1:data$rep)
    measure <- compute_measure(data$x, data$y, type, package)
  time2 <- Sys.time()
  duration <- (time2 - time1) / data$rep

  # Print the results
  res <- as.matrix(measure)
  colnames(res) <- paste0(type, " (", package, ")")
  print(res)
  cat("\nDuration:", formatC(duration, digits=2), "secs\n\n")
}

cases <- 1:8
for(case in cases)
  compute(case, "LMG", "relaimpo")
