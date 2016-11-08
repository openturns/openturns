library(DiceDesign)

# Comparison to be done with space_filling_criteria.val
filename <- "design_dim_2_size_20_centered_perturbLHS.csv"
data <- read.table(filename, sep=";")
minDist <- mindist(data)
phip <- phiP(data)
c2 <- discrepancyCriteria(data,type='C2')$DisC2
values_formated <- paste0("c2=", sprintf("%1.10e", c2), ", phip=", sprintf("%1.10e", phip), ", mindist=", sprintf("%1.10e", minDist))

content_file <- c()
content_file <- c(content_file, c(paste0("initial design=", filename), values_formated))

fileConn <-file("space_filling_criteria_perturbation_ref.val", open="w")

# Perturbations
dimension <- 2
size <- 20
for (row1 in 1:size)
{
  for (row2 in 1:size)
  {
    for (column in 1:dimension)
    {
      # The 3-tuple (row1, row2, column)
      # Criteria exchange design[row1, column] and design[row2, column]
      updated_c2 <- sqrt( DiceDesign:::discrepancyC2_EP(data, row1, row2, column, c2*c2) )
      values_formated <- paste0("row1=",row1, ", row2=", row2, ", column=", column, ", updated c2=", sprintf("%1.10e", updated_c2))
      writeLines(values_formated, fileConn)
    }
  }
}
close(fileConn)

## New test with biggest dimension
fileConn <-file("space_filling_criteria_perturbation_ref.val", open="a+")
dimension <- 50
size <- 100
# export the design
filename <- "design_dim_50_size_100_centered_perturbLHS.csv"
data <- read.table(filename, sep=";")
minDist <- mindist(data)
phip <- phiP(data)
c2 <- discrepancyCriteria(data,type='C2')$DisC2
values_formated <- paste0("c2=", sprintf("%1.10e", c2), ", phip=", sprintf("%1.10e", phip), ", mindist=", sprintf("%1.10e", minDist))
writeLines(c(paste0("initial design=", filename), values_formated), fileConn)

# Perturbations
for (row1 in seq(1,size,10))
{
  for (row2 in seq(1,size,10))
  {
    for (column in seq(1,dimension,10))
    {
      # The 3-tuple (row1, row2, column)
      # Criteria exchange design[row1, column] and design[row2, column]
      updated_c2 <- sqrt( DiceDesign:::discrepancyC2_EP(data, row1, row2, column, c2*c2) )
      values_formated <- paste0("row1=",row1, ", row2=", row2, ", column=", column, ", updated c2=", sprintf("%1.10e", updated_c2))
      writeLines(values_formated, fileConn)
    }
  }
}
close(fileConn)
