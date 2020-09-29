
#NAME: VINCENT CHI
#EMAIL: vincentchi9702@gmail.com
#ID: 304576879
from __future__ import print_function

import sys
import csv

'''
SUPERBLOCK
total number of blocks (decimal)
total number of i-nodes (decimal)
block size (in bytes, decimal)
i-node size (in bytes, decimal)
blocks per group (decimal)
i-nodes per group (decimal)
first non-reserved i-node (decimal)

'''


class SUPERBLOCK():
	def __init__(self, num_blocks, num_inodes, block_size, inode_size, blocks_per_group,inodes_per_group,first_nonreserved_inode):
		self.num_blocks = num_blocks;
		self.num_inodes = num_inodes;
		self.block_size = block_size;
		self.inode_size = inode_size;
		self.blocks_per_group = blocks_per_group;
		self.inodes_per_group = inodes_per_group;
		self.first_nonreserved_inode = first_nonreserved_inode


'''
GROUP
group number (decimal, starting from zero)
total number of blocks in this group (decimal)
total number of i-nodes in this group (decimal)
number of free blocks (decimal)
number of free i-nodes (decimal)
block number of free block bitmap for this group (decimal)
block number of free i-node bitmap for this group (decimal)
block number of first block of i-nodes in this group (decimal)
'''

class GROUP():
	def __init__(self,group_num, total_blocks, total_inodes, free_blocks, free_inodes, block_bitmap, inode_bitmap, first_block_inodes):
		self.group_num = group_num
		self.total_blocks = total_blocks
		self.total_inodes = total_inodes
		self.free_blocks = free_blocks
		self.free_inodes = free_inodes
		self.block_bitmap = block_bitmap
		self.inode_bitmap = inode_bitmap
		self.first_block_inodes = first_block_inodes



class BFREE():
	def __init__(self,block_num):
		self.block_num = block_num


class IFREE():
	def __init__(self,block_num):
		self.block_num = block_num

'''
INODE
inode number (decimal)
file type ('f' for file, 'd' for directory, 's' for symbolic link, '?" for anything else)
mode (low order 12-bits, octal ... suggested format "%o")
owner (decimal)
group (decimal)
link count (decimal)
time of last I-node change (mm/dd/yy hh:mm:ss, GMT)
modification time (mm/dd/yy hh:mm:ss, GMT)
time of last access (mm/dd/yy hh:mm:ss, GMT)
file size (decimal)
number of (512 byte) blocks of disk space (decimal) taken up by this file
'''

class INODE():
	def __init__(self,inode_number, file_type, mode, owner, group, link_count, time_change, time_mod, time_access, file_size, num_blocks, blocks):
		self.inode_number = inode_number
		self.file_type = file_type
		self.mode = mode
		self.owner = owner
		self.group = group
		self.link_count = link_count
		self.time_change = time_change
		self.time_mod = time_mod
		self.time_access = time_access
		self.file_size = file_size
		self.num_blocks = num_blocks
		self.blocks = blocks

'''
DIRENT
parent inode number (decimal) ... the I-node number of the directory that contains this entry
logical byte offset (decimal) of this entry within the directory
inode number of the referenced file (decimal)
entry length (decimal)
name length (decimal)
name (string, surrounded by single-quotes). Don't worry about escaping, we promise there will be no single-quotes or commas in any of the file names.
'''

class DIRENT():
	def __init__(self,parent_inode_number, byte_offset, inode_number, entry_length, name_length, name):
		self.parent_inode_number = parent_inode_number
		self.byte_offset = byte_offset
		self.inode_number = inode_number
		self.entry_length = entry_length
		self.name_length = name_length
		self.name = name

