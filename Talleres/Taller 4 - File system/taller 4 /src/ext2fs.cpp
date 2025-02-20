#include "ext2fs.h"
#include "hdd.h"
#include "pentry.h"
#include "mbr.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>

Ext2FS::Ext2FS(HDD & disk, unsigned char pnumber) : _hdd(disk), _partition_number(pnumber)
{
	assert(pnumber <= 3);

	// Load Superblock
	unsigned char * buffer = new unsigned char[2*SECTOR_SIZE];

	// Get partition info
	const PartitionEntry & pentry = _hdd[_partition_number];

	// Skip first two sectors & read 3rd and 4th sectors
	// Warning: We assume common sector size of 512bytes but it may not be true
	_hdd.read(pentry.start_lba()+2, buffer);
	_hdd.read(pentry.start_lba()+3, buffer+SECTOR_SIZE);

	// Warning: this only works in Little Endian arhitectures
	_superblock = (struct Ext2FSSuperblock *) buffer;

	// Load Block Group Table

	// Compute block size by shifting the value 1024
	unsigned int block_size = 1024 << _superblock->log_block_size;
	unsigned int sectors_per_block = block_size / SECTOR_SIZE;

	// Compute amount of block groups in FS
	_block_groups = _superblock->blocks_count / _superblock->blocks_per_group;

	if((_superblock->blocks_count % _superblock->blocks_per_group) != 0)
	{
		_block_groups++;
	}
	std::cerr << "block_groups: " << _block_groups << std::endl;

	// Allocate enough memory to keep the table
	_bgd_table = new Ext2FSBlockGroupDescriptor[_block_groups];

	// Since the size of the table is not forced to be a factor of SECTOR_SIZE
	// we need to compute the amount of sectors we should read and then copy
	// one by one the descriptors into the table in the apropriate order.
	unsigned int size_of_bgd_table = sizeof(Ext2FSBlockGroupDescriptor) * _block_groups;
	unsigned int bgd_table_sectors = size_of_bgd_table / SECTOR_SIZE;
	if((size_of_bgd_table % SECTOR_SIZE) != 0)
	{
		bgd_table_sectors++;
	}
	std::cerr << "bgd_table_sectors: " << bgd_table_sectors << std::endl;

	buffer = new unsigned char[bgd_table_sectors * SECTOR_SIZE];
	for(unsigned int i = 0; i < bgd_table_sectors; i++)
	{
		std::cerr << "pentry.start_lba()+4+i: " << (unsigned int) (pentry.start_lba()+4+i) << " buffer+(SECTOR_SIZE*i): " << (unsigned char *) (buffer+(SECTOR_SIZE*i)) << std::endl;
		_hdd.read(pentry.start_lba()+4+i, buffer+(SECTOR_SIZE*i));
	}

	// Copy descriptors into table
	for(unsigned int i = 0; i < _block_groups; i++)
	{
		_bgd_table[i] = *((Ext2FSBlockGroupDescriptor *) (buffer+(i*sizeof(Ext2FSBlockGroupDescriptor))));
	}
	delete[] buffer;
}

unsigned int Ext2FS::blockgroup_for_inode(unsigned int inode)
{
	// en qué Block Group se encuentra el inodo ?
	assert(inode > 0);
	return (inode - 1) / _superblock->inodes_per_group;
}

unsigned int Ext2FS::blockgroup_inode_index(unsigned int inode)
{
	// que número de inodo corresponde de nuestra tabla de inodo de su BLOCK GROUP?
	assert(inode > 0);
	return (inode - 1) % _superblock->inodes_per_group;
}

// Free allocated memory
Ext2FS::~Ext2FS()
{
	delete[] _superblock;
	delete[] _bgd_table;
}

struct Ext2FSSuperblock * Ext2FS::superblock()
{
	return _superblock;
}

unsigned int Ext2FS::block_groups()
{
	return _block_groups;
}

