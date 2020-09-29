//VINCENT CHI                                                                                                                                                                    
//304576879                                                                                                                                                                      
//vincentchi9702@gmail.com                                                                                                                                                       

#include <stdint.h>
#include <sys/types.h>

#include "ext2_fs.h"
#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <math.h>
#include <time.h>

int fd;
struct ext2_super_block* super_block;
struct ext2_group_desc* group;
struct ext2_inode* inode;


void print_time(uint32_t time_1970){
  //referenced the following official documentaion to format time
  //http://www.cplusplus.com/reference/ctime/gmtime/
  //http://www.cplusplus.com/reference/ctime/strftime/
  time_t time = time_1970;
  struct tm* time_object = gmtime(&time);
  char date[9];
  strftime(date, 9, "%m/%d/%y", time_object);
  std::cout << date << " "
	    << time_object->tm_hour << ":" 
	    << time_object->tm_min << ":"
	    << time_object->tm_sec << ",";
  return;

}


void SUPERBLOCK(){
  //1024 bytes offset of superblock                                                                                                                
  super_block = (struct ext2_super_block* ) malloc( sizeof(struct ext2_super_block) );

  if ( pread(fd, super_block, sizeof(struct ext2_super_block), 1024) == -1){
    std::cerr << "error getting superblock \n";
    exit(2);
  }

  if( super_block->s_magic != EXT2_SUPER_MAGIC ){
    std::cerr << "file is not ext2 \n";
    exit(2);
  }

  uint32_t total_num_blocks = super_block->s_blocks_count;
  uint32_t total_num_inodes = super_block->s_inodes_count;
  uint32_t block_size = EXT2_MIN_BLOCK_SIZE << super_block->s_log_block_size;
  uint32_t inode_size = super_block->s_inode_size;
  uint32_t blocks_per_group = super_block->s_blocks_per_group;
  uint32_t inodes_per_group = super_block->s_inodes_per_group;
  uint32_t first_nonreserved_inode = super_block->s_first_ino;

  std::cout << "SUPERBLOCK," << total_num_blocks << ","
	    << total_num_inodes << ","
	    << block_size << ","
	    << inode_size << ","
	    << blocks_per_group << ","
	    << inodes_per_group << ","
	    << first_nonreserved_inode << "\n" ;

  return;

}



void GROUP(){

  uint32_t num_groups = super_block->s_blocks_count / super_block->s_blocks_per_group + 1;
  uint32_t num_blocks_left = super_block->s_blocks_count;
  uint32_t num_inodes_left = super_block->s_inodes_count;
  uint32_t group_num;


  for (group_num = 0; group_num < num_groups; group_num++){
    free(group);

    group = (struct ext2_group_desc* ) malloc( sizeof(struct ext2_group_desc) );

    if ( pread(fd, group, sizeof(struct ext2_group_desc), 1024 + sizeof(struct ext2_super_block) +  group_num * sizeof(struct ext2_group_desc) ) == -1){
      std::cerr << "error getting group summaryy \n";
      exit(2);
    }


    uint32_t blocks_in_group = super_block->s_blocks_per_group;
    uint32_t inodes_in_group = super_block->s_inodes_per_group;
    uint32_t num_free_blocks = group->bg_free_blocks_count;
    uint32_t num_free_inodes = group->bg_free_inodes_count;
    uint32_t free_block_bitmap = group->bg_block_bitmap;
    uint32_t free_inode_bitmap = group->bg_inode_bitmap;
    uint32_t first_inode_block = group->bg_inode_table;

      
    if ( num_blocks_left < blocks_in_group)
      blocks_in_group = num_blocks_left;
    else
      num_blocks_left -= blocks_in_group;


    if (num_inodes_left < inodes_in_group)
      inodes_in_group = num_inodes_left;
    else
      num_inodes_left -= inodes_in_group;


    std::cout << "GROUP,"  << group_num << ","
      << blocks_in_group << ","
      << inodes_in_group << ","
      << num_free_blocks << ","
      << num_free_inodes << ","
      << free_block_bitmap << ","
      << free_inode_bitmap << ","
      << first_inode_block << "\n" ;

  }
  return;
}



void FREE_BLOCK_ENTRIES(){


  uint32_t num_groups = super_block->s_blocks_count / super_block->s_blocks_per_group + 1;

  uint32_t group_num;
  uint32_t block_size = EXT2_MIN_BLOCK_SIZE << super_block->s_log_block_size;

  for (group_num = 0; group_num < num_groups; group_num++){
    free(group);

    group = (struct ext2_group_desc* ) malloc( sizeof(struct ext2_group_desc) );

    if ( pread(fd, group, sizeof(struct ext2_group_desc), 1024 + sizeof(struct ext2_super_block) +  group_num * sizeof(struct ext2_group_desc) ) == -1){
      std::cerr << "error getting group summaryy \n";
      exit(2);
    }
      
    for (uint32_t i = 0; i < block_size; i++){

      uint8_t byte;
      //get the bitmap 
      if (pread(fd, &byte , 1, (block_size * group->bg_block_bitmap) + i) == -1) {
std::cerr << "error getting free block summary \n";
exit(2);
      }

      //loop thru each bit of the bitmap
      for (int j = 0; j < 8; j++){

if((byte & (1 << j)) == 0){
  uint block_num = group_num*super_block->s_blocks_per_group + i*8 + j + 1;
  std::cout << "BFREE," << block_num <<  "\n";
}

      }

    }

  }
  return;
}


