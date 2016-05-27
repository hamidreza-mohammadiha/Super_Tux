#ifndef __FIND_RECTS_HPP__
#define __FIND_RECTS_HPP__

#include <climits>

namespace FindRects {

	/** Input aray type, typically the smallest integral type */
	typedef unsigned char InputType;
	/** Output array type, which consists of the width and height of any given rectangle at this point */
	typedef unsigned char OutputType;
	enum { OUTPUT_MAX_LENGTH = UCHAR_MAX };

	/** A rectangle, (x,y) is the upper-left corner, coordinates start at (0,0) */
	struct Rect {
		int x;
		int y;
		int w;
		int h;
		Rect(int x, int y, int w, int h): x(x), y(y), w(w), h(h) {}
		/** Returns true if a point (x,y) is inside this rectangle */
		bool isPointInside(int x, int y) { return x >= this->x && x < this->x + this->w && y >= this->y && y < this->y + this->h; }
	};

	/** Find a biggest rectangle in a given two-dimensional array
		@param data: two-dimensional array, value 0 is empty area, any non-zero value is considered for searching
		@param width: a width of data array
		@param height: a height of data array
	*/
	Rect findBiggest(const InputType* data, int width, int height);

	/** Split an area into rectangles, trying to cover as much area as possible
		@param minLength: minimal length of the rectangle side, can be 1 to count every element in the array
		@param output: output array of width x height * 2, where each two elements are width/height of a rectangle's top-left corner, or 0 if the area is inside a rectangle
		@return total area size of all rectangles, covered by rectangles with side length = maxLength or bigger,
	*/
	long long findAll (const InputType* input, int width, int height, int minLength, OutputType* output);

}

#endif
