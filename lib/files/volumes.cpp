/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2022 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
/**
 ******************************************************************************
 * @file    volumes.cpp
 * @author  SO
 * @version v2.7.1
 * @date    04-January-2022
 * @brief   Interface for Volumes for file exchange.
 ******************************************************************************
 */

// === Includes ===
#include "volumes.h"

// Provide valid template instanciations
template class FAT32::Volume<SDHC::Card>;

// === Class Methods ===

/**
 * @brief Construct a new FAT32::Volume object
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param memory_used The object reference to the used memory.
 */
template<class Memory>
FAT32::Volume<Memory>::Volume( Memory& memory_used)
: memory{memory_used}
{

};

/**
 * @brief Read the first sector of a specific cluster from memory.
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param file The filehandler to use for data access.
 * @param cluster The cluster number to be read.
 * @return Returns True when the first sector of the specified cluster was read.
 */
template<class Memory>
bool FAT32::Volume<Memory>::read_cluster( Filehandler& file, const unsigned long cluster)
{
    // Set the current position in the filehandler
    file.current.sector = 1;
    file.current.cluster = cluster;

    // Read the correct block
    const unsigned long block = this->partition.get_LBA_of_cluster(cluster);

    // Convert the byte buffer to a 32-bit buffer for transfer with memory
    const unsigned long* buffer = reinterpret_cast<unsigned long*>(file.block_buffer.begin());

    // Read the new block from memory
    return Drive::read_single_block(this->memory, buffer, block);
};

/**
 * @brief Get the next cluster of a current cluster, reading the FAT.
 * The internal FAT buffer is used for data transfer.
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param cluster The number of the cluster to get the next sector.
 * @return The entry of in the FAT of the current cluster.
 */
template<class Memory>
std::optional<unsigned long> FAT32::Volume<Memory>::read_FAT_entry(const unsigned long cluster)
{
    // Read the FAT
    const unsigned long block = this->partition.get_FAT_sector(cluster, 1);

    // When sector is not already in buffer read it from memory
    if (block != this->FAT_sector_in_buffer)
    {
        // Convert the byte buffer to a 32-bit buffer for transfer with memory
        const unsigned long* buffer = reinterpret_cast<unsigned long*>(this->FAT.begin());

        // Read the memory
        if (not Drive::read_single_block(this->memory, buffer, block))
            return {};

        // Update the current sector in the buffer
        this->FAT_sector_in_buffer = block;
    }

    // Read the FAT entry and return it.
    const unsigned short position = this->partition.get_FAT_position(cluster);
    return this->partition.read_FAT(this->FAT.begin(), position);
};

/**
 * @brief Set the state of a cluster writing to the FAT.
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param cluster The number of the cluster to be update.
 * @param FAT_Entry The new state of the cluster.
 * @return Returns True when the FAT was properly updated.
 */
template<class Memory>
bool FAT32::Volume<Memory>::write_FAT_entry(
    const unsigned long cluster,
    const unsigned long FAT_Entry
)
{
    // get the LBA address of the block containing the FAT entry
    unsigned long lba_address = this->partition.get_FAT_sector(cluster, 1);

    // Convert the byte buffer to a 32-bit buffer for transfer with memory
    const unsigned long* buffer = reinterpret_cast<unsigned long*>(this->FAT.begin());

    // When sector is not already in buffer read it from memory
    if (lba_address != this->FAT_sector_in_buffer)
    {
        // Read the sector from memory
        if (not Drive::read_single_block(this->memory, buffer, lba_address))
            return false;

        // Update the current sector in the buffer
        this->FAT_sector_in_buffer = lba_address;
    }

    // Write the new entry in the buffer
    const unsigned short position = this->partition.get_FAT_position(cluster);
    this->partition.write_FAT(this->FAT.begin(), position, FAT_Entry);

    // Write the new data to FAT 1 in the memory
    if (not Drive::write_single_block(this->memory, buffer, lba_address))
        return false;

    // Write the new data to FAT 2 in the memory
    lba_address = this->partition.get_FAT_sector(cluster, 2);
    return Drive::write_single_block(this->memory, buffer, lba_address);
};

/**
 * @brief Read the next sector of a specific cluster from memory
 * until the end of the file is reached.
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param file The filehandler to use for data access.
 * @return Returns True when the next sector was found and could be read.
 */
