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
 * \brief Implementation of zipios::ZipCentralDirectoryEntry.
 *
 * This file includes the implementation of the zipios::ZipCentralDirectoryEntry
 * which is a zipios::FileEntry used when reading the central
 * directory of a Zip archive.
 */

#include "zipcentraldirectoryentry.hpp"

#include "zipios++/zipiosexceptions.hpp"

#include "dostime.h"
#include "zipios_common.hpp"


namespace zipios
{



namespace
{


/** \brief The signature of a ZipCentralDirectoryEntry.
 *
 * This value represents the signature of a Zip Central Directory Entry.
 *
 * The signature represents:
 *
 * \code
 * "PK 1.2"
 * \endcode
 */
uint32_t const  g_signature = 0x02014b50;


// The zip codes (values are pre-shifted)
uint16_t const   g_msdos         = 0x0000;
uint16_t const   g_amiga         = 0x0100;
uint16_t const   g_open_vms      = 0x0200;
uint16_t const   g_unix          = 0x0300;
uint16_t const   g_vm_cms        = 0x0400;
uint16_t const   g_atari_st      = 0x0500;
uint16_t const   g_os2_hpfs      = 0x0600;
uint16_t const   g_macintosh     = 0x0700;
uint16_t const   g_z_system      = 0x0800;
uint16_t const   g_cpm           = 0x0900;
uint16_t const   g_windows       = 0x0A00;
uint16_t const   g_mvs           = 0x0B00;
uint16_t const   g_vse           = 0x0C00;
uint16_t const   g_acorn_risc    = 0x0D00;
uint16_t const   g_vfat          = 0x0E00;
uint16_t const   g_alternate_vms = 0x0F00;
uint16_t const   g_beos          = 0x1000;
uint16_t const   g_tandem        = 0x1100;
uint16_t const   g_os400         = 0x1200;
uint16_t const   g_osx           = 0x1300;


/** \brief The header of a ZipCentralDirectoryEntry in a Zip archive.
 *
 * This structure shows how the header of the ZipCentralDirectoryEntry is defined.
 * Note that the file name, file comment, and extra field have a
 * variable size which is defined in three 16 bit values before
 * they appear.
 *
 * The filename cannot be empty, however, the file comment and the
 * extra field can (and usually are.)
 *
 * \note
 * This structure is NOT used directly only for its sizeof() and
 * documentation because that way zipios can work on little and big
 * endians without the need to know the endianess of your computer.
 */
struct ZipCentralDirectoryEntryHeader
{
    uint32_t        m_signature;
    uint16_t        m_writer_version;
    uint16_t        m_extract_version;
    uint16_t        m_gp_bitfield;
    uint16_t        m_compress_method;
    uint32_t        m_dostime;
    uint32_t        m_crc_32;
    uint32_t        m_compressed_size;
    uint32_t        m_uncompressed_size;
    uint16_t        m_filename_len;
    uint16_t        m_extra_field_len;
    uint16_t        m_file_comment_len;
    uint16_t        m_disk_num_start;
    uint16_t        m_intern_file_attr;
    uint32_t        m_extern_file_attr;
    uint32_t        m_rel_offset_loc_head;
    //uint8_t       m_filename[m_filename_len];
    //uint8_t       m_extra_field[m_extra_field_len];
    //uint8_t       m_file_comment[m_file_comment_len];
};


} // no name namespace


/** \class ZipCentralDirectoryEntry
 * \brief A specialization of ZipLocalEntry for
 *
 * Specialization of ZipLocalEntry, that add fields for storing the
 * extra information, that is only present in the entries in the zip
 * central directory and not in the local entry headers.
 */


/** \brief Initialize a ZipCentralDirectoryEntry.
 *
 * This function initializes a FileEntry specific to a Central Directory
 * of a Zip archive file.
 *
 * This definition includes a comment which the ZipLocalEntry lacks.
 *
 * \param[in] filename  The name of the file representing this entry.
 * \param[in] file_comment  Comment specific to this file entry.
 * \param[in] extra_field  The extra buffer(s) attached to this entry.
 */
ZipCentralDirectoryEntry::ZipCentralDirectoryEntry(std::string const& filename, std::string const& file_comment, buffer_t const& extra_field)
    : ZipLocalEntry(filename, extra_field)
    , m_file_comment(file_comment)
{
}


/** \brief Clean up the entry.
 *
 * The destructor makes sure the entry is fully cleaned up.
 */
ZipCentralDirectoryEntry::~ZipCentralDirectoryEntry()
{
}


/** \brief Get the file comment.
 *
 * This function returns a copy of the file comment.
 *
 * \return The file comment of the entry, may be an empty string.
 */
std::string ZipCentralDirectoryEntry::getComment() const
{
    return m_file_comment;
}


/** \brief Set the file comment.
 *
 * This function is used to set the file comment of the entry as read
 * from a Zip archive.
 *
 * \param[in] comment  The comment to set the entry as.
 */
void ZipCentralDirectoryEntry::setComment(std::string const& comment)
{
    m_file_comment = comment;
}


/** \brief Convert the entry into a string.
 *
 * This function is used o convert the entry into a printable string
 * (assuming the filename is printable.)
 *
 * \todo
 * We may want to test whether the entry is a directory and if so
 * not display any sizes, just says "directory" instead.
 *
 * \return The filename and sizes of the entry.
 */
std::string ZipCentralDirectoryEntry::toString() const
{
    OutputStringStream sout;
    sout << m_filename << " (" << m_uncompressed_size << " bytes, ";
    sout << m_compressed_size << " bytes compressed)";
    return sout.str();
}


/** \brief Compute and return the current header size.
 *
 * This function computes the size that this entry will take in the
 * Central Directory of the Zip archive.
 *
 * \return The total size of the Central Directory entry on disk.
 */
size_t ZipCentralDirectoryEntry::getHeaderSize() const
{
    /** \TODO
     * Add support for 64 bit Zip. At this time this function returns
     * an invalid size if the filename, extra field, or file comment
     * sizes are more than allowed in an older version of the Zip format.
     */
    return sizeof(ZipCentralDirectoryEntryHeader) + m_filename.size() + m_extra_field.size() + m_file_comment.size() ;
}


/** \brief Create a clone of this Central Directory entry.
 *
 * This function allocates a new copy of this ZipCentralDirectoryEntry
 * object and returns a smart pointer to it.
 *
 * \return A smart pointer to the copy.
 */
FileEntry::pointer_t ZipCentralDirectoryEntry::clone() const
{
    return FileEntry::pointer_t(new ZipCentralDirectoryEntry(*this));
}


/** \brief Read a Central Directory entry.
 *
 * This function reads one Central Directory entry from the specified
 * input stream. If anything goes wrong with the input stream, the read
 * function will throw an error.
 *
 * \note
 * While reading the entry is marked as invalid. If the read fails, the
 * entry will remain invalid. On success, the function restores the status
 * back to valid.
 *
 * \note
 * If the signature or some other parameter is found to be invalid, then
 * the input stream is marked as failed and an exception is thrown.
 *
 * \exception IOException
 * This exception is thrown if the signature read does not match the
 * signature of a Central Directory entry. This can only mean a bug
 * in a Zip writer or an invalid/corrupt file altogether.
 *
 * \param[in] is  The input stream to read from.
 *
 * \sa write()
 */
void ZipCentralDirectoryEntry::read(std::istream& is)
{
    m_valid = false; // set back to true upon successful completion below.

    // verify the signature
    uint32_t signature;
    zipRead(is, signature);
    if(g_signature != signature)
    {
        is.setstate(std::ios::failbit);
        throw IOException("ZipCentralDirectoryEntry::read(): Expected Central Directory entry signature not found");
    }

    uint16_t writer_version(0);
    uint16_t compress_method(0);
    uint32_t dostime(0);
    uint32_t compressed_size(0);
    uint32_t uncompressed_size(0);
    uint32_t rel_offset_loc_head(0);
    uint16_t filename_len(0);
    uint16_t extra_field_len(0);
    uint16_t file_comment_len(0);
    uint16_t intern_file_attr(0);
    uint32_t extern_file_attr(0);
    uint16_t disk_num_start(0);
    std::string filename;

    // read the header
    zipRead(is, writer_version);                    // 16
    zipRead(is, m_extract_version);                 // 16
    zipRead(is, m_gp_bitfield);                     // 16
    zipRead(is, compress_method);                   // 16
    zipRead(is, dostime);                           // 32
    zipRead(is, m_crc_32);                          // 32
    zipRead(is, compressed_size);                   // 32
    zipRead(is, uncompressed_size);                 // 32
    zipRead(is, filename_len);                      // 16
    zipRead(is, extra_field_len);                   // 16
    zipRead(is, file_comment_len);                  // 16
    zipRead(is, disk_num_start);                    // 16
    zipRead(is, intern_file_attr);                  // 16
    zipRead(is, extern_file_attr);                  // 32
    zipRead(is, rel_offset_loc_head);               // 32
    zipRead(is, filename, filename_len);            // string
    zipRead(is, m_extra_field, extra_field_len);    // buffer
    zipRead(is, m_file_comment, file_comment_len);  // string
    /** \TODO check whether this was a 64 bit header and make sure
     *        to read the 64 bit header too if so
     */

    m_compress_method = static_cast<StorageMethod>(compress_method);
    m_unix_time = dos2unixtime(dostime);
    m_compressed_size = compressed_size;
    m_uncompressed_size = uncompressed_size;
    m_entry_offset = rel_offset_loc_head;
    m_filename = FilePath(filename);

    // the zipRead() should throw if it is false...
    if(is)
    {
        m_valid = true;
    }
}


/** \brief Write a Central Directory Entry to the output stream.
 *
 * This function verifies that the data of the Central Directory entry
 * can be written to disk. If so, then it writes a block. The size of
 * the blocks varies depending on the filename, file comment, and extra
 * data. The current size can be determined using the getHeaderSize()
 * function.
 *
 * \exception InvalidStateException
 * The function verifies whether the filename, extra field,
 * file comment, file data, or data offset are not too large.
 * If any one of these parameters is too large, then this
 * exception is raised.
 *
 * \param[in] os  The output stream where the data is written.
 *
 * \sa getHeaderSize()
 * \sa read()
 */
void ZipCentralDirectoryEntry::write(std::ostream& os)
{
    /** \TODO add support for 64 bit entries
     *        (zip64 is available, just need to add a 64 bit header...)
     */
    if(m_filename.length()     > 0x10000
    || m_extra_field.size()    > 0x10000
    || m_file_comment.length() > 0x10000)
    {
        throw InvalidStateException("ZipLocalEntry::write(): file name or extra field too large to save in a Zip file.");
    }

// Solaris defines _ILP32 for 32 bit platforms
#if !defined(_ILP32)
    if(m_compressed_size   >= 0x100000000ULL
    || m_uncompressed_size >= 0x100000000ULL
    || m_entry_offset      >= 0x100000000LL)
    {
        throw InvalidStateException("The size of this file is too large to fit in a zip archive.");
    }
#endif

    // define version
    uint16_t writer_version = g_zip_format_version;
    // including the "compatibility" code
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    // MS-Windows
    /** \TODO should we use g_msdos instead?
     */
    writer_version |= g_windows;
#elif defined(__APPLE__) && defined(__MACH__)
    // OS/X
    writer_version |= g_osx;
#else
    // Other Unices
    writer_version |= g_unix;
#endif

    uint16_t compress_method(static_cast<uint8_t>(m_compress_method));
    uint32_t dostime(unix2dostime(m_unix_time));
    uint32_t compressed_size(m_compressed_size);
    uint32_t uncompressed_size(m_uncompressed_size);
    uint16_t filename_len(m_filename.length());
    uint16_t extra_field_len(m_extra_field.size());
    uint16_t file_comment_len(m_file_comment.length());
    uint16_t disk_num_start(0);
    uint16_t intern_file_attr(0);
    /** \FIXME
     * I do not understand the external mapping, simply
     * copied value for a file with -rw-rw-r-- permissions
     * compressed with info-zip
     */
    uint32_t extern_file_attr(0x81B40000);
    uint32_t rel_offset_loc_head(m_entry_offset);

    zipWrite(os, g_signature           );       // 32
    zipWrite(os, writer_version        );       // 16
    zipWrite(os, m_extract_version     );       // 16
    zipWrite(os, m_gp_bitfield         );       // 16
    zipWrite(os, compress_method       );       // 16
    zipWrite(os, dostime               );       // 32
    zipWrite(os, m_crc_32              );       // 32
    zipWrite(os, compressed_size       );       // 32
    zipWrite(os, uncompressed_size     );       // 32
    zipWrite(os, filename_len          );       // 16
    zipWrite(os, extra_field_len       );       // 16
    zipWrite(os, file_comment_len      );       // 16
    zipWrite(os, disk_num_start        );       // 16
    zipWrite(os, intern_file_attr      );       // 16
    zipWrite(os, extern_file_attr      );       // 32
    zipWrite(os, rel_offset_loc_head   );       // 32
    zipWrite(os, m_filename            );       // string
    zipWrite(os, m_extra_field         );       // buffer
    zipWrite(os, m_file_comment        );       // string
}


} // zipios namespace
// vim: ts=4 sw=4 et