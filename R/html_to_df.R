########################
# Function: html_to_df #
########################

html_to_df <- function(x) {
  
  # Takes File of HTML match WCopyFind output #
  # Basically resulting object to file_name() function call #
  
  # Check to install/load requisite packages
  packages <- c("RCurl", "XML", "rlist")
  check.packages <- function(pkg){
    new.pkg <- pkg[!(pkg %in% installed.packages()[, "Package"])]
    if (length(new.pkg))
      install.packages(new.pkg, dependencies = TRUE)
    sapply(pkg, require, character.only = TRUE)
  }
  check.packages(packages)
  
  # Read in Files #
  location <- getURL(x,.opts = list(ssl.verifypeer = FALSE) )
  tablez <- readHTMLTable(location)
  tablez <- list.clean(tablez, fun = is.null, recursive = FALSE)
  n.rows <- unlist(lapply(tablez, function(t) dim(t)[1]))
  dat_out <- as.data.frame(tablez[[which.max(n.rows)]])
  dat_out <- as.data.frame(sapply(dat_out, as.character), stringsAsFactors=F)
  colnames(dat_out) <- c("Perfect_Match", "Overall_Match", "View_both_Files",
                         "File_L", "File_R")
  # Convert counts to numeric
  dat_out$Perfect.Match_count = as.numeric(gsub("\\s* .*", "", dat_out$Perfect_Match))
  dat_out$Overall.Match_count = as.numeric(gsub("\\s* .*", "", dat_out$Overall_Match))
  
  dat_out$Overall.Match_percent = gsub("%.*", "", dat_out$Overall_Match)
  dat_out$Overall.Match_percent = as.numeric(gsub(".*\\(", "", dat_out$Overall.Match_percent))
  
  dat_out$Perfect.Match_percent = gsub("%.*", "", dat_out$Perfect_Match)
  dat_out$Perfect.Match_percent = as.numeric(gsub(".*\\(", "", dat_out$Perfect.Match_percent))
  dat_out$labs <- gsub(".rtf.txt", "", dat_out$File_R) # drop this nonsense for labeling.
  
  # Set up New Class
  dat_out_comb <- new("RCopy", data = dat_out)
  
  # Print out a little info on data.frame()
  cat("DataFrame dimensions:\n")
  print(dim(dat_out))
  
  # Return item #
  return ( dat_out_comb )
}