template<class Memory>
bool FAT32::Volume<Memory>::read_next_sector_of_cluster(Filehandler& file)
{
    // Convert the byte buffer to a 32-bit buffer for transfer with memory
    const unsigned long* buffer = reinterpret_cast<unsigned long*>(file.block_buffer.begin());

    // Check whether next sector can be read directly
    if (file.current.sector != this->partition.Sectors_per_Cluster)
    {
        // Get the next bock address
        const unsigned long block = this->partition.get_LBA_of_cluster(file.current.cluster)
            + file.current.sector++;

        // Read the next block from memory
        return Drive::read_single_block(this->memory, buffer, block);
    }
    else // Next cluster has to be read
    {
        // Get the FAT entry of the current cluster
        const auto FAT_Entry = this->read_FAT_entry(file.current.cluster);

        // When FAT could not be read return false
        if( not FAT_Entry)
            return false;
        else //Check the return code for valid content
        {
            // Entry should not be zero
            if (FAT_Entry.value() == 0)
            {
                this->error = Error::Code::FAT_Corrupted;
                return false;
            }

            // Get the return code and pad FAT16 entries with 1, so the same
            // decoding of the entry can be used for both FATs
            const unsigned long code = this->partition.is_fat16 ? FAT_Entry.value() + 0xFFFF0000 : FAT_Entry.value();

            // Decode the entry
            if (code == FAT_Code::End_of_File)
            {
                this->error = Error::Code::End_of_File_Reached;
                return false;
            }
            else if (code == FAT_Code::Bad_Sector)
            {
                this->error = Error::Code::Bad_Sector;
                return false;
            }
        }

        // Read the next cluster according to the FAT and return
        return this->read_cluster(file, FAT_Entry.value());
    }
};

/**
 * @brief Write the current sector of a specific cluster to memory.
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param file The filehandler to use for data access.
 * @return Returns True when the sector of the specified cluster was written.
 */
template<class Memory>
bool FAT32::Volume<Memory>::write_current_sector(Filehandler& file)
{
    // Calculate the block address
    // -> The current sector is decremented by 1, because the current sector count starts at 1
    const unsigned long block = this->partition.get_LBA_of_cluster(file.current.cluster) +
        file.current.sector - 1;

    // Convert the byte buffer to a 32-bit buffer for data transfer
    const unsigned long* buffer = reinterpret_cast<unsigned long*>(file.block_buffer.begin());

    // Write the block to memory
    return Drive::write_single_block(this->memory, buffer, block);
};

/**
 * @brief Read the entry of the given directory and store the entry in
 * the given filehandle.
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param file The filehandler to use for data access.
 * @param id The ID of the entry to read from the directory.
 * @return Returns True when the ID was found and the entry read successfully.
 */
template<class Memory>
bool FAT32::Volume<Memory>::get_file(Filehandler& file, const unsigned long id)
{
    // Only when provided filehandle is a directory
    if ( file.is_file() )
    {
        this->error = Error::Code::Not_a_Directory;
        return false;
    } 

    // Set the ID of the files
    file.id = id;

    // Read the block from memory which contains the first file entry
    if (not this->read_cluster(file, file.start_cluster))
        return false;

    // Check in which sector the file id lies -> one sector contains 32 entries
    const unsigned char sector_count = id / (512/32);

    // Read the sectors
    for (unsigned char count = 0; count < sector_count; count++)
    {
        // Check whether next sector was found
        if( not this->read_next_sector_of_cluster(file) )
        {
            // Sector with file id is not allocated -> ID does not exist
            this->error = Error::Code::File_ID_not_found;
            return false;
        }
    }

    // When block was read, get the file properties
    file.update_properties_from_buffer();
    return true;
};

/**
 * @brief Read the root directory of the file system
 * and determine the name of the volume.
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param file The filehandler to use for data access.
 * @return Returns True when the root directory was read successfully.
 */
template<class Memory>
bool FAT32::Volume<Memory>::read_root(Filehandler& file)
{
    // Read the block which contains the root cluster
    const unsigned long block = this->partition.get_LBA_of_root_cluster();

    // Convert the byte buffer to a 32-bit buffer for transfer with memory
    const unsigned long* buffer = reinterpret_cast<unsigned long*>(file.block_buffer.begin());

    // Read the block from memory
    if (not Drive::read_single_block(this->memory, buffer, block))
        return false;

    // Set the current position in the filehandler
    file.id = 0;
    file.current.sector = 1;
    file.current.cluster = this->partition.is_fat16 ? 0 : this->partition.Root_Directory_Cluster;
    file.directory_cluster = this->partition.is_fat16 ? 0 : this->partition.Root_Directory_Cluster;
    file.start_cluster = this->partition.is_fat16 ? 0 : this->partition.Root_Directory_Cluster;

    // Current file buffer contains the root directory!
    file.update_properties_from_buffer();
    
    // Set the file also to containing a directory
    file.attributes |= FAT32::Attribute::is_Directory;

    return true;
};

