#ifndef COMMON_H
#define COMMON_H

struct driver_info
{
	const char* name;
	int major;

	const struct file_operations* fops;
	const struct device_attribute** dattrs;
 
	int (*init)(void);
	void (*exit)(void);
};

extern struct driver_info info;		//implement this yourself.
extern int debug;

#define trace(format, arg...) do { if( debug & 1 ) pr_info( "%s: %s: " format "\n", info.name, __FUNCTION__, ## arg ); } while (0)
#define info(format, arg...) pr_info( "%s: " format "\n", info.name, ## arg )
#define warning(format, arg...) pr_warn( "%s: " format "\n", info.name, ## arg )
#define error(format, arg...) pr_err( "%s: " format "\n", info.name, ## arg )

#endif // COMMON_H
