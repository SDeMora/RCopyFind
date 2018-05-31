\name{scripter}
\alias{scripter}
scripter
\title{
scripter}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
scripter(a = 200, b = "C:\\Users\\xheli\\Desktop\\CReport", c = 1, ca = "C:\\Users\\xheli\\Desktop\\ALEC Model Leg.txt", d = 2, db = "C:\\Users\\xheli\\Desktop\\txt files", e = 8, f = 6, g = 20, h = 9, i = 75, j = 0, k = 1, l = 1, m = 1, n = 1, o = 1, p = 1, q = 2)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{a}{
%%     ~~Describe \code{a} here~~
}
  \item{b}{
%%     ~~Describe \code{b} here~~
}
  \item{c}{
%%     ~~Describe \code{c} here~~
}
  \item{ca}{
%%     ~~Describe \code{ca} here~~
}
  \item{d}{
%%     ~~Describe \code{d} here~~
}
  \item{db}{
%%     ~~Describe \code{db} here~~
}
  \item{e}{
%%     ~~Describe \code{e} here~~
}
  \item{f}{
%%     ~~Describe \code{f} here~~
}
  \item{g}{
%%     ~~Describe \code{g} here~~
}
  \item{h}{
%%     ~~Describe \code{h} here~~
}
  \item{i}{
%%     ~~Describe \code{i} here~~
}
  \item{j}{
%%     ~~Describe \code{j} here~~
}
  \item{k}{
%%     ~~Describe \code{k} here~~
}
  \item{l}{
%%     ~~Describe \code{l} here~~
}
  \item{m}{
%%     ~~Describe \code{m} here~~
}
  \item{n}{
%%     ~~Describe \code{n} here~~
}
  \item{o}{
%%     ~~Describe \code{o} here~~
}
  \item{p}{
%%     ~~Describe \code{p} here~~
}
  \item{q}{
%%     ~~Describe \code{q} here~~
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
Sephanie L. DeMora & Loren Collingwood
}
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
%% ~~objects to See Also as \code{\link{help}}, ~~~
}
\examples{
script<-scripter()
write.table(script,"script.txt", col.names = FALSE, row.names = FALSE, quote = FALSE)
shell("Copyfind64.4.1.5.exe < script.txt")
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }% use one of  RShowDoc("KEYWORDS")
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