/**
 * @brief Mount the volume using the assigned memory interface.
 * Reads the boot sector of the memory and derives the filesystem
 * from that.
 * 
 * When the filesystem is valid, the partition is initialized accordingly.
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @return Returns True when the memory contains a recognized filesystem.
 */
template<class Memory>
bool FAT32::Volume<Memory>::mount(void)
{
    // Use the FAT buffer to read mount data
    const unsigned long* buffer = reinterpret_cast<unsigned long*>(this->FAT.begin());

    // Read first block
    if (not Drive::read_single_block(this->memory, buffer, 0))
        return false;

    // check type
    if (not FAT32::boot_sector_valid(this->FAT.begin()) )
    {
        // boot section not recognized
        this->error = Error::Code::File_System_Invalid;
        return false;
    }

    // Check type if partition table
    if (not FAT32::boot_is_MBR(this->FAT.begin()) )
    {
        // boot section not recognized
        this->error = Error::Code::File_System_Invalid;
        return false;
    }

    // Boot section is valid and recognized
    const unsigned long block_address = FAT32::get_partition_begin(this->FAT.begin());
    if (not Drive::read_single_block(this->memory, buffer, block_address))
        return false;
    this->partition = BPB::initialize_partition(this->FAT.begin(), block_address);
    return true;
};

/**
 * @brief Get the next empty fileid which fits in the current directory.
 * The directory has to be read from memory.
 * 
 * The directory clusters are not extended when directory is full!
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param directory Filehandler to use for memory access.
 * @return Returns the fileid of the next empty entry, or False when no id was found.
 */
template<class Memory>
std::optional<unsigned long> FAT32::Volume<Memory>::get_empty_id(Filehandler& directory)
{
    // Check whether directory was provided
    if ( directory.is_file() )
        return {};

    // The next empty file id
    unsigned long empty_id = 0;
    
    // Read the current sector and see whether there are empty entries
    for (unsigned char entry_count = 0; entry_count<16; entry_count++)
    {
        // Current entry empty?
        if ((directory.block_buffer[32*entry_count] == 0x00) || (directory.block_buffer[32*entry_count] == 0xE5))
            return static_cast<unsigned long>(entry_count);
    }

    // The current sector does not contain an empty id, so keep reading next sectors
    while( this->read_next_sector_of_cluster(directory) )
    {
        // One sector was read, so the possible id is incremented by 16 entries
        empty_id += 16;

        // Read the current sector and see whether there are empty entries
        for (unsigned char entry_count = 0; entry_count<16; entry_count++)
        {
            // Current entry empty?
            if ((directory.block_buffer[32*entry_count] == 0x00) || (directory.block_buffer[32*entry_count] == 0xE5))
                return empty_id + entry_count;
        }
    }

    // No empty id is found
    return {};
};

/**
 * @brief Read the FAT and get the next cluster which
 * is empty and can be allocated.
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @return Returns the number of the next empty cluster, False when no cluster is empty.
 */
template<class Memory>
std::optional<unsigned long> FAT32::Volume<Memory>::get_next_empty_cluster(void)
{
    // Limit the number of clusters according to the filesystem
    const unsigned long max_cluster = this->partition.is_fat16 ? this->partition.FAT_Size * 512/16 : this->partition.FAT_Size * 512/32;

    // Check FAT until cluster is empty, valid clusters start with number 2
    for (unsigned long empty_cluster = 2; empty_cluster<= max_cluster; empty_cluster++)
    {
        // Check whether current cluster is empty
        if ( this->read_FAT_entry(empty_cluster).value_or(1) == 0U )
            return empty_cluster;
    }

    // No empty cluster found -> drive is full
    this->error = Error::Code::No_Memory_Left;
    return {};
};

/**
 * @brief Read the current directory and get the fileid
 * of the file/directory specified by its name.
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param directory Filehandler of directory to use for memory access.
 * @param filename The name of the file to search for.
 * @return Returns the fileid of the file when it was found, returns False when
 * file was not found.
 */