struct Ext2FSBlockGroupDescriptor * Ext2FS::block_group(unsigned int index)
{
	assert(index < _block_groups);
	return & _bgd_table[index];
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSInode & inode)
{
	output << "mode: " << inode.mode << std::endl;
	output << "uid: " << inode.uid << std::endl;
	output << "size: " << inode.size << std::endl;
	output << "atime: " << inode.atime << std::endl;
	output << "ctime: " << inode.ctime << std::endl;
	output << "mtime: " << inode.mtime << std::endl;
	output << "dtime: " << inode.dtime << std::endl;
	output << "gid: " << inode.gid << std::endl;
	output << "links_count: " << inode.links_count << std::endl;
	output << "blocks: " << inode.blocks << std::endl;
	output << "flags: " << inode.flags << std::endl;
	//output << "os_dependant_1: " << inode.os_dependant_1 << std::endl;

	//output << std::hex;
	for(unsigned int i = 0; i < 15; i++)
		output << "block[" << i << "]: " << inode.block[i] << std::endl;
	//output << std:dec;

	output << "generation: " << inode.generation << std::endl;
	output << "file_acl: " << inode.file_acl << std::endl;
	output << "directory_acl: " << inode.directory_acl << std::endl;
	output << "faddr: " << inode.faddr << std::endl;
	//output << "os_dependant_2: " << inode.os_dependant_2 << std::endl[3];

	return output;
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSSuperblock & superblock)
{
	output << "inodes_count: " << superblock.inodes_count << std::endl;
	output << "blocks_count: " << superblock.blocks_count << std::endl;
	output << "reserved_blocks_count: " << superblock.reserved_blocks_count << std::endl;
	output << "free_blocks_count: " << superblock.free_blocks_count << std::endl;
	output << "free_inodes_count: " << superblock.free_inodes_count << std::endl;
	output << "first_data_block: " << superblock.first_data_block << std::endl;
	output << "log_block_size: " << superblock.log_block_size << std::endl;
	output << "log_fragment_size: " << superblock.log_fragment_size << std::endl;
	output << "blocks_per_group: " << superblock.blocks_per_group << std::endl;
	output << "fragments_per_group: " << superblock.fragments_per_group << std::endl;
	output << "inodes_per_group: " << superblock.inodes_per_group << std::endl;
	output << "mount_time: " << superblock.mount_time << std::endl;
	output << "write_time: " << superblock.write_time << std::endl;
	output << "mount_count: " << superblock.mount_count << std::endl;
	output << "max_mount_count: " << superblock.max_mount_count << std::endl;
	output << "magic_bytes: " << superblock.magic_bytes << std::endl;
	output << "state: " << superblock.state << std::endl;
	output << "errors: " << superblock.errors << std::endl;
	output << "minor_revision_level: " << superblock.minor_revision_level << std::endl;
	output << "lastcheck: " << superblock.lastcheck << std::endl;
	output << "checkinterval: " << superblock.checkinterval << std::endl;
	output << "creator_os: " << superblock.creator_os << std::endl;
	output << "revision_level: " << superblock.revision_level << std::endl;
	output << "default_reserved_userid: " << superblock.default_reserved_userid << std::endl;
	output << "default_reserved_groupid: " << superblock.default_reserved_groupid << std::endl;
	// -- EXT2_DYNAMIC_REV Specific --
	output << "first_inode: " << superblock.first_inode << std::endl;
	output << "inode_size: " << superblock.inode_size << std::endl;
	output << "block_group_number: " << superblock.block_group_number << std::endl;
	output << "feature_compatibility: " << superblock.feature_compatibility << std::endl;
	output << "feature_incompatibility: " << superblock.feature_incompatibility << std::endl;
	output << "readonly_feature_compatibility: " << superblock.readonly_feature_compatibility << std::endl;
	output << "uuid: ";
	for(int i = 0; i < 16; i++)
		output << superblock.uuid[i];
	output << std::endl;
	output << "volume_name: ";
	for(int i = 0; i < 16; i++)
		output << superblock.volume_name[i];
	output << std::endl;
	output << "last_mounted: ";
	for(int i = 0; i < 64; i++)
		output << superblock.last_mounted[i];
	output << std::endl;
	output << "algo_bitmap: " << superblock.algo_bitmap << std::endl;
	// Performance hints
	output << "prealloc_blocks: " << (unsigned int) superblock.prealloc_blocks << std::endl;
	output << "prealloc_dir_blocks: " << (unsigned int) superblock.prealloc_dir_blocks << std::endl;
	//char alignment[2];
	// Journaling support
	output << "journal_uuid: ";
	for(int i = 0; i < 16; i++)
		output << superblock.journal_uuid[i];
	output << std::endl;
	output << "journal_inode: " << superblock.journal_inode << std::endl;
	output << "journal_device: " << superblock.journal_device << std::endl;
	output << "last_orphan: " << superblock.last_orphan << std::endl;
	// Directory indexing support
	output << "hash_seed: ";
	for(int i = 0; i < 4; i++)
		output << superblock.hash_seed[i];
	output << std::endl;
	output << "default_hash_version: " << (unsigned int) superblock.default_hash_version << std::endl;
	//char padding[3];
	// Other options
	output << "default_mount_options: " << superblock.default_mount_options << std::endl;
	output << "first_meta_bg: " << superblock.first_meta_bg << std::endl;
	//char unused[760];

	return output;
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSBlockGroupDescriptor & bg_descriptor)
{
	output << "block_bitmap: " << bg_descriptor.block_bitmap << std::endl;
	output << "inode_bitmap: " << bg_descriptor.inode_bitmap << std::endl;
	output << "inode_table: " << bg_descriptor.inode_table << std::endl;
	output << "free_blocks_count: " << bg_descriptor.free_blocks_count << std::endl;
	output << "free_inodes_count: " << bg_descriptor.free_inodes_count << std::endl;
	output << "used_dirs_count: " << bg_descriptor.used_dirs_count << std::endl;

	return output;
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSDirEntry & dent)
{
	output << "inode: " << dent.inode << std::endl;
	output << "record_length: " << dent.record_length << std::endl;
	output << "name_length: " << (unsigned short) dent.name_length << std::endl;
	output << "file_type: " << (unsigned short) dent.file_type << std::endl;

	// NULL terminate dent name
	char * dentname = new char[dent.name_length + 1];
	strncpy(dentname, dent.name, dent.name_length);
	dentname[dent.name_length] = '\0';

	output << "name: " << dentname << std::endl;

	delete[] dentname;

	return output;
}

