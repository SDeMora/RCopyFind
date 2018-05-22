###################################
#####       Steph DeMora      #####
##### RCopyFind HTML Function #####
###################################

# Set initial class ( a bit different than what we had done before) #
################
# Class: RCopy #
################

setClass("RCopy", slots=c(data = "data.frame"), contains=c("data.frame"))

#######################
# Function: file_name #
#######################

# dir = string/character of directory where matches.html resides
file_name <- function(dir) {
  paste0("file://", dir, "/matches.html")
}

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

# Creating Path File #
#file_path <- "file:///Users/lorencollingwood/Dropbox/SYG%20Law%20(ALEC%20Paper)/WCopyFind_Laws_statutes_Results%20(NEW)/matches.html"
dir <- "C:/Users/xheli/Dropbox/Research/Complete Projects/SYG Law (ALEC Paper)/WCopyFind_Laws_statutes_Results (NEW)"
#file_path <- "C:/Users/xheli/Dropbox/Course%20Material/POSC%20207/RCopyFind_Steph_Private/matches.html"

match_dat <- html_to_df(file_name(dir)); head(match_dat@data) # note you need to call slot

###################################
# Function/Method: plot.RCopy     #
###################################
# takes object from html_to_df() function call, which is class RCopy
# count = logical, default = TRUE, raw count overlap, FALSE gives percents
# trim = logical, default = FALSE, TRUE trims count only 25 => word overlap; percent > 0
# ... other plot arguments

plot.RCopy <- function(x, count=TRUE, trim=FALSE,...) {

  if (count) { # Overall Match Counts #
    if (trim) { # Trim to Matches greater than 25
      x@data <- x@data[x@data$Overall.Match_count >= 25,]
    } # close trim

    # Sort Data for plotting display #
    x@data <- x@data[rev(order(x@data$Perfect.Match_count)),]
    xvar <- x@data[,7] # Count Number column
    labs <- x@data[,10] # Labels column#

    # Initiate Plot
    dotchart(xvar,
             xlab="Word Similarity",
             bty="n",
             main = "Textual Similarity to Model Legislation",
             labels=labs,
             ...)

  } else { # Overall Percent, not counts #

    if (trim) { # Trimmed to greater than 0% match
      x@data <- x@data[x@data$Overall.Match_percent > 0,]
    } # close trim

    # Sort Data for plotting display #
    x@data <- x@data[rev(order(x@data$Overall.Match_percent)),]
    xvar <- x@data[,8] # Percent Column
    labs <- x@data[,10] # Labels Column #

    # Initiate Plot #
    dotchart(xvar,
             xlab="Percent Word Similarity",
             bty="n",
             main = "Textual Similarity to Model Legislation",
             labels=labs,
             ...)
  } # close else

}

# Plot, Defaults -- Overall Counts, No trim; set cex here
plot(match_dat, cex=.4)

# Plot, Overall Counts, trimmed #
plot(match_dat, cex=.4, trim=T)

# Plot, Overall Percent Match
plot(match_dat, cex=.4, count=F)

# Plot, Overall Percent Match, trimmed
plot(match_dat, cex=.4, count=F, trim=T)