template<class Memory>
std::optional<unsigned long> FAT32::Volume<Memory>::get_fileid(
    Filehandler& directory,
    const std::array<char, 12> filename
    )
{
    // Check whether directory was provided
    if ( directory.is_file() )
    {
        this->error = Error::Code::Not_a_Directory;
        return {};
    }

    // The file id
    unsigned long file_id = 0;

    // Check current sector for the name
    for (unsigned char entry = 0; entry < (512/32); entry++)
    {
        if (std::equal(
            filename.begin(),
            filename.end() - 1,
            directory.block_buffer.begin() + entry*32
        ))
        {
            return static_cast<unsigned long>(entry);
        }
    }

    // The current sector does not contain the fileid, so keep reading next sectors
    while( this->read_next_sector_of_cluster(directory) )
    {
        // One sector was read, so the possible id is incremented by 16 entries
        file_id += 16;

        // Read the current sector and see whether the file is found
        for (unsigned char entry = 0; entry < (512/32); entry++)
        {
            if (std::equal(
                filename.begin(),
                filename.end() - 1,
                directory.block_buffer.begin() + entry*32
            ))
            {
                return file_id + entry;
            }
        }
    }

    // File is not found
    this->error = Error::Code::File_ID_not_found;
    return {};
};

/**
 * @brief Read the last sector of an opened file.
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param file The filehandle to the opened file to be used for memory access.
 * @return Returns True when the end of file was found and the data was read.
 */
template<class Memory>
bool FAT32::Volume<Memory>::read_last_sector_of_file(Filehandler& file)
{
    // Get the number of clusters the file occupies
    const unsigned long cluster_allocated = (file.size / 512) / this->partition.Sectors_per_Cluster;
    
    // Read the next clusters until the cluster where the file ends was found
    unsigned long cluster_offset = file.start_cluster;
    for( unsigned long count = 0; count < cluster_allocated; count++)
    {
        // Read the FAT to determine the next cluster
        auto next_cluster = this->read_FAT_entry(cluster_offset);

        // Check whether entry is valid
        if( next_cluster )
            cluster_offset = next_cluster.value();
        else
            return false;
    }

    // Get the sector address of the cluster where the file ends
    const unsigned long cluster_lba = this->partition.get_LBA_of_cluster(cluster_offset);

    // Get the sector offset according to the size of the file
    const unsigned long sector_offset = (file.size / 512) % this->partition.Sectors_per_Cluster;

    // Convert the byte buffer to a 32-bit buffer for transfer with memory
    const unsigned long* buffer = reinterpret_cast<unsigned long*>(file.block_buffer.begin());

    // Read the new block from memory
    if(not Drive::read_single_block(this->memory, buffer, cluster_lba + sector_offset))
        return false;

    // End of file was found, update file properties
    file.current.cluster = cluster_offset;
    file.current.sector = sector_offset + 1;
    file.current.byte = static_cast<unsigned short>(
        file.size - ((cluster_allocated*this->partition.Sectors_per_Cluster + sector_offset) * 512)
    );
    return true;
};

/**
 * @brief Update the filesize of a file in the directory
 * entry of the file.
 * 
 * The file buffer has to be written to the memory!
 * The file buffer is used for the directory access, so any
 * data which is not written to the memory is lost.
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param file The filehandle of the file of which the directory entry should be updated.
 * @return Returns True when the directory entry was updated successfully.
 */
template<class Memory>
bool FAT32::Volume<Memory>::write_filesize_to_directory(Filehandler& file)
{
    // Get the offset of the file id when sector containing it is read
    // 16 entries per sector, one sectors has 32 bytes
    const unsigned short entry_offset = (file.id % 16) * 32;

    // Read the directory cluster of the file
    if (not this->read_cluster(file, file.directory_cluster))
        return false;

    // Read additional sectors if necessary
    const unsigned long sector_offset = file.id / 16; 
    for (unsigned long count=0; count<sector_offset; count++)
    {
        // Keep reading sectors
        if(not this->read_next_sector_of_cluster(file))
            return false;
    }

    // Update the entry
    write_long(file.block_buffer.begin(), entry_offset + FAT32::DIR_Entry::Filesize, file.size);

    // Write entry to memory again
    return this->write_current_sector(file);
};

/**
 * @brief Write the current file buffer to memory.
 * When the current sector of cluster is full after
 * the write, new memory is allocated.
 * 
 * The filesize is not updated in the directory! This has
 * to be done after writing the content!
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param file The filehandle to the file which will be written to memory.
 * @return Returns True when the content was written successfully.
 */
