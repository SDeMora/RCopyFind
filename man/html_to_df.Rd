\name{html_to_df}
\alias{html_to_df}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
html_to_df}
\description{
html_to_df takes the raw html code typically produced by WCopyFind and translates it into a workable dataframe.
}
\usage{
html_to_df(x)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{x}{
%%     ~~Describe \code{x} here~~
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
Stephanie L. DeMora and Loren Collingwood}
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
function (x) 
{
    packages <- c("RCurl", "XML", "rlist")
    check.packages <- function(pkg) {
        new.pkg <- pkg[!(pkg \%in\% installed.packages()[, "Package"])]
        if (length(new.pkg)) 
            install.packages(new.pkg, dependencies = TRUE)
        sapply(pkg, require, character.only = TRUE)
    }
    check.packages(packages)
    location <- getURL(x, .opts = list(ssl.verifypeer = FALSE))
    tablez <- readHTMLTable(location)
    tablez <- list.clean(tablez, fun = is.null, recursive = FALSE)
    n.rows <- unlist(lapply(tablez, function(t) dim(t)[1]))
    dat_out <- as.data.frame(tablez[[which.max(n.rows)]])
    dat_out <- as.data.frame(sapply(dat_out, as.character), stringsAsFactors = F)
    colnames(dat_out) <- c("Perfect_Match", "Overall_Match", 
        "View_both_Files", "File_L", "File_R")
    dat_out$Perfect.Match_count = as.numeric(gsub("\\s* .*", 
        "", dat_out$Perfect_Match))
    dat_out$Overall.Match_count = as.numeric(gsub("\\s* .*", 
        "", dat_out$Overall_Match))
    dat_out$Overall.Match_percent = gsub("\%.*", "", dat_out$Overall_Match)
    dat_out$Overall.Match_percent = as.numeric(gsub(".*\\(", 
        "", dat_out$Overall.Match_percent))
    dat_out$Perfect.Match_percent = gsub("\%.*", "", dat_out$Perfect_Match)
    dat_out$Perfect.Match_percent = as.numeric(gsub(".*\\(", 
        "", dat_out$Perfect.Match_percent))
    dat_out$labs <- gsub(".rtf.txt", "", dat_out$File_R)
    dat_out_comb <- new("RCopy", data = dat_out)
    cat("DataFrame dimensions:\n")
    print(dim(dat_out))
    return(dat_out_comb)
  }
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }% use one of  RShowDoc("KEYWORDS")
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
