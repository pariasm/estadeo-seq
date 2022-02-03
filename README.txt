Estadeo Video Stabilization
***************************

*******
SUMMARY
*******

This program implements a video stabilization method using parametric models.
Motion smoothing is one of the key steps for video stabilization. Its objective
is to remove the undesired motion of a video from the transformations computed
in a previous motion estimation process. In this work, we study several 
smoothing strategies. We assume that the motion of the scene is captured
through a set of parametric models, which are the most commonly used 
techniques. We propose a classication of strategies based on convolutions with
a Gaussian kernel. These strategies are classified into global and local 
methods, depending on how the reference frame is dened. Our study follows a 
unified formal approach and explains some details for the implementation of the 
techniques, such as the definition of correct boundary conditions in each case. 
The experiments show that local methods usually outperform global ones but, 
under certain conditions, global methods have a similar performance.

This program is part of an IPOL publication (http://www.ipol.im/).

Reference articles:

[1] Javier Sanchez, "COMPARISON OF MOTION SMOOTHING STRATEGIES FOR 
VIDEO STABILIZATION USING PARAMETRIC MODELS", Image Processing Online, 2017.

[2] Javier Sanchez and Jean-Michel Morel, "MOTION SMOOTHING STRATEGIES FOR 
VIDEO STABILIZATION", SIAM J. Imaging Sciences, 2017.

******
AUTHOR
******

Javier Sánchez Pérez <jsanchez@ulpgc.es> 
Centro de Tecnologías de la Imagen (CTIM) 
Universidad de Las Palmas de Gran Canaria

*********
CHANGELOG
*********

Version 1.2, released on February 3, 2022 
	Pablo Arias: I/O from RGB images sequences using iio

Version 1.1, released on June 16, 2017 
	Thiaud Ehret: I/O from images sequences using iio

Version 1,   released on May 7, 2017
	Original release.

*******
LICENSE
*******

This program is free software: you can use, modify and/or redistribute it
under the terms of the simplified BSD License. You should have received a
copy of this license along this program. If not, see
<http://www.opensource.org/licenses/bsd-license.html>.

Copyright (C) 2017, Javier Sánchez Pérez <jsanchez@ulpgc.es>
All rights reserved.

***********
COMPILATION
***********

Required environment: Any unix-like system with a standard compilation
environment (make and C and C++ compilers)

Compilation instructions: run "make" to produce two executables:
 - "estadeo" the main algorithm
 - "generate_output" auxiliary program for the online demo

*****
USAGE
*****

The program reads an sequence of image files and produces an output sequence
with the motion stabilized. In only works with images of 1 or 3 channels.

Usage instructions obtained by executing the programe with no inputs:
```
bin/estadeo
```
   
*************
LIST OF FILES
*************
main.cpp: Main function to be called from the command line. It reads and check 
  the parameters and call the estadeo program
estadeo.cpp: Implements the estadeo video stabilization
motion_smoothing.cpp: Implements the transformation smoothing strategies
video_cooling.cpp: Methods to improve the video after stabilization
color_bicubic_interpolation.cpp: Functions to warp the video using bicubic 
  interpolation
gauss_conv_dct.cpp: Convolution with a Gaussian function through DCT
sutherland_hodgman.cpp: Polygon clipping using the Sutherland & Hodgman method
utils.cpp: Functions to read and write videos and transform files 
cmline_execute.sh: Script to be executed from the command line that facilitates
  the process of converting videos to/from raw data and calling the estadeo 
  algorithm

Complementary programs:
direct_method: a directory with the code of parametric motion estimation. 
  This is an implementation of the inverse compositional algorithm that is 
  available at http://www.ipol.im/pub/pre/153/
feature_based_method: a directory for feature-based image matching. This is
  the code from the ORSA method, http://www.ipol.im/pub/art/2012/mmm-oh/
generate_graphics.cpp: Program to create some output information used in the 
  online demo only
estadeo.sh: Script used from the IPOL demo to facilitate the process of 
  converting videos to/from raw data and calling the estadeo method
