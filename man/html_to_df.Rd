\name{html_to_df}
\alias{html_to_df}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
html_to_df}
\description{
html_to_df takes the raw html code typically produced by WCopyFind and translates it into a workable dataframe.}
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
Stephanie L. DeMora and Loren Collingwood
}
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
%% ~~objects to See Also as \code{\link{help}}, ~~~
}
\examples{
#Pull in the example data location at set it to "dir":
dir <- system.file("extdata", package="RCopyFind")

#Use the "file_name" function to list the appropraite file path for data conversion:
match_dat<-file_name(dir)

#Use the html_to_df function to convert the HTML code into a dataframe:
match_dat<- html_to_df(match_dat)

#Take a gander!
View(match_dat@data)
head(match_dat@data)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }% use one of  RShowDoc("KEYWORDS")
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
