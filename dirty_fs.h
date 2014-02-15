
/*
 * Dirty File System (DFS) is a strawman implementation of linux based file
 * system. This is heavily based on ext2 and I am implementing it for learning
 * how a file system is build from scratch and also learn about kernel details
 * as well.
 * Maybe I will release it as open source for others to learn as well.
 */

/*
 * Size of the on disk inode structure.
 */
#define DFS_INODE_SIZE	1024

/*
 * On disk inode structure definition.
 */
struct dfs_dinode {
	__le16	di_mode;		//2
	__le16	di_uid;			//4
	__le32	di_size;		//8
	__le32	di_atime;		//12
	__le32	di_mtime;		//16
	__le32	di_ctime;		//20
	__le16	di_gid;			//22
	__le64	di_reserved1;		//30
	__le32	di_generation;		//34
	__le32	di_blocks_num;		//38
	__le32	di_nlinks;		//42
	__le32	di_flags;		//46
	__le16	di_reserved2;		//48
	__le32	di_block[20];		//128
};

/*
 * This is the first block of the file system.
 */
struct dfs_super_block {
	__le32	dsb_magic;		// Magic field to catch corruption.
	__le32	dsb_inos;		// Number of inodes allocated.
	__le32  dsb_free_blocks;	// Number of free blocks.
	__le32  dsb_free_inodes;	// Number of free inodes.
	__le32	dsb_mount_options;	// Some mount options
	__le32	dsb_fs_state;		// Track the status of file like corruption etc.
	__le32	dsb_block_size;		// Size of the number of blocks.
	__le32	dsb_num_of_blocks;	// Total number of blocks in the file system.
	__le32	dsb_free_num_blocks;	// Size of the FS.
	__le32	dsb_max_file_size;	// Maximum size of the file
	__le32	dsb_flags;		// Super block specific flags.
	__le32	dsb_blocks_per_group;	// Number of blocks per group
	__le32	dsb_inode_per_group;	// Number of inodes per group
	__le32	dsb_inode_size;		// Size of inodes in bytes.
	char	dsb_vol_name[32];	// Name of volume.
	__le32	dsb_reserved[245];	// Reserved field.
};

/*
 * Incore super block structure.
 */
struct dfs_super_block_info {
	struct dfs_super_block *sbi_sb;
	struct buffer_head *sbi_bh;
	struct buffer_head *sbi_group_desc;
	uint32_t s_sb_block;
	uint32_t s_blocks_per_group;
	uint32_t s_inodes_per_group;
	uint32_t s_inodes_per_block;
	uint32_t s_itb_per_group;

};

struct dfs_block_desc {
	__le32	dbd_inode_bitmap;	// Inode bitmap block.
	__le32	dbd_block_bitmap;	// Data bitmap block.
	__le32	dbd_num_free_blocks;	// Number of free blocks.
	__le32	dbd_num_of_files;	// Number of files.
	__le32	dbd_inode_table;	// inode table blocks;
	__le32	dbd_num_inode_blocks;	// Total number of inode blocks.
	__le32	dbd_pad[2];
};	// size = 32 bytes.

/*
 * In core inode definition.
 */
struct dfs_inode {

};
