#pragma once
/*
  Zipios++ - a small C++ library that provides easy access to .zip files.
  Copyright (C) 2000-2015  Thomas Sondergaard
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

/** \file
    Header file that defines a number of exceptions used by FileCollection and
    its subclasses.
*/


#include "zipios++/zipios-config.h"

#include <stdexcept>
#include <string>

namespace zipios
{


/** \brief Base exception of the zipios environement
 *
 * Unfortunately, all exceptions are marked as runtime_error.
 *
 * However, if we find a problem we will throw logic_error instead.
 * So if you get a logic_error, it is an error that we assume should
 * never occur. A runtime_error, on the other hand, is expected to
 * happen once in a while (i.e. cannot create a file, cannot read
 * a file, etc.)
 */
class Exception : public std::runtime_error
{
public:
  Exception( std::string const& msg ) : runtime_error( msg ) {}
};



/** \brief An IOException is used to signal an I/O error.
 *
 * If a file or directory cannot be opened, read, or written, this
 * exception is raised.
 */
class IOException : public Exception
{
public:
  IOException( std::string const& msg ) : Exception( msg ) {}
};


/** \brief An FCollException is used to signal a FileCollection problem.
 *
 * A process dealing with a collection of files will use this exception
 * if a problem arise while dealing with the collection.
 */
class FCollException : public Exception
{
public:
  FCollException( std::string const& msg ) : Exception( msg ) {}
};


/** \brief Exception used when it is not possible to move forward.
 *
 * An object member function may throw this exception when the
 * operation it normally performs is inappropriate or impossible to
 * perform because of the current state of the object.
 */
class InvalidStateException : public Exception
{
public:
  InvalidStateException( std::string const& msg ) : Exception( msg ) {}
};



} // namespace

// vim: ts=2 sw=2 et
