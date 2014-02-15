
/*
 * DFS
 *
 * Contains file related operations.
 * Most of the operations are just redirects to generic file io.
 */

 #include <include/linux/fs.h>

 loff_t
 dfs_file_lseek(struct file *filp, loff_t pos, int origin)
 {

 	return generic_file_llseek(filp, pos, origin);
 }

 ssize_t
 dfs_file_read(struct file *filp, char __user *buf, size_t len, loff_t *pos)
 {
 	return do_sync_read(filp, buf, len, ppos);

 }

 ssize_t
 dfs_file_write(struct file *filp, char __user *buf, size_t len, loff_t *pos)
 {
 	return do_sync_write(filp, buf, len, pos);

 }

 int
 dfs_file_open(struct inode *ino, struct file *filp)
 {
 	return generic_file_open(ino, filp);

 }

 const struct file_operations dfs_file_operations = {
 	.llseek		= dfs_file_lseek,
	.read		= dfs_file_read,
	.write		= dfs_file_write,
	.open		= dfs_file_open
};