void FREE_INODES(){


  uint32_t num_groups = super_block->s_blocks_count / super_block->s_blocks_per_group + 1;
  uint32_t group_num;
  uint32_t block_size = EXT2_MIN_BLOCK_SIZE << super_block->s_log_block_size;

  for (group_num = 0; group_num < num_groups; group_num++){
    free(group);
    group = (struct ext2_group_desc* ) malloc( sizeof(struct ext2_group_desc) );

    if ( pread(fd, group, sizeof(struct ext2_group_desc), 1024 + sizeof(struct ext2_super_block) +  group_num * sizeof(struct ext2_group_desc) ) == -1){
      std::cerr << "error getting group summaryy \n";
      exit(2);
    }
      
    for (uint32_t i = 0; i < block_size; i++){

      uint8_t byte;
      //get the bitmap 
      if (pread(fd, &byte , 1, (block_size * group->bg_inode_bitmap) + i) == -1) {
std::cerr << "error getting free block summary \n";
 exit(2);
      }

      //loop thru each bit of the bitmap
      for (int j = 0; j < 8; j++){

	if((byte & (1 << j)) == 0){
	  uint block_num = group_num*super_block->s_blocks_per_group + i*8 + j + 1;
	  std::cout << "IFREE," << block_num <<  "\n";
	}

      }

    }

  }
  return;

}



void DIRECTORY_ENTRIES(struct ext2_inode* inode, int block_index, uint32_t parent_inode_number){

  if (inode->i_block[block_index] == 0){
    return;
  }

  uint32_t block_size = EXT2_MIN_BLOCK_SIZE << super_block->s_log_block_size;

  struct ext2_dir_entry *directory;
  directory = (struct ext2_dir_entry*) malloc(sizeof(struct ext2_dir_entry));

  uint32_t offset = 0;

  while (offset < block_size){

    if (pread(fd,directory,sizeof(struct ext2_dir_entry), inode->i_block[block_index] * block_size + offset  ) == -1){

      std::cerr << "error getting directory from inode \n";
      exit(2);

    }

    if (directory->inode == 0){

      offset += directory->rec_len;
      continue;

    }
    else{

      int name_length = std::string(directory->name).length();

      std::cout << "DIRENT," << parent_inode_number << ","
		<< offset << ","
		<< directory->inode << ","
		<< directory->rec_len << ","
		<< name_length << ","
		<< '\'' << std::string(directory->name)<< '\''
		<< "\n";
    }

    offset += directory->rec_len;


  }

  return;


}

void INDIRECT_BLOCK_REFS(struct ext2_inode*  inode, int block_num, int block_offset, uint32_t parent_inode_number, int level){




  uint32_t block_size = EXT2_MIN_BLOCK_SIZE << super_block->s_log_block_size;

  uint32_t blocks[256];

   

  if (pread(fd, blocks, block_size,  block_num * block_size ) < 0) {
    std::cerr << "error getting array of blocks from indirect block\n";
    exit(2);
  }

  if (level == 1){

    for (int i = 0; i < 256; i++){

      if (blocks[i] != 0){

	std::cout << "INDIRECT," 
		  << parent_inode_number << "," 
		  << level << "," 
		  << block_offset << "," 
		  << block_num << "," 
		  << blocks[i] 
		  << "\n";

      }

      block_offset += 1;
    }

  }

  else if (level == 2){

    for (int i = 0; i < 256; i++){

      if (blocks[i] == 0) continue;
      else std::cout << "INDIRECT," 
		     << parent_inode_number << "," 
		     << level << "," 
		     << block_offset << "," 
		     << block_num << "," 
		     << blocks[i] 
		     << "\n";
      INDIRECT_BLOCK_REFS(inode, blocks[i],  block_offset + 256*i , parent_inode_number, 1);

    }


  }

  else if (level == 3){

    for (int i = 0; i < 256; i++){

      if (blocks[i] == 0) continue;
      else std::cout << "INDIRECT," 
		     << parent_inode_number << "," 
		     << level << "," 
		     << block_offset << "," 
		     << block_num << "," 
		     << blocks[i] 
		     << "\n";

      INDIRECT_BLOCK_REFS(inode, blocks[i], block_offset + 256*256*i , parent_inode_number, 2);

      
    }



  }
}


