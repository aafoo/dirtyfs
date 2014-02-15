
/*
 *
 * super block primitives.
 */

#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/parser.h>
#include <linux/random.h>
#include <linux/buffer_head.h>
#include <linux/exportfs.h>
#include <linux/vfs.h>
#include <linux/seq_file.h>
#include <linux/mount.h>
#include <linux/log2.h>
#include <asm/uaccess.h>

 /*
  * struct to define the file system type.
  */
static struct file_system_type dirty_fs_type = {
	.owner		= THIS_MODULE,
	.name		= "dfs",
	.mount		= dfs_mount,
	.kill_sb	= kill_super_block,	// Need to figure this out :)
	.fs_flag	= FS_REQUIRES_DEV	// What is that? 
};

static struct dentry *dfs_mount(struct file_system_type *fs_type, int flags,
				const char *dev_name, void *data)
{
	return mount_dbev(fs_type, flags, dev_name, data, dfs_fill_super);

}

static int dfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct buffer_hear *bh = NULL;
	struct dfs_super_block *dsb = NULL;
	struct dfs_super_block_info *sbi = NULL;
	struct inode *inode = NULL;
	unsigned long sb_block_no = 1;
	int blocksize = BLOCK_SIZE;
	int err;
	int i = 0;
	int j = 0;

	err = -ENOMEM;
	sbi = kzalloc(sizeof(*sbi), GFP_KERNEL);
	if (!sbi) {
		goto err_out;
	}

	sb->s_fs_info = sbi;
	sbi->s_sb_block = sb_block_no;

	blocksize = sb_min_blocksize(sb, BLOCK_SIZE);

	if (blocksize != BLOCK_SIZE) {
		logical_sb_block = (sb_block_no * BLOCK_SIZE) / blocksize;
		offset = (sb_block_no * BLOCK_SIZE) % blocksize;
	} else {
		logical_sb_block = sb_block_no;
	}

	if (!(bh = sb_bread(sb, logical_sb_block))) {
		goto err_out;
	}

	dsb = (struct dfs_super_block *) (((char *)bh->b_data) + offset);
	sbi->s_dsb = dsb;
	sb->s_magic = le32_to_cpu(dsb->dsb_magic);

	if (sb->s_magic != DSB_SUPER_MAGIC) {
		goto err_out;
	}

	sbi->s_blocks_per_group = le32_to_cpu(dsb->dsb_blocks_per_group);
	sbi->s_inodes_per_group = le32_to_cpu(dsb->dsb_inodes_per_group);
	sbi->s_inodes_per_block = le32_to_cpu(dsb->dsb_block_size) /
					le16_to_cpu(dsb->dsb_inodes_size);
	sbi->s_itb_per_group = sbi->s_inodes_per_group /
					sbi->s_inodes_per_block;

	desc_block_count = le32_to_cpu(dsb->dsb_num_of_blocks) /
				le32_to_cpu(dsb->dsb_blocks_per_group) + 1;
	
	sbi->sbi_group_desc = kmalloc(desc_block_count * sizeof(struct buffer_head *), GFP_KERNEL);
	if (sbi->sbi_group_desc == NULL) {
		goto err_out;
	}

	for (i = 0; i < desc_block_count; i++) {
		block = descriptor_loc(sb, logic_sb_block, i);
		sbi->sbi_group_desc[i] = sb_bread(sb, block);
		if (!sbi->sbi_group_desc[i]) {
			for (j = 0; j < i; j++) {
				brelse(sbi->sbi_group_desc[j]);
			}
		}
		
	}

	sbi->sbi_db_count = desc_block_count;
	get_random_bytes(&sbi->sbi_next_gen, sizeof(u32));
	sbi->sbi_num_free_blocks = le32_to_cpu(dsb->dsb_free_num_blocks);
	sbi->sbi_free_inodes = le32_to_cpu(dsb->dsb_free_inodes);

	sb->s_ops = &dfs_sops;
	sb->s_export_op = &dfs_export_ops;

	root = dfs_iget(sb, DFS_ROOT_INO);
	sb->s_root = d_alloc_root(root);

