###################################
# Function/Method: plot.RCopy     #
###################################
# takes object from html_to_df() function call, which is class RCopy
# count = logical, default = TRUE, raw count overlap, FALSE gives percents
# trim = logical, default = FALSE, TRUE trims count only 25 => word overlap; percent > 0
# ... other plot arguments

plot.RCopy <-function (x, count = TRUE, trim = FALSE, ...) 
{
    if (count) {
        if (trim) {
            x@data <- x@data[x@data$Overall.Match_count >= 25, 
                ]
        }
        x@data <- x@data[rev(order(x@data$Perfect.Match_count)), 
            ]
        xvar <- x@data[, 7]
        labs <- x@data[, 10]
        dotchart(xvar, xlab = "Word Similarity", bty = "n", main = "Textual Similarity to Model Legislation", 
            labels = labs, ...)
    }
    else {
        if (trim) {
            x@data <- x@data[x@data$Overall.Match_percent > 0, 
                ]
        }
        x@data <- x@data[rev(order(x@data$Overall.Match_percent)), 
            ]
        xvar <- x@data[, 8]
        labs <- x@data[, 10]
        dotchart(xvar, xlab = "Percent Word Similarity", bty = "n", 
            main = "Textual Similarity to Model Legislation", 
            labels = labs, ...)
    }
}
