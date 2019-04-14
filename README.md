# RCopyFind

Type: R Package
Title: A Package to Compare HTML output from WCopyFind
Version: 1.0
Author: Stephanie L. DeMora, and Loren Collingwood
Maintainer: Stephanie L. DeMora <Stephanie.DeMora@email.ucr.edu>
Description: This package imports HTML files produced by plagiarism software, "WCopyFind" (for more information go to: http://plagiarism.bloomfieldmedia.com/wordpress/) and transforms them into workable datafames. Additionally, it produces dotplots for easy data visualization.

License: GPL-2
Depends: RCurl, XML, rlist



### Load packages:
library(devtools)

### Install RCopyFind
install_github("SDeMora/RCopyFind")

library(RCopyFind)

### For more information go to:
http://www.stephaniedemora.com/teaching-course-material/