/***************************************************************************
 *   Copyright (C) 2004 by Peter Snoblin                                   *
 *   pas@truman.edu                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef DFILESTREAM_H
#define DFILESTREAM_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include "dstring.h"
#include "functions.h"

//! A very basic file stream class
/*!
	This class implements a very simple method by which to access a file. It's read only, and a line by line basis.
*/
class dfilestream {
public:
	//! The basic empty constructor
	dfilestream();
	//! Reads from path
	/*!
		\param path The file to open
		\param mode The mode in which to open the file, r = read, w = write
		If file doesn't exist, nothing happens. If you try to read, you'll get errors.
		\sa dfilestream( char *path )
	*/
	dfilestream( dstring *path, char *mode );
	//! Reads from path
	/*!
		\param path The file to open
		\param mode The mode in which to open the file, r = read, w = write
		Of course, path must be null terminated. Also, note that the dstring version is prefered.
		\sa dfilestream(dstring *path)
	*/
	dfilestream( char *path, char *mode );
	~dfilestream();
	
	//! Opens a file
	/*!
		\param path The file to open
		\param mode The mode in which to open the file, r = read, a = write
		\return True on success, false on failure.
		\sa open( char *path )
	*/
	int open( dstring *path, char *mode );
	//! Opens a file
	/*!
		\param path The file to open
		\param mode The mode in which to open the file, r = read, w = write
		Of course, path must be null termianted. Also, note that the dstring version is prefered
		\return True on success, false on failure.
	*/
	int open( char *path, char *mode );
	
	//! fetches a line
	/*!
		\return The line read from the file.
		On EOF it will return a null pointer. If there is a blank line, an empty string will be returned. \n will be stripped from the line. If no valid file has been opened, it will return NULL.
	*/
	dstring *readline();
	
	//! Writes the line to a file
	/*!
		\param line The line to write
		This function writes the line to the file associated with the object, please note that it will appened a \n after the string.
	*/
	void writeline(dstring *line);
	
	//! Writes the line to a file
	/*!
		\param line The line to write
		Unlikle the dstring version of this method, this call writes the given integer to the file, on it's own seperate line. It is first translated to ascii.
	*/
	void writeline(int line);
	
	//! Returns the opened file
	/*!
		\return the name of the open file, NULL if no file is open.
	*/
	dstring *get_filename() const;
	
private:
	FILE *fh;
	char *filemode;
	dstring *filename;
};

#endif
