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
 * \brief Define the zipios::ZipInputStreambuf class.
 *
 * This file defines the zipios::ZipInputStreambuf class which is
 * used to read the data of files found in a Zip archive.
 */

#include "inflateinputstreambuf.hpp"

#include "ziplocalentry.hpp"


namespace zipios
{


/** ZipInputStreambuf is a zip input streambuf filter.
 */
class ZipInputStreambuf : public InflateInputStreambuf
{
public:
                            ZipInputStreambuf(std::streambuf *inbuf, offset_t start_pos = -1);
                            ZipInputStreambuf(ZipInputStreambuf const& src) = delete;
    ZipInputStreambuf&      operator = (ZipInputStreambuf const& src) = delete;
    virtual                 ~ZipInputStreambuf() override;

    void                    closeEntry();
    void                    close();
    FileEntry::pointer_t    getNextEntry();

protected:
    virtual std::streambuf::int_type             underflow() override;

private:
    bool                    m_open_entry = false;
    ZipLocalEntry           m_curr_entry;
    offset_t                m_data_start = 0; // Do not forget entry header has a length too.
    offset_t                m_remain = 0;     // For STORED entry only. the number of bytes that
                                              // has not been put in the m_outvec yet.
};


} // namespace
// vim: ts=4 sw=4 et