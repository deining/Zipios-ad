
#include "zipios++/zipios-config.h"

#include <algorithm>
#include "zipios++/meta-iostreams.h"

#include <zlib.h>

#include "zipios++/zipoutputstreambuf.h"

namespace zipios {

using std::ios ;
using std::cerr ;
using std::endl ;
using std::min ;

ZipOutputStreambuf::ZipOutputStreambuf( streambuf *outbuf, bool del_outbuf ) 
  : DeflateOutputStreambuf( outbuf, false, del_outbuf ),
    _open_entry( false    ),
    _open      ( true     ),
    _method    ( DEFLATED ),
    _level     ( 6        )
{
}


void ZipOutputStreambuf::closeEntry() {
  if ( ! _open_entry )
    return ;

  closeStream() ;

  updateEntryHeaderInfo() ;
  setEntryClosedState( ) ;
}


void ZipOutputStreambuf::close() {
  finish() ;
  // FIXME: should close _outbuf here too.
}


void ZipOutputStreambuf::finish() {
  closeEntry() ;
  ostream os( _outbuf ) ;
  writeCentralDirectory( _entries, _zip_comment, os ) ;
  _open = false ;
}


ZipOutputStreambuf::~ZipOutputStreambuf() {
  finish() ;
}


void ZipOutputStreambuf::putNextEntry( const ZipCDirEntry &entry ) {
  if ( _open_entry )
    closeEntry() ;

  if ( _method == DEFLATED ) {
    if ( ! init( _level ) )
      cerr << "ZipOutputStreambuf::putNextEntry(): init() failed!\n" ;
  }

  _entries.push_back( entry ) ;
  ZipCDirEntry &ent = _entries.back() ;

  ostream os( _outbuf ) ;

  // Update entry header info
  ent.setLocalHeaderOffset( os.tellp() ) ;
  ent.setMethod( _method ) ;
  
  os << static_cast< ZipLocalEntry >( ent ) ;

  _open_entry = true ;
}


void ZipOutputStreambuf::setComment( const string &comment ) {
  _zip_comment = comment ;
}


void ZipOutputStreambuf::setLevel( int level ) {
  _level = level ;
}


void ZipOutputStreambuf::setMethod( StorageMethod method ) {
  _method = method ;

}

//
// Protected and private methods
//

int ZipOutputStreambuf::overflow( int c ) {
  return DeflateOutputStreambuf::overflow( c ) ;
//    // FIXME: implement
  
//    cout << "ZipOutputStreambuf::overflow() not implemented yet!\n" ;
//    return EOF ;
}



int ZipOutputStreambuf::sync() {
  return DeflateOutputStreambuf::sync() ;
//    // FIXME: implement
//    cout << "ZipOutputStreambuf::sync() not implemented yet!\n" ;
//    return EOF ;
}



void ZipOutputStreambuf::setEntryClosedState() {
  _open_entry = false ;
  // FIXME: update put pointers to trigger overflow on write. overflow
  // should then return EOF while _open_entry is false.
}


void ZipOutputStreambuf::updateEntryHeaderInfo() {
  if ( ! _open_entry )
    return ;

  sync() ; // Make sure remaining data has been written ;

  ostream os( _outbuf ) ;
  int curr_pos = os.tellp() ;
  
  // update fields in _entries.back()
  ZipCDirEntry &entry = _entries.back() ;
  entry.setSize( /*FIXME!!*/ 0 ) ;
  entry.setCompressedSize( curr_pos - entry.getLocalHeaderOffset() 
			   - entry.getLocalHeaderSize() ) ;

  // write ZipLocalEntry header to header position
  os.seekp( entry.getLocalHeaderOffset() ) ;
  os << static_cast< ZipLocalEntry >( entry ) ;
  os.seekp( curr_pos ) ;
}


void ZipOutputStreambuf::writeCentralDirectory( const vector< ZipCDirEntry > &entries, 
						EndOfCentralDirectory eocd, 
						ostream &os ) {
  int cdir_start = os.tellp() ;
  vector< ZipCDirEntry >::const_iterator it ;
  int cdir_size = 0 ;

  for ( it = entries.begin() ; it != entries.end() ; ++it ) {
    os << *it ;
    cdir_size += it->getCDirHeaderSize() ;
  }
  eocd.setOffset( cdir_start ) ;
  eocd.setCDirSize( cdir_size ) ;
  eocd.setTotalCount( entries.size() ) ;
  os << eocd ;
}

} // namespace

/** \file
    Implementation of ZipOutputStreambuf.
*/

/*
  Zipios++ - a small C++ library that provides easy access to .zip files.
  Copyright (C) 2000  Thomas S�ndergaard
  
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