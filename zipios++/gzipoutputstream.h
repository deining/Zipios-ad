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
    Header file that defines GZIPOutputStream.
*/

#include "zipios++/gzipoutputstreambuf.h"


namespace zipios
{

/** \anchor GZIPOutputStream_anchor
 * GZIPOutputStream is an ostream that writes the output to a gz file. The
 * interface approximates the interface of the Java GZIPOutputStream.
 */
class GZIPOutputStream : public std::ostream
{
public:

  /** GZIPOutputStream constructor.
      @param os ostream to which the compressed zip archive is written.
    */
  explicit GZIPOutputStream( std::ostream &os ) ;

  /** GZIPOutputStream constructor.
      @filename filename to write the gzip archive to. */
  explicit GZIPOutputStream( std::string const& filename ) ;

  void setFilename( std::string const& filename ) ;
  void setComment( std::string const& comment ) ;
  
  /** Calls finish and closes the stream. */
  void close() ;

  /** Finishes the stream. */
  void finish() ;

  /** Destructor. */
  virtual ~GZIPOutputStream() ;

private:
  std::ofstream *ofs ;
  GZIPOutputStreambuf *ozf ;
};
 
} // zipios namespace.

// vim: ts=2 sw=2 et