struct Ext2FSInode * Ext2FS::inode_for_path(const char * path)
{
	char * mypath = new char[strlen(path)+1];
	mypath[strlen(path)] = '\0';
	strncpy(mypath, path, strlen(path));

	char * pathtok = strtok(mypath, PATH_DELIM);
	struct Ext2FSInode * inode = NULL;

	while(pathtok != NULL)
	{
		struct Ext2FSInode * prev_inode = inode;
		// std::cerr << "pathtok: " << pathtok << std::endl;
		inode = get_file_inode_from_dir_inode(prev_inode, pathtok);
		pathtok = strtok(NULL, PATH_DELIM);

		delete prev_inode;
	}

	delete[] mypath;
	return inode;
}

unsigned int Ext2FS::blockaddr2sector(unsigned int block)
{
	// Get partition info
	const PartitionEntry & pentry = _hdd[_partition_number];

	// Compute block size by shifting the value 1024
	unsigned int block_size = 1024 << _superblock->log_block_size;
	unsigned int sectors_per_block = block_size / SECTOR_SIZE;

	return pentry.start_lba() + block * sectors_per_block;
}

/**
 * Warning: This method allocates memory that must be freed by the caller
 */
struct Ext2FSInode * Ext2FS::load_inode(unsigned int inode_number)
{
	// tenemos  boot-sector | grupo 0 | grupo 1 | ... | grupo n
	// donde dentro de cada grupo | superblock | descriptor de grupo | db bitmap | inode bitmap | INODE TABLE | data blocks 
	// la finalidad de esta función es traer a memoria el bloque que tiene al inodo que buscamos y devolver al inodo como resultado

	// paso 1 : sacarle el nro de grupo
	unsigned int nro_grupo = blockgroup_for_inode(inode_number);

	// paso 2 : traemos el Ext2FSBlockGroupDescriptor del grupo
	Ext2FSBlockGroupDescriptor* descriptor = block_group(nro_grupo);

	// paso 3 : necesitamos la dire de la INODE TABLE
	unsigned int dire_tabla = descriptor->inode_table;
	
	// paso 4 : sacarle el índice en la INODE TABLE 
	unsigned int indice = blockgroup_inode_index(inode_number);

	// paso 5 : qué tan grande es el bloque ? 
	// se calcula así porq el tam de bloque puede ser 1KB (1024 * 1), 2KB (1024 * 2)
	// recordar que  log_block_size es log base 2 del tam del bloque. 1024 es base
	unsigned int tam_bloque = 1024 << _superblock->log_block_size; 

	// paso 6 : cuántos inodos entran en un bloque ? 
	unsigned int cant_inodos_por_bloque =  tam_bloque / _superblock->inode_size;

	// paso 7 : qué bloque tiene al inodo que buscamos ? 
	unsigned int bloque_inodo = indice / cant_inodos_por_bloque ;

	// paso 8 : cuál es el offset dentro de ese bloque para acceder al inodo que buscamos ?
	unsigned int offset_inodo = (indice % cant_inodos_por_bloque) * _superblock->inode_size;