'''
INDIRECT
I-node number of the owning file (decimal)
(decimal) level of indirection for the block being scanned ... 1 for single indirect, 2 for double indirect, 3 for triple
logical block offset (decimal) represented by the referenced block. If the referenced block is a data block, this is the logical block offset of that block within the file. If the referenced block is a single- or double-indirect block, this is the same as the logical offset of the first data block to which it refers.
block number of the (1, 2, 3) indirect block being scanned (decimal) . . . not the highest level block (in the recursive scan), but the lower level block that contains the block reference reported by this entry.
block number of the referenced block (decimal)
'''
class INDIRECT():
	def __init__(self,inode_number_owner, level, block_offset, block_number_indirect, block_number):
		self.inode_number_owner = inode_number_owner
		self.level = level
		self.block_offset = block_offset
		self.block_number_indirect = block_number_indirect
		self.block_number = block_number




def check_block(block_num,highest_block_num, non_reserved):
	if (block_num != 0):
		if (block_num < 0 or block_num > highest_block_num ):
			return 'invalid'
		elif (block_num < non_reserved ):
			return 'reserved'


	return 'zero'



def main():
    exit_status = 0

    if len(sys.argv) != 2:
        print("invalid arguments",file=sys.stderr)
        sys.exit(1)


    inodes = []
    groups = []
    freeblocks = []
    superblocks = []
    dirents = []
    inodes = []
    indirects = []
    free_inodes = []



    try:

	    with open(sys.argv[1]) as csv_file:
			csv_reader = csv.reader(csv_file, delimiter=',')
			line_count = 0
			for row in csv_reader:
			    if (row[0] == "GROUP"):
			    	group = GROUP(int(row[1]),int(row[2]),int(row[3]),int(row[4]),int(row[5]),int(row[6]),int(row[7]),int(row[8]))
			    	groups.append(group)
			    	continue
			    if (row[0] == "SUPERBLOCK"):
			    	superblock = SUPERBLOCK(int(row[1]),int(row[2]),int(row[3]),int(row[4]),int(row[5]),int(row[6]),int(row[7]))
			    	superblocks.append(superblock)
			    	continue
			    if (row[0] == "BFREE"):
			    	freeblocks.append(int(row[1]))
			    	continue

			    if (row[0] == "IFREE"):
			    	free_inodes.append(int(row[1]))
			    	continue

			    if (row[0] == "INODE"):
			    	block_table = []
			    	for i in range(15):
			    		block_table.append(int(row[12+i]))
			    	#print(block_table)
			    	inode = INODE(int(row[1]), row[2],int(row[3]),int(row[4]), int(row[5]), int(row[6]), row[7],row[8],row[9], int(row[10]), int(row[11]), block_table )
			    	inodes.append(inode)
			    	continue
			    if (row[0] == "DIRENT"):
			    	dirent = DIRENT(int(row[1]), row[2],int(row[3]),int(row[4]), int(row[5]),row[6])
			    	dirents.append(dirent)
			    	continue
			    if (row[0] == "INDIRECT"):
			    	indirect = INDIRECT(int(row[1]), int(row[2]),int(row[3]),int(row[4]),int(row[5]))
			    	indirects.append(indirect)
			    	continue	


    except IOError:
		print("error opening csv file",file=sys.stderr)
		sys.exit(1)




    highest_block_num = superblocks[0].num_blocks

    non_reserved = groups[0].first_block_inodes + (groups[0].total_inodes * superblocks[0].inode_size) / superblocks[0].block_size




    ##########################################BLOCK AUDITS##########################################
    references = {}
    for i in range(non_reserved,highest_block_num):
    	references[i] = []
    
    for i in inodes:
      if (i.file_type != 's'):
    	index = 0

    	for k in (i.blocks):

    		if (k == 0):
    			index += 1
    			continue
    		elif (index < 12):

    			if (check_block(k,highest_block_num,non_reserved) == "invalid"):
    				print("INVALID BLOCK {} IN INODE {} AT OFFSET {}".format(k, i.inode_number,index))
    				exit_status = 2
    			elif (check_block(k,highest_block_num,non_reserved) == "reserved"):
    				print("RESERVED BLOCK {} IN INODE {} AT OFFSET {}".format(k, i.inode_number,index))
    				exit_status = 2
    			else:
    				references[k].append([k,i.inode_number,index,0])


    				
    		elif (index == 12):
    			if (check_block(k,highest_block_num,non_reserved) == "invalid"):
    				print("INVALID INDIRECT BLOCK {} IN INODE {} AT OFFSET {}".format(k,i.inode_number,12))
    				exit_status = 2
    			elif (check_block(k,highest_block_num,non_reserved) == "reserved"):
    				print("RESERVED INDIRECT BLOCK {} IN INODE {} AT OFFSET {}".format(k,i.inode_number,12))
    				exit_status = 2
    			else:
    				references[k].append([k,i.inode_number,12,1])
    			
    				
    		elif (index == 13):
    			if (check_block(k,highest_block_num,non_reserved) == "invalid"):
    				print("INVALID DOUBLE INDIRECT BLOCK {} IN INODE {} AT OFFSET {}".format(k,i.inode_number,12+256))
    				exit_status = 2
    			elif (check_block(k,highest_block_num,non_reserved) == "reserved"):
    				print("RESERVED DOUBLE INDIRECT BLOCK {} IN INODE {} AT OFFSET {}".format(k,i.inode_number,12+256))
    				exit_status = 2
    			else:
    				references[k].append([k,i.inode_number,12 + 256,2])
    				
    		elif (index == 14):
    			if (check_block(k,highest_block_num,non_reserved) == "invalid"):
    				print("INVALID TRIPLE INDIRECT BLOCK {} IN INODE {} AT OFFSET {}".format(k,i.inode_number,12 + 256 + 256*256))
    				exit_status = 2
    			elif (check_block(k,highest_block_num,non_reserved) == "reserved"):
    				print("RESERVED TRIPLE INDIRECT BLOCK {} IN INODE {} AT OFFSET {}".format(k,i.inode_number,12 + 256 + 256*256))
    				exit_status = 2
    			else:
    				references[k].append([k,i.inode_number,12 + 256 + 256*256,3])
    				
    		index += 1


    for i in indirects:  
        if (i.block_number == 0):
        	continue
    	if (i.level == 1):
    		if( check_block(i.block_number,highest_block_num,non_reserved) == "invalid"):
    			print("INVALID BLOCK {} IN INODE {} AT OFFSET {}".format(i.block_number, i.inode_number_owner,i.block_offset))
    			exit_status = 2
    		elif (check_block(i.block_number,highest_block_num,non_reserved) == "reserved"):
    			print("RESERVED BLOCK {} IN INODE {} AT OFFSET {}".format(i.block_number, i.inode_number_owner,i.block_offset))
    			exit_status = 2
    		else:
    				references[i.block_number].append([i.block_number, i.inode_number_owner,i.block_offset,0])

    	if (i.level == 2):
    		if( check_block(i.block_number,highest_block_num,non_reserved) == "invalid"):
    			print("INVALID INDIRECT BLOCK {} IN INODE {} AT OFFSET {}".format(i.block_number, i.inode_number_owner,i.block_offset))
    			exit_status = 2
    		elif (check_block(i.block_number,highest_block_num,non_reserved) == "reserved"):
    			print("RESERVED INDIRECT BLOCK {} IN INODE {} AT OFFSET {}".format(i.block_number, i.inode_number_owner,i.block_offset))
    			exit_status = 2
    		else:
    				references[i.block_number].append([i.block_number, i.inode_number_owner,i.block_offset,1])


    	if (i.level == 3):
    		if( check_block(i.block_number,highest_block_num,non_reserved) == "invalid"):
    			print("INVALID DOUBLE INDIRECT BLOCK {} IN INODE {} AT OFFSET {}".format(i.block_number, i.inode_number_owner,i.block_offset))
    			exit_status = 2
    		elif (check_block(i.block_number,highest_block_num,non_reserved) == "reserved"):
    			print("RESERVED DOUBLE INDIRECT BLOCK {} IN INODE {} AT OFFSET {}".format(i.block_number, i.inode_number_owner,i.block_offset))
    			exit_status = 2
    		else:
    				references[i.block_number].append([i.block_number, i.inode_number_owner,i.block_offset,2])
    


    for i in range(non_reserved, highest_block_num):
    	if ( not(i in freeblocks) and (len(references[i]) == 0) ):
    		print("UNREFERENCED BLOCK {}".format(i))
    		exit_status = 2


    for i in range(non_reserved, highest_block_num):
    	if ( (i in freeblocks) and (len(references[i]) != 0) ):
    		print("ALLOCATED BLOCK {} ON FREELIST".format(i))
    		exit_status = 2

    for i in range(non_reserved, highest_block_num):
    	if (  (len(references[i]) > 1) ):
    		for block in references[i]:
    			blocknum = block[0]
    			inode = block[1]
    			offset = block[2]
    			blocktype = ''
    			if (block[3] == 0):
    				blocktype = ''

    			elif (block[3] == 1):
    				blocktype = "INDIRECT "
    			elif (block[3] == 2):
    				blocktype = "DOUBLE INDIRECT "
    			elif (block[3] == 3):
    				blocktype = "TRIPLE INDIRECT "
    			print("DUPLICATE {}BLOCK {} IN INODE {} AT OFFSET {}".format(blocktype,blocknum,inode,offset))
    			exit_status = 2
    
    ##########################################INODE ALLOCATION AUDITS##########################################

    inode_nums = []

    for i in inodes:
         inode_nums.append(i.inode_number)
    for i in inode_nums:
         if i in free_inodes:
                print("ALLOCATED INODE {} ON FREELIST".format(i))

    for i in range(superblocks[0].first_nonreserved_inode,superblocks[0].num_inodes):
    	if (not(i in free_inodes) and not(i in inode_nums)):
    		print("UNALLOCATED INODE {} NOT ON FREELIST".format(i))
    		exit_status = 2
    	elif ((i in free_inodes) and (i in inode_nums)):
    		print("ALLOCATED INODE {} ON FREELIST".format(i))
    		exit_status = 2
    	

    
    ##########################################DIRECTORY CONSISTAENCY AUDITS##########################################
    inode_links = {}
    inode_links_true = {}
    parents = {2:2}

    for i in inodes:
    	inode_links[i.inode_number] = 0
    	inode_links_true[i.inode_number] = i.link_count
    

    for d in dirents:

    	if (d.inode_number < 1 or d.inode_number >= superblocks[0].num_inodes ):
    		print("DIRECTORY INODE {} NAME {} INVALID INODE {}".format(d.parent_inode_number,d.name,d.inode_number))
    		exit_status = 2


    	elif (d.inode_number in inode_links.keys()): 
    		inode_links[d.inode_number]  += 1

    		if (d.name != "'.'" and d.name != "'..'"):
    			parents[d.inode_number] = d.parent_inode_number

    	elif not(d.inode_number in inode_links.keys()):
    		print("DIRECTORY INODE {} NAME {} UNALLOCATED INODE {}".format(d.parent_inode_number,d.name,d.inode_number))
    		exit_status = 2
    	


    	

    ####CHECK '.' AND '..' LINKS#####
    for d in dirents:
    	if (d.name == "'.'"):
    		if (d.parent_inode_number != d.inode_number):
    			print("DIRECTORY INODE {} NAME '..' LINK TO INODE {} SHOULD BE {}".format(d.parent_inode_number,d.inode_number, d.parent_inode_number))
    			exit_status = 2

    	if (d.name == "'..'"):
    		if (d.inode_number != parents[d.parent_inode_number]):
    			print("DIRECTORY INODE {} NAME '..' LINK TO INODE {} SHOULD BE {}".format(d.parent_inode_number,d.inode_number, d.parent_inode_number))
    			exit_status = 2


 
    for i in inode_links.keys():
    	if inode_links[i] != inode_links_true[i]:
    		print("INODE {} HAS {} LINKS BUT LINKCOUNT IS {}".format(i,inode_links[i],inode_links_true[i]))
    		exit_status = 2


    sys.exit(exit_status)



if __name__ == "__main__":

	main()








