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


