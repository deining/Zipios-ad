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
    Header file that defines ZipOutputStreambuf.
*/

#include "zipios++/deflateoutputstreambuf.h"


namespace zipios
{

/** GZIPOutputStreambuf is a zip output streambuf filter.
 */
class GZIPOutputStreambuf : public DeflateOutputStreambuf
{
public:
    explicit      GZIPOutputStreambuf(std::streambuf *outbuf);
    virtual       ~GZIPOutputStreambuf();

    void          setFilename(std::string const& filename);
    void          setComment(std::string const& comment);
    void          close();
    void          finish();

protected:
    virtual int   overflow(int c = EOF);
    virtual int   sync();

private:
    void          writeHeader();
    void          writeTrailer();
    void          writeInt(uint32_t i);

    std::string   m_filename;
    std::string   m_comment;
    bool          m_open = false;
};


} // zipios namespace

// vim: ts=4 sw=4 et
