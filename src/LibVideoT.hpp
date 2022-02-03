/*
 * Copyright (c) 2015, Pablo Arias <pariasm@gmail.com>
 * All rights reserved.
 *
 * This program is free software: you can use, modify and/or
 * redistribute it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later
 * version. You should have received a copy of this license along
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIB_VIDEOT_HPP_INCLUDED
#define LIB_VIDEOT_HPP_INCLUDED

#include <vector>
#include <string>
#include <stdexcept>
#include <cassert>
#include <climits>
//#include <cstdio>
#include <cmath>


#include "mt19937ar.h"
#include "Utilities.h"

/**
 * @brief Structure containing size informations of a video.
 *
 * @param width    : width of the image;
 * @param height   : height of the image;
 * @param channels : number of channels in the image;
 * @param frames   : number of frames in the video;
 * @param wh       : equal to width * height. Provided for convenience;
 * @param whc      : equal to width * height * channels. Provided for convenience.
 * @param whcf     : equal to width * height * frames * channels. Provided for convenience.
 * @param whf      : equal to width * height * frames. Provided for convenience.
 **/
struct VideoSize
{
	unsigned width;
	unsigned height;
	unsigned frames;
	unsigned channels;
	unsigned wh;
	unsigned whc;
	unsigned whcf;
	unsigned whf;

	//! Constuctors
	VideoSize(void)
		: width(0), height(0), frames(0), channels(0)
	{
		update_fields();
	}

	VideoSize(unsigned w, unsigned h, unsigned f, unsigned c)
		: width(w), height(h), frames(f), channels(c)
	{
		update_fields();
	}

	//! Comparison operators
	inline bool operator == (const VideoSize& sz) const
	{
		return (width    == sz.width     &&
		        height   == sz.height    &&
		        channels == sz.channels  &&
		        frames   == sz.frames    );
	}

	inline bool operator != (const VideoSize& sz) const
	{ 
		return !operator==(sz);
	}

	//! Updates products of dimensions
	inline void update_fields(void)
	{
		wh = width * height;
		whc = wh * channels;
		whcf = whc * frames;
		whf  = wh  * frames;
	}

	//! Returns index
	inline unsigned index(unsigned x, unsigned y, unsigned t, unsigned c) const
	{
		assert(x < width && y < height && t < frames && c < channels);
		return t*whc + c*wh + y*width + x;
	}

	//! Returns index assuming the video has one channel
	inline unsigned index(unsigned x, unsigned y, unsigned t) const
	{
		assert(x < width && y < height && t < frames);
		return t*wh + y*width + x;
	}

	//! Compute coordinates from index
	inline
	void coords(unsigned idx, unsigned& x, unsigned& y, unsigned& t, unsigned& c) const
	{
		assert(idx < whcf);
		t = (idx      ) / whc;
		c = (idx % whc) / wh ;
		y = (idx % wh ) / width;
		x = (idx % width  );
	}

	//! Compute coordinates from index assuming the video has one channel
	inline
	void coords(unsigned idx, unsigned& x, unsigned& y, unsigned& t) const
	{
		assert(idx < whf);
		t = (idx      ) / wh;
		y = (idx % wh ) / width;
		x = (idx % width  );
	}
};

/**
 * @brief A video class template with very basic functionalities.
 *
 * NOTE: should not be used with T = bool, since current implementation
 * relies on std::vector, and std::vector<bool> cannot return a non-
 * constant reference to an element of the array.
 *
 * @param sz       : VideoSize structure with size of the video;
 * @param data     : pointer to an std::vector<T> containing the data
 **/
template <class T>
class Video
{
	public:

		//! Size
		VideoSize sz;

		//! Data
		std::vector<T> data;

		//! Constructors
		Video(void); //< empty
		Video(const Video& i_in); //< copy
		Video(const std::string i_pathToFiles,
		          unsigned i_firstFrame, unsigned i_lastFrame, unsigned i_frameStep = 1); //< from filename
		Video(unsigned i_width, unsigned i_height, unsigned i_frames, unsigned i_channels = 1);  //< alloc
		Video(unsigned i_width, unsigned i_height, unsigned i_frames, unsigned i_channels, T val);  //< init
		Video(const VideoSize& i_size);  //< alloc
		Video(const VideoSize& i_size, T val);  //< init