template<class Memory>
bool FAT32::Volume<Memory>::write_file_to_memory(Filehandler& file)
{
    // Get the current location of the file in memory
    const unsigned long cluster = file.current.cluster;
    const unsigned long sector = file.current.sector;

    // Write the file buffer to the memory location
    if(not this->write_current_sector(file))
        return false;

    // When the current sector is full, update the filehandle
    if (file.current.byte == 512)
    {
        // Reset the byte counter
        file.current.byte = 0;

        // Check whether cluster is full
        if (sector < this->partition.Sectors_per_Cluster)
            file.current.sector++;
        else // When current cluster is full, allocate the next empty cluster 
        {
            // Get next empty cluster
            auto next_cluster = this->get_next_empty_cluster();

            // When next cluster was found, allocate it
            if ( not next_cluster)
                return false;

            // Set current cluster
            if(not this->write_FAT_entry(cluster, next_cluster.value()))
                return false;

            // Set next cluster to end-of-file
            const unsigned long entry = this->partition.is_fat16 ? 0xFFFF : 0xFFFFFFFF;
            if(not this->write_FAT_entry(next_cluster.value(), entry))
                return false;

            // Update the filehandle
            file.current.cluster = next_cluster.value();
            file.current.sector = 1;
        }
    }
    return true;
};

/**
 * @brief Create a entry in the provided open directory.
 * The loaded sector has to contain the ID of the file! 
 * 
 * @tparam Memory The memory class to interface with the memory.
 * @param directory Filehandler with loaded directory sector which contains the entry ID.
 * @param id The file id to make the entry for.
 * @param start_cluster The start cluster of the new file.
 * @param name The name of the file. ALL UPPER CASE!
 * @param attributes The file attributes. For File ARCHIVE is important.
 * @param time The current time of file creation.
 * @return Returns True when the entry was written to memory.
 */
template<class Memory>
bool FAT32::Volume<Memory>::make_directory_entry(
    Filehandler& directory,
    const unsigned long id,
    const unsigned long start_cluster,
    const std::array<char, 12> name,
    const unsigned char attributes,
    const std::time_t time
)
{
    // Get the byte offset within the sector
    const unsigned int offset = (id % 16) * 32;
        
    // Write the Name to the entry
    std::copy(
        name.begin(),
        name.end(),
        directory.block_buffer.begin() + offset
    );

    // Set the start cluster of the entry
    write_short(directory.block_buffer.begin(), DIR_Entry::First_Cluster_L + offset, start_cluster & 0xFFFF);
    if (this->partition.is_fat16)
        write_short(directory.block_buffer.begin(), DIR_Entry::First_Cluster_H + offset, (start_cluster>>16) & 0U);
    else
        write_short(directory.block_buffer.begin(), DIR_Entry::First_Cluster_H + offset, (start_cluster>>16) & 0xFFFF);

    // Set the attributes
    write_byte(directory.block_buffer.begin(), DIR_Entry::Attributes + offset, attributes);

    // Convert the seconds since epoch to FAT format
    std::tm* utc = std::localtime(&time);
    const unsigned short time_entry =
        (utc->tm_hour << FAT32::Time_Pos::Hours) 
        | (utc->tm_min << FAT32::Time_Pos::Minutes) 
        | (utc->tm_sec / 2);
    const unsigned short date_entry =
        ((utc->tm_year-80) << FAT32::Date_Pos::Year) 
        | ((utc->tm_mon+1) << FAT32::Date_Pos::Month) 
        | (utc->tm_mday);

    // Write time
    write_short(directory.block_buffer.begin(), offset + DIR_Entry::Creation_Time, time_entry);
    write_short(directory.block_buffer.begin(), offset + DIR_Entry::Write_Time, time_entry);

    // Write date
    write_short(directory.block_buffer.begin(), offset + DIR_Entry::Creation_Date, date_entry);
    write_short(directory.block_buffer.begin(), offset + DIR_Entry::Access_Date, date_entry);
    write_short(directory.block_buffer.begin(), offset + DIR_Entry::Write_Date, date_entry);
    
    //Write file size, 0 for empty file. When writing to file, the file size has to be updated!
    // Otherwise OEMs will assume the file is still empty.
    write_long(directory.block_buffer.begin(), offset + DIR_Entry::Filesize, 0U);

    // After updating the entry, write the sector of the directory to memory
    return this->write_current_sector(directory);
};