library(DiceDesign)

# For each design, evaluation of some criteria
# Comparison to be done with space_filling_criteria.val
basename <- "design_dim_2_size_20_"
files <- paste0(basename, 1:10, "_nonunit.csv")

content_file <- c()
for (f in files)
{
  data <- data <- read.table(f, sep=";")
  minDist <- mindist(data)
  phip <- phiP(data)
  c2 <- discrepancyCriteria(data,type='C2')$DisC2
  values_formated <- paste0("c2=", sprintf("%1.13e", c2), ", phip=", sprintf("%1.13e", phip), ", mindist=", sprintf("%1.13e", minDist))
  content_file <- c(content_file, c(paste0("design=", f), values_formated))
}

basename <- "design_dim_50_size_100_"
files <- paste0(basename, 1:5, "_nonunit.csv")
for (f in files)
{
  data <- data <- read.table(f, sep=";")
  minDist <- mindist(data)
  phip <- phiP(data)
  c2 <- discrepancyCriteria(data,type='C2')$DisC2
  values_formated <- paste0("c2=", sprintf("%1.13e", c2), ", phip=", sprintf("%1.13e", phip), ", mindist=", sprintf("%1.13e", minDist))
  content_file <- c(content_file, c(paste0("design=", f), values_formated))
}

fileConn <-file("space_filling_criteria_nonunit_ref.val")
writeLines(content_file, fileConn)
close(fileConn)