	// paso 9 : declaramos los buffers
	unsigned char* res  = (unsigned char*) malloc(sizeof(Ext2FSInode));
	unsigned char* lectura = (unsigned char*) malloc(sizeof(tam_bloque));
	
	// paso 10 : lectura
	read_block(dire_tabla + bloque_inodo, lectura);

	// paso 11 : copiamos la data a res
	for (int i = 0; i < sizeof(Ext2FSInode); i++)
	{
		res[i] = lectura[offset_inodo + i];
	}

	// dice que tenemos que liberar memoria
	delete[] lectura;

	// como era un puntero a char, lo casteamos
	return (Ext2FSInode*)res;
}

unsigned int Ext2FS::get_block_address(struct Ext2FSInode * inode, unsigned int block_number)
{
	// para sacar la dire del bloque que contiene al inodo pueden suceder varios escenarios 
	// está en los directos, en el indirecto simple o en el indirecto doble ( no hay indirecto triple en este taller)
	
	// caso 1 : directo
	if (block_number < 12)
	{
		return inode->block[block_number];
	}
	
	// caso 2 : indirectos
	else 
	{
		// declaramos res
		unsigned int res;

		// indirecto simple
		// la cantidad de punteros que tiene este será...
		unsigned int limite_indirecto_simple = _superblock->log_block_size / sizeof(unsigned int);
		
		if (block_number < limite_indirecto_simple)
		{
			// declaramos el buffer de lectura
			unsigned char* lectura = (unsigned char*) malloc(1024 << _superblock->log_block_size);
			
			// dire de la tabla de indirecto simple
			unsigned int dire_bloque_indirectos_simple = inode->block[12];
			
			// traemos el bloque con todos los punteros
			read_block(dire_bloque_indirectos_simple, lectura);

			// indexamos adecuadamente (recordar que del 12 en adelante está en los indirectos, en esta caso cae en el simple)
			unsigned int dire_bloque = block_number - 12;

			// res = a ese punterito
			res = ((unsigned int*)lectura)[dire_bloque];

			// liberamos memoria
			delete[] lectura;
		}
		// indirecto doble
		else
		{
			// dire de la tabla de indirectos doble 
			unsigned int dire_bloque_indirectos_doble = inode->block[13];

			// declaramos el buffer 
			unsigned char* lectura = (unsigned char*) malloc(1024 << _superblock->log_block_size);
			
			// traemos el bloque de punteros
			read_block(dire_bloque_indirectos_doble, lectura);

			// nos traemos el bloque apuntado por el primer puntero
			unsigned int primero = block_number - limite_indirecto_simple;

			// dire y lectura del primero de la segunda tabla
			unsigned int dire_bloque_apuntado_primer_puntero = ((unsigned int*) lectura)[primero];
			read_block(dire_bloque_apuntado_primer_puntero, lectura);

			// cuánto nos movemos ?
			unsigned int offset = primero % _superblock->inode_size;

			// res
			res = ((unsigned int*)lectura)[offset];	
			
			// liberamos memoria
			delete[] lectura;
		}
		// casteamos a un unsigned int
		return res;
	}
}

void Ext2FS::read_block(unsigned int block_address, unsigned char * buffer)
{
	unsigned int block_size = 1024 << _superblock->log_block_size;
	unsigned int sectors_per_block = block_size / SECTOR_SIZE;
	for(unsigned int i = 0; i < sectors_per_block; i++)
		_hdd.read(blockaddr2sector(block_address)+i, buffer+i*SECTOR_SIZE);
	}

struct Ext2FSInode * Ext2FS::get_file_inode_from_dir_inode(struct Ext2FSInode * from, const char * filename)
{
	if(from == NULL)
		from = load_inode(EXT2_RDIR_INODE_NUMBER);
	//std::cerr << *from << std::endl;
	assert(INODE_ISDIR(from));	

	// paso 1 : buffer del primer bloque lleno de dir-entries apuntado por el primer puntero 
	unsigned char* lectura_bloque = (unsigned char*) malloc (1024 << _superblock->log_block_size);

	// paso 2 : contador de bloque
	int bloque = 0;

	// paso 3 : dire bloque
	unsigned int dire_bloque = get_block_address(from,bloque);

	// paso 4 : lectura bloque
	read_block(dire_bloque, lectura_bloque);

	// paso 5 : definimos el índice del dir entry (que como ex variable se actualiza variablemente)
	unsigned int offset = 0;

	// paso 6 : condición de ciclo ( cuántos bloques tiene el directorio ?)
	unsigned int bloque_max = from->size / (1024 << _superblock->log_block_size);

