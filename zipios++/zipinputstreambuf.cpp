
#include "zipios++/zipios-config.h"

#include <algorithm>
#if defined (HAVE_STD_IOSTREAM) && defined (USE_STD_IOSTREAM)
#include <iostream>
#else
#include <iostream.h>
#endif

#include <zlib.h>

#include "zipios++/zipinputstreambuf.h"

namespace zipios {

using std::ios ;
using std::cerr ;
using std::endl ;
using std::min ;

ZipInputStreambuf::ZipInputStreambuf( streambuf *inbuf, int s_pos, bool del_inbuf ) 
  : InflateInputStreambuf( inbuf, s_pos, del_inbuf ),
    _open_entry( false                   ) 
{
  ConstEntryPointer entry = getNextEntry() ;
  
  if ( ! entry->isValid() ) {
    ; // FIXME: throw something?
  }
}

void ZipInputStreambuf::closeEntry() {
  if ( ! _open_entry )
    return ;
  
  // check if we're positioned correctly, otherwise position us correctly
  int position = _inbuf->pubseekoff(0, ios::cur, 
				    ios::in);
  if ( position != _data_start + _curr_entry.getCompressedSize() )
    _inbuf->pubseekoff(_data_start + _curr_entry.getCompressedSize(), 
		       ios::beg, ios::in) ;

}

void ZipInputStreambuf::close() {
}

ConstEntryPointer ZipInputStreambuf::getNextEntry() {
  if ( _open_entry )
    closeEntry() ;

  // read the zip local header
  istream is( _inbuf ) ; // istream does not destroy the streambuf.
  is >> _curr_entry ;
  if ( _curr_entry.isValid() ) {
    _data_start = _inbuf->pubseekoff(0, ios::cur, 
				     ios::in);
    if ( _curr_entry.getMethod() == DEFLATED ) {
      _open_entry = true ;
      reset() ; // reset inflatestream data structures 
//        cerr << "deflated" << endl ;
    } else if ( _curr_entry.getMethod() == STORED ) {
      _open_entry = true ;
      _remain = _curr_entry.getSize() ;
      // Force underflow on first read:
      setg( &( _outvec[ 0 ] ),
	    &( _outvec[ 0 ] ) + _outvecsize,
	    &( _outvec[ 0 ] ) + _outvecsize ) ;
//        cerr << "stored" << endl ;
    } else {
      _open_entry = false ; // Unsupported compression format.
      cerr << "Unsupported compression format" << endl ;
    }
  } else {
    _open_entry = false ;
  }

  if ( _curr_entry.isValid() && _curr_entry.trailingDataDescriptor() )
    ; // throw ZipException( "Trailing data descriptor in zip file not supported" ) ; 
  return new ZipLocalEntry( _curr_entry ) ;
}

ZipInputStreambuf::~ZipInputStreambuf() {
}


int ZipInputStreambuf::underflow() {
  if ( ! _open_entry )
    return EOF ; // traits_type::eof() 
  if ( _curr_entry.getMethod() == DEFLATED )
    return InflateInputStreambuf::underflow() ;

  // Ok, we're are stored, so we handle it ourselves.
  int num_b = min( _remain, _outvecsize ) ;
  int g = _inbuf->sgetn( &(_outvec[ 0 ] ) , num_b ) ;
  setg( &( _outvec[ 0 ] ),
	&( _outvec[ 0 ] ),
	&( _outvec[ 0 ] ) + g ) ;
  _remain -= g ;
  if ( g > 0 )
    return *gptr() ;
  else
    return EOF ; // traits_type::eof() 
}


// FIXME: We need to check somew
//  
//    // gp_bitfield bit 3 is one, if the length of the zip entry
//    // is stored in a trailer.
//    if ( is->good  && ( _curr_entry.gp_bitfield & 4 ) != 1 )
//      return true ;
//    else {
//      is->clear() ;
//      return false ;
//    }


} // namespace

/** \file
    Implementation of ZipInputStreambuf.
*/

/*
  Zipios++ - a small C++ library that provides easy access to .zip files.
  Copyright (C) 2000  Thomas S�ndergaard
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/