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
numeric. The number of files to be compared. Default = 200
}
  \item{b}{
location. The file location for the reports from WCopyFind to print in.
}
  \item{c}{
numeric. The first comparison group number. This should remain at default = 1}
  \item{ca}{
The location of the first comparison file or group. }
  \item{d}{
numeric. The secodn comparison group number. This should remain at default = 2}
  \item{db}{
The location of the second comparison file or group.}
  \item{e}{
Set phrase Length. Default = 8}
  \item{f}{
Set Word Threshold. Default = 6}
  \item{g}{
Skip Length Setting Default = 20}
  \item{h}{
Mismatch Tolerance. Default = 9}
  \item{i}{
Mismatch Percentage. Default = 75}
  \item{j}{
Produces Brief Report (Yes = 1, No = 0). Default = 0
}
  \item{k}{
Ignore Case (Yes = 1, No = 0). Default = 0}
  \item{l}{
Ignore Number (Yes = 1, No = 0). Default = 0}
  \item{m}{
Ignore Outer Punctuation (Yes = 1, No = 0). Default = 0}
  \item{n}{
Ignore Punctuation (Yes = 1, No = 0). Default = 0}
  \item{o}{
Skip Long Words (Yes = 1, No = 0). Default = 0}
  \item{p}{
First group to compare. Should remain at default = 1}
  \item{q}{
Second group to compare. Should remain at default = 2}
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