	// paso 6 : ciclo
	while (bloque < bloque_max)
	{
		// paso 7 : decidir si pasamos al siguiente bloque
		if (offset > (1024 << _superblock->log_block_size))
		{
			// nuevo bloque
			bloque++;

			// actualizar el valor de offset
			offset -= (1024 << _superblock->log_block_size);

			// cargamos el bloque
			dire_bloque = get_block_address(from, bloque);
			read_block(dire_bloque, lectura_bloque);
		}

		// paso 8 : nos traemos el dir entry actual
		Ext2FSDirEntry* dir_entry_actual = ((Ext2FSDirEntry*)lectura_bloque + offset);

		// paso 9 : por la estructura del dir-entry tenemos unsigned char name_length; y char name[];
		if (strlen(filename) == dir_entry_actual->name_length && !strncmp(filename, dir_entry_actual->name, strlen(filename)))
		{
			// liberamos memo
			delete[] lectura_bloque;
			// final : si encontramos el archivo, le cargamos el inodo unsigned int inode;
			return load_inode(dir_entry_actual->inode);
		}
		
		// paso 10 : en caso de que no sea el dir_entry ==> pasamos al siguiente modificando el índice con unsigned short record_length;
		offset += dir_entry_actual->record_length;

	}

	// si no lo encuentra ==>
	return NULL;
}

fd_t Ext2FS::get_free_fd()
{
	for(fd_t fd = 0; fd < EXT2_MAX_OPEN_FILES; fd++)
	{
		// Warning: This is inefficient
		if(!(_fd_status[fd/sizeof(unsigned int)] & (1 << (fd % sizeof(unsigned int)))))
			return fd;
	}
	return -1;
}

void Ext2FS::mark_fd_as_used(fd_t fd)
{

	_fd_status[fd/sizeof(unsigned int)] = (_fd_status[fd/sizeof(unsigned int)] | (1 << (fd % sizeof(unsigned int))));
}

void Ext2FS::mark_fd_as_free(fd_t fd)
{

	_fd_status[fd/sizeof(unsigned int)] = (_fd_status[fd/sizeof(unsigned int)] ^ (1 << (fd % sizeof(unsigned int))));
}

fd_t Ext2FS::open(const char * path, const char * mode)
{
	fd_t new_fd = get_free_fd();
	if(new_fd < 0)
		return -1;
	mark_fd_as_used(new_fd);

	// We ignore mode
	struct Ext2FSInode * inode = inode_for_path(path);
	assert(inode != NULL);
	std::cerr << *inode << std::endl;

	if(inode == NULL)
		return -1;

	_open_files[new_fd] = *inode;
	delete inode;
	_seek_memory[new_fd] = 0;

	return new_fd;
}

int Ext2FS::read(fd_t filedesc, unsigned char * buffer, int size)
{
	int realsize = ((_seek_memory[filedesc] + size) >= _open_files[filedesc].size)?_open_files[filedesc].size-_seek_memory[filedesc]:size;
	if(realsize > 0)
	{
		int seek = _seek_memory[filedesc];
		unsigned int read = 0;

		// Compute block size by shifting the value 1024
		unsigned int block_size = 1024 << _superblock->log_block_size;
		unsigned int sectors_per_block = block_size / SECTOR_SIZE;

		unsigned int start_block = (seek / block_size);
		unsigned int end_block = ((seek + realsize - 1) / block_size);


		for(unsigned int block = start_block; block <= end_block; block++)
		{
			unsigned int block_address = get_block_address(&_open_files[filedesc], block);

			unsigned char block_buf[block_size];
			read_block(block_address, block_buf);

			do
			{
				buffer[read++] = block_buf[seek++ % block_size];
			} while(((seek % block_size) != 0) && (read < realsize));
		}
	}
	return realsize;
}

int Ext2FS::write(fd_t filedesc, const unsigned char * buffer, int size)
{
	return -1;
}

int Ext2FS::seek(fd_t filedesc, int offset)
{
	//std::cerr << "offset: " << offset << " size: " << _open_files[filedesc].size << std::endl;
	int position = offset;
	if(offset < 0)
		position = _open_files[filedesc].size + offset;
	if(position >= _open_files[filedesc].size)
		position = _open_files[filedesc].size - 1;
	_seek_memory[filedesc] = position;
	return position;
}

int Ext2FS::tell(fd_t filedesc)
{
	return _seek_memory[filedesc];
}

void Ext2FS::close(fd_t filedesc)
{
	mark_fd_as_free(filedesc);
}
