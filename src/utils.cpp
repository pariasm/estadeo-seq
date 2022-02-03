// This program is free software: you can use, modify and/or redistribute it
// under the terms of the simplified BSD License. You should have received a
// copy of this license along this program. If not, see
// <http://www.opensource.org/licenses/bsd-license.html>.
//
// Copyright (C) 2017, Javier Sánchez Pérez <jsanchez@ulpgc.es>
// All rights reserved.


#include <stdio.h>
#include "LibVideoT.hpp"
#include "direct_method/transformation.h"

/**
  *
  *  Function to get the frame size
  * 
**/
size_t get_frame_size(
  char *name,       //file name
  int first,        //first frame number
  int &width,       //size of the video
  int &height,      //size of the video
  int &channels     //size of the video
) 
{
	Video<float> input;
	input.loadVideo(name, first, first);

	if(input.sz.channels != 1 && input.sz.channels != 3)
	{
		printf("Video needs to have 1 or 3 channels\n");
		return 0;
	}

	width = input.sz.width;
	height = input.sz.height;
	channels = input.sz.channels;

	return width * height * channels;
}


/**
  *
  *  Function to read a video in raw data
  * 
**/
size_t read_video(
  char *name,      //file name
  int first,
  int last,
  int step,
  float *I,        //video to read
  int size         //size of the video
)
{
	Video<float> input;
	input.loadVideo(name, first, last, step);

	if(input.sz.channels != 1 && input.sz.channels != 3)
	{
		printf("Video needs to have 1 or 3 channels\n");
		return 0;
	}

	unsigned pos_I = 0;

	// copy single channel in 3 channel image
	if(input.sz.channels == 1)
	{
		for(int i = 0; i < input.sz.whf; ++i, pos_I+=3)
		{
			I[pos_I+0] = input(i);
			I[pos_I+1] = input(i);
			I[pos_I+2] = input(i);
		}
	}
	else
	{
		for (unsigned k = 0; k < input.sz.frames; ++k)
		for (unsigned j = 0; j < input.sz.height; ++j)
		for (unsigned i = 0; i < input.sz.width ; ++i)
		for (unsigned c = 0; c < input.sz.channels; ++c, ++pos_I)
			I[pos_I] = input(i,j,k,c);
	}

	return pos_I;
}


/**
  *
  *  Function to read a video in raw data
  *
**/
size_t write_video(
  char *name,       // file name
  int first,        // firs frame
  int step,         // frame step
  float *I,         // video to write
  int width,
  int height,
  int frames,
  int channels
)
{
	Video<float> output(width, height, frames, channels);

	unsigned pos_I = 0;
	for (unsigned k = 0; k < output.sz.frames; ++k)
	for (unsigned j = 0; j < output.sz.height; ++j)
	for (unsigned i = 0; i < output.sz.width ; ++i)
	for (unsigned c = 0; c < output.sz.channels; ++c, ++pos_I)
		output(i,j,k,c) = I[pos_I];

	output.saveVideo(name, first, step);
    
	return pos_I;
}


/**
  *
  *  Function to save transformations to a file
  * 
**/
void save_transforms(
  char *name,      //file name
  float *H,        //set of transformations
  int nparams,     //number of parameters of the transformations
  int ntransforms, //number of transformations
  int nx,          //image width
  int ny           //image height
)
{
/*FILE *fd=fopen(name,"w");

  if(fd!=NULL)
  {
    fprintf(fd,"%d %d %d %d\n", nparams, ntransforms, nx, ny);

    for(int i=0;i<ntransforms;i++)
    {
      for(int j=0;j<nparams;j++) fprintf(fd, "%.15f ", H[i*nparams+j]);
      fprintf(fd, "\n");
    }
    fclose(fd);
  }
  */


  FILE *fd=fopen(name,"w");

  if(fd!=NULL)
  {
    float matrix[9];

    for(int i=0;i<ntransforms;i++)
    {
      params2matrix(H + i*nparams, matrix, nparams);
      for(int j=0;j<9;j++) fprintf(fd, "%.15f ", matrix[j]);
      fprintf(fd, "\n");
    }
    fclose(fd);
  }

}


/**
  *
  *  Function to read transformations from a file
  * 
**/
void read_transforms(
  char *name,      //file name
  float *H,        //set of transformations
  int nparams,     //number of parameters of the transformations
  int ntransforms, //number of transformations
  int &nx,         //image width
  int &ny          //image height
)
{
  FILE *fd=fopen(name,"r");
  if(fd!=NULL)
  {
    int r=fscanf(fd,"%d %d %d %d", &nparams, &ntransforms, &nx, &ny);
    if(r>0)
      for(int i=0;i<ntransforms;i++) {
        for(int j=0;j<nparams;j++) r=fscanf(fd, "%f", &(H[i*nparams+j]));
      }
    fclose(fd);
  }
}