		//! Destructor
		~Video(void) { };

		void clear(void);
		void resize(unsigned i_width, unsigned i_height, unsigned frames, unsigned i_channels = 1);
		void resize(const VideoSize& i_size);

		//! Read/write pixel access ~ inline for efficiency
		T& operator () (unsigned idx); //< from coordinates
		T& operator () (unsigned x, unsigned y, unsigned t, unsigned c = 0); //< from coordinates

		//! Read only pixel access ~ inline for efficiency
		T operator () (unsigned idx) const; //< from index
		T operator () (unsigned x, unsigned y, unsigned t, unsigned c = 0) const; //< from coordinates

		//! Pixel access with special boundary conditions
		T& getPixelSymmetric(int x, int y, int t, unsigned c = 0);
		T  getPixelSymmetric(int x, int y, int t, unsigned c = 0) const;
		
		//! I/O
		void loadVideo(const std::string i_pathToFiles, 
		               unsigned i_firstFrame, unsigned i_lastFrame, unsigned i_frameStep = 1);
		void saveVideo(const std::string i_pathToFiles, 
		               unsigned i_firstFrame, unsigned i_frameStep = 1,
		               T i_pmin = 0, T i_pmax = 255) const;
		void saveVideoAscii(const std::string i_prefix, 
		                    unsigned i_firstFrame, unsigned i_frameStep = 1) const;

		void transformVideoToBayer(const Video<T>& input);
		void transformVideoFromBayer(const Video<T>& input);

		std::vector<T> dct_shuffle_video(int initFrame, int lastFrame);
};

// Implementations

template <class T> 
Video<T>::Video(void)
	: sz(), data(0)
{ }
	
template <class T> 
Video<T>::Video(const Video& i_in)
	: sz(i_in.sz), data(i_in.data)
{ }

template <class T> 
Video<T>::Video(
	const std::string i_pathToFiles
,	unsigned i_firstFrame
,	unsigned i_lastFrame
,	unsigned i_frameStep
) : sz(), data(0)
{
	loadVideo(i_pathToFiles, i_firstFrame, i_lastFrame, i_frameStep);
}
	
template <class T> 
Video<T>::Video(
	unsigned i_width
,	unsigned i_height
,	unsigned i_frames
,	unsigned i_channels
)
	: sz(i_width, i_height, i_frames, i_channels)
	, data(sz.whcf)
{ }

template <class T> 
Video<T>::Video(
	unsigned i_width
,	unsigned i_height
,	unsigned i_frames
,	unsigned i_channels
,	T val
)
	: sz(i_width, i_height, i_frames, i_channels)
	, data(sz.whcf, val)
{ }
	
template <class T> 
Video<T>::Video(const VideoSize& i_size, T val)
	: sz(i_size)
	, data(sz.whcf, val)
{ }
	
template <class T> 
Video<T>::Video(const VideoSize& i_size)
	: sz(i_size)
	, data(sz.whcf)
{ }
	
template <class T> 
void Video<T>::clear(void)
{
	sz.width = 0;
	sz.height = 0;
	sz.frames = 0;
	sz.channels = 0;
	sz.update_fields();
	data.clear();
}

template <class T> 
void Video<T>::resize(const VideoSize& i_size)
{
	if (sz != i_size)
	{
		clear();
		sz = i_size;
		data.resize(sz.whcf);
	}
}

template <class T> 
void Video<T>::resize(
	unsigned i_width
,	unsigned i_height
,	unsigned i_frames
,	unsigned i_channels
){
	resize(VideoSize(i_width, i_height, i_frames, i_channels));
}
	
template <class T> 
void Video<T>::loadVideo(
    const std::string i_pathToFiles
,   unsigned i_firstFrame
,   unsigned i_lastFrame
,   unsigned i_frameStep
){
	throw std::runtime_error("Video<T>::loadVideo(...) is only implemented "
			"for T = float");
}

template <> void Video<float>::loadVideo(
    const std::string i_pathToFiles
,   unsigned i_firstFrame
,   unsigned i_lastFrame
,   unsigned i_frameStep
);

template <class T> 
std::vector<T> Video<T>::dct_shuffle_video(int i, int f)
{
	throw std::runtime_error("Video<T>::dct_shuffle_video(...) is only implemented "
			"for T = float");
}