err_out:
	return err;
}

static struct kmem_cache *dfs_inode_cachep;

/*
 * Called for everytime inode cache entry is created.
 */
static void dfs_init_once(void *foo)
{
	struct dfs_inode *di = (struct dfs_inode *)foo;
	
	inode_init_once(*di->vfs_inode);
}

/*
 * Create the inode cache during mount.
 */
static int dfs_init_inode_cache(void)
{
	dfs_inode_cachep = kmem_create_cache("dfs_inode_cache",
					sizeof(struct dfs_inode),
					0, (SLAB_RECLAIM_ACCOUNT |
					    SLAB_MEM_SPREAD),
					init_once);
	if (dfs_inode_cachep == NULL)
		return -ENOMEM;
	return 0;
}

static void dfs_destroy_inodecache(void)
{
	kmem_cache_destroy(dfs_inode_cachep);
}

struct inode* dfs_alloc_inode(struct super_block *sb)
{
	struct dfs_inode *di;
	di = (struct dfs_inode *)kmem_cache_alloc(dfs_inode_cachep, GFP_KERNEL);

	if (!di)
		return NULL;
	di->vfs_inode.i_version = 1;
	return &di->vfs_inode;

}

 void dfs_destroy_inode(struct inode *inode)
 {

 }

 int dfs_write_inode(struct inode *inode, struct writeback_control *wbc)
 {

 }

 int dfs_drop_inode(struct inode *inode)
 {

 }

 void dfs_evict_inode(struct inode *inode)
 {

 }

 void dfs_put_super(struct super_block *sb)
 {

 }

 void dfs_write_super(struct super_block *sb)
{

}

int dfs_synch_fs(struct super_block *sb, int wait)
{

}

int dfs_statfs(struct dentry *dentry, int wkstatfs *kstatfs)
{

}

int dfs_remount_fs(struct super_block *sb, int *, char *)
{

}

int dfs_show_options (struct seq_file *seq_file, struct vfsmount *vfsmount)
{

}

static const struct super_operations dfs_sops = {
	.alloc_inode	= dfs_alloc_inode,
	.destroy_inode	= dfs_destroy_inode,
	.write_inode	= dfs_write_inode,
	.drop_inode	= dfs_drop_inode,
	.evict_inode	= dfs_evict_inode,
	.put_super	= dfs_put_super,
	.write_super	= dfs_write_super,
	.sync_fs	= dfs_synch_fs,
	.statfs		= dfs_statfs,
	.remount_fs	= dfs_remount_fs,
	.show_options	= dfs_show_options,

};

static struct inode* dfs_nfs_get_inode(struct super_block *sb,
		u64 ino, u32 generation)
{
	struct inode *inode;

	if (ino < DFS_FIRST_INO(sb) && ino != DFS_ROOT_INO)
		return ERR_PTR(-ESTALE);
	if (ino > le32_to_cpu(EXT2_SB(sb)->dsb_inos))
		return ERR_PTR(-ESTALE);

	inode = dfs_iget(sb, ino);
	if (IS_ERR(inode))
		return ERR_CAST(inode);

	if (generation && inode->i_generation != generation) {
		/*
		 * Not the right inode.
		 */
		iput(inode);
		ERR_PTR(-ESTALE);
	}
	return inode;
}

static struct dentry* dfs_fh_to_dentry(struct super_block *sb, struct fid *fid,
		int fh_len, int fh_type)
{
	return generic_fh_to_dentry(sb, fid, fh_len, fh_type,
				    dfs_nfs_get_inode);
}

static struct dentry* dfs_fh_to_dentry(struct super_block *sb, struct fid *fid,
		int fh_len, int fh_type)
{
	return generic_fh_to_parent(sb, fid, fh_len, fh_type,
				    dfs_nfs_get_inode);
}
static const struct export_operations dfs_export_ops = {
	.fh_to_dentry = dfs_fh_to_dentry,
	.fh_to_parent = dfs_fh_to_parent,
	.get_parent   = dfs_get_parent,
};