void INODES(){


  uint32_t num_groups = super_block->s_blocks_count / super_block->s_blocks_per_group + 1;

  uint32_t group_num;
  uint32_t block_size = EXT2_MIN_BLOCK_SIZE << super_block->s_log_block_size;
  uint32_t inode_size = super_block->s_inode_size;


  for (group_num = 0; group_num < num_groups; group_num++){
    free(group);
    group = (struct ext2_group_desc* ) malloc( sizeof(struct ext2_group_desc) );

    if ( pread(fd, group, sizeof(struct ext2_group_desc), 1024 + sizeof(struct ext2_super_block) +  group_num * sizeof(struct ext2_group_desc) ) == -1){
      std::cerr << "error getting group summaryy \n";
      exit(2);
    }
      
    uint32_t inodes_per_group = super_block->s_inodes_per_group;



    uint32_t inodes_in_current_group = 0;

    for (uint32_t i = 0; i < block_size; i++){

      uint8_t byte;
      //get the bitmap 
      if (pread(fd, &byte , 1, (block_size * group->bg_inode_bitmap) + i) == -1) {
	std::cerr << "error getting free block summary \n";
	exit(2);
      }

      //loop thru each bit of the bitmap
      for (int j = 0; j < 8; j++){

	inodes_in_current_group += 1;

	if((byte & (1 << j)) != 0){

	    

	  int inode_location = group->bg_inode_table * block_size + inode_size * ( i*8 + j);

	  inode = (struct ext2_inode* ) malloc( sizeof(struct ext2_inode) );


	  if ( pread(fd, inode, inode_size, inode_location) == -1) {
	    std::cerr << "error getting inode summary \n";
	    exit(2);
	  }

	  //when all links and blocks. associated are freed
	  if ( !(inode->i_mode) || inode->i_links_count == 0 ){
	    continue;
	  }

	    
	  if (inodes_in_current_group > inodes_per_group){
	    break;
	  }

	  //std::cout<< inode_location << "\n";

	  int inode_number = group_num * inodes_per_group + i * 8 + j + 1;
	  char file_type = '?';

	  uint16_t owner = inode->i_uid;
	  uint16_t group = inode->i_gid;
	  uint16_t link_count = inode->i_links_count;

	  uint32_t size = inode->i_size;
	    

	  //anything else
	    
	  //reg file
	  if ((inode->i_mode & 0xF000) ==      0x8000){
	    file_type = 'f';
	  }
	  //symb link
	  if ((inode->i_mode & 0xF000) == 0xA000){
	    file_type = 's';
	  }
	  //directory
	  if ((inode->i_mode & 0xF000) == 0x4000){
	    file_type = 'd';
	  }

	  uint32_t time_of_last_change  = inode->i_ctime;
	  uint32_t time_of_last_modification  = inode->i_mtime;
	  uint32_t time_of_last_access  = inode->i_atime;


	  std::cout << "INODE,"  << inode_number << "," 
		    << file_type << "," 
		    << std::oct << (inode->i_mode & 0xFFF ) << "," 
		    << std::dec << owner << "," 
		    << group << "," 
		    << link_count << "," ;
	  print_time(time_of_last_change);
	  print_time(time_of_last_modification);
	  print_time(time_of_last_access);
	  /*
	    << time_of_last_change << "," 
	    << time_of_last_modification << "," 
	    << time_of_last_access << "," 
	  */
	  /*<< file_size << ","
	    << num_blocks_taken << ","*/
	  std::cout << inode->i_size << ","
		    << inode->i_blocks ;



	  if (!(file_type == 's' && size < 60)){
	        
	    for (int block = 0; block < 15; block++){
	      std::cout << ',' << inode->i_block[block];
	    }

	  }

	  std::cout<< "\n";


	  //Scan first 12 entries of inode table for direct, only if file type is directory
	  if (file_type == 'd'){
	    for (int index = 0; index < 12; index++){
	      DIRECTORY_ENTRIES(inode,index, inode_number);

	    }
	        
	  }

	  int block_offset = 12;

	  for (int index = 12; index < 15; index ++){


	    if (index == 12) {
	      block_offset = 12;

	      INDIRECT_BLOCK_REFS(inode, inode->i_block[index],block_offset, inode_number,1);

	    }

	    if (index == 13) {
	      block_offset = 12 + 256;
	      INDIRECT_BLOCK_REFS(inode, inode->i_block[index],block_offset, inode_number,2);
	    }

	    if (index == 14) {
	      block_offset = 12 + 256 + 256*256;
	      INDIRECT_BLOCK_REFS(inode, inode->i_block[index],block_offset, inode_number,3);

	    }

	    //INDIRECT_BLOCK_REFS(inode,12, inode_number);
	        
	    //void INDIRECT_BLOCK_REFS(struct ext2_inode*  inode,int block_num, int block_offset, uint32_t parent_inode_number, int level){

	  }
	    

	  //scan the primary single, double, triple indirect for other file types






	}

	if (inodes_in_current_group > inodes_per_group){
	  break;
	}


      }

    }


  }
  return;

}










int main(int argc, char* argv[]){

  const char* file = argv[1];

  if(argc == 2){
    if ((fd = open(file, O_RDONLY)) == -1){
      std::cerr << "error opening file \n";
      exit(1);
    }
  }
  else{
    std::cerr << "no file given \n";
    exit(1);
  }
  //  std::cout << fd << "\n";                                                                                                                     

  SUPERBLOCK();


  GROUP();

  FREE_BLOCK_ENTRIES();

  FREE_INODES();

  INODES();

  return 0;
}