template <> std::vector<float> Video<float>::dct_shuffle_video(int i, int f);

template <class T> 
void Video<T>::saveVideo(
	const std::string i_pathToFiles
,	unsigned i_firstFrame
,	unsigned i_frameStep
,	T i_pmin
,	T i_pmax
) const {
	throw std::runtime_error("Video<T>::saveVideo(...) is only implemented "
			"for T = float");
}

template <> void Video<float>::saveVideo(
	const std::string i_pathToFiles
,	unsigned i_firstFrame
,	unsigned i_frameStep
,	float i_pmin
,	float i_pmax
) const;

template <class T> 
void Video<T>::transformVideoFromBayer(
		const Video<T>& input
) {
	throw std::runtime_error("Video<T>::transformVideoFromBayer(...) is only implemented "
			"for T = float");
}
template <> void Video<float>::transformVideoFromBayer(
		const Video<float>& input
		) ;

template <class T> 
void Video<T>::transformVideoToBayer(
		const Video<T>& input
) {
	throw std::runtime_error("Video<T>::transformVideoToBayer(...) is only implemented "
			"for T = float");
}
template <> void Video<float>::transformVideoToBayer(
		const Video<float>& input
		) ;

	
template <class T> 
void Video<T>::saveVideoAscii(
	const std::string i_prefix
,	unsigned i_firstFrame
,	unsigned i_frameStep
) const {
	throw std::runtime_error("Video<T>::saveVideoAscii(...) is only implemented "
			"for T = float");
}

template <> void Video<float>::saveVideoAscii(
	const std::string i_prefix
,	unsigned i_firstFrame
,	unsigned i_frameStep
) const;

template <class T>
inline T& Video<T>::operator () (unsigned idx) 
{
	assert(idx < sz.whcf);
	return data[idx];
}

template <class T>
inline T& Video<T>::operator() (
	unsigned x
,	unsigned y
,	unsigned t
,	unsigned c
){
	return data[sz.index(x,y,t,c)];
}

template <class T>
inline T Video<T>::operator () (unsigned idx) const
{
	assert(idx < sz.whcf);
	return data[idx];
}

template <class T>
inline T Video<T>::operator() (
	unsigned x
,	unsigned y
,	unsigned t
,	unsigned c
) const {
	return data[sz.index(x,y,t,c)];
}

template <class T>
inline T& Video<T>::getPixelSymmetric(
	int x
,	int y
,	int t
,	unsigned c
) {
	// NOTE: assumes that -width+1 < x < 2*(width -1)
	assert(-(int)sz.width   < x && x < 2*(int)sz.width -1&&
	       -(int)sz.height  < y && y < 2*(int)sz.height-1&&
	       -(int)sz.frames  < t && t < 2*(int)sz.frames-1);
	// symmetrize
	x = (x < 0) ? -x : (x >= (int)sz.width  ) ? 2*(int)sz.width  - 2 - x : x ;
	y = (y < 0) ? -y : (y >= (int)sz.height ) ? 2*(int)sz.height - 2 - y : y ;
	t = (t < 0) ? -t : (t >= (int)sz.frames ) ? 2*(int)sz.frames - 2 - t : t ;

	return data[sz.index(x,y,t,c)];
}

template <class T>
inline T Video<T>::getPixelSymmetric(
	int x
,	int y
,	int t
,	unsigned c
) const {
	// NOTE: assumes that -width+1 < x < 2*(width -1)
	assert(-(int)sz.width   < x && x < 2*(int)sz.width  - 1 &&
	       -(int)sz.height  < y && y < 2*(int)sz.height - 1 &&
	       -(int)sz.frames  < t && t < 2*(int)sz.frames - 1 );
	// symmetrize
	x = (x < 0) ? -x : (x >= (int)sz.width  ) ? 2*(int)sz.width  - 2 - x : x ;
	y = (y < 0) ? -y : (y >= (int)sz.height ) ? 2*(int)sz.height - 2 - y : y ;
	t = (t < 0) ? -t : (t >= (int)sz.frames ) ? 2*(int)sz.frames - 2 - t : t ;

	return data[sz.index(x,y,t,c)];
}


#endif // LIB_VIDEOT_HPP_INCLUDED
