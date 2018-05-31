scripter<-function(a=200,b="C:\\Users\\xheli\\Desktop\\CReport",
                   c=1,ca="C:\\Users\\xheli\\Desktop\\ALEC Model Leg.txt",
                   d=2,db="C:\\Users\\xheli\\Desktop\\txt files",
                   e=8,f=6,g=20,h=9,i=75,j=0,k=1,l=1,m=1,n=1,o=1,p=1,q=2){
  arg1 <- paste("Documents,",a, sep="")
  arg2 <- paste("ReportFolder,",b,sep="")
  arg3 <- paste("Document,",c,",",ca,sep="")
  arg4 <- paste("Document,",d,",",db,sep="")
  arg5 <- paste("PhraseLength,",e,sep="")
  arg6 <- paste("WordThreshold,",f,sep="")
  arg7 <- paste("SkipLength,",g,sep="")
  arg8 <- paste("MismatchTolerance,",h,sep="")
  arg9 <- paste("MismatchPercentage,",i,sep="")
  arg10 <- paste("BriefReport,",j,sep="")
  arg11 <- paste("IgnoreCase,",k,sep="")
  arg12 <- paste("IgnoreNumber,",l,sep="")
  arg13 <- paste("IgnoreOuterPunctuation,",m,sep="")
  arg14 <- paste("IgnorePunctuation,",n,sep="")
  arg15 <- paste("SkipLongWords,",o,sep="")
  arg16<- paste("Locale,English")
  arg17 <- paste("Compare,",p,",",q,sep="")
  script<-paste(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,
                arg10,arg11,arg12,arg13,arg14,arg15,arg16,
                "PrepareForComparisons",
                arg17,
                "Done",
                sep="\n")
  return(script)
}