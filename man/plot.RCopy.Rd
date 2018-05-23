\name{plot.RCopy}
\alias{plot.RCopy}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
plot.RCopy}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
plot.RCopy(x, count = TRUE, trim = FALSE, ...)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{x}{
%%     ~~Describe \code{x} here~~
}
  \item{count}{
%%     ~~Describe \code{count} here~~
}
  \item{trim}{
%%     ~~Describe \code{trim} here~~
}
  \item{\dots}{
%%     ~~Describe \code{\dots} here~~
}
}
\details{
%%  ~~ If necessary, more details than the description above ~~
}
\value{
%%  ~Describe the value returned
%%  If it is a LIST, use
%%  \item{comp1 }{Description of 'comp1'}
%%  \item{comp2 }{Description of 'comp2'}
%% ...
}
\references{
%% ~put references to the literature/web site here ~
}
\author{
%%  ~~who you are~~
}
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
%% ~~objects to See Also as \code{\link{help}}, ~~~
}
\examples{
##---- Should be DIRECTLY executable !! ----
##-- ==>  Define data, use random,
##--	or do  help(data=index)  for the standard data sets.

## The function is currently defined as
function (x, count = TRUE, trim = FALSE, ...) 
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
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }% use one of  RShowDoc("KEYWORDS")
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
