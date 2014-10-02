#ifndef EXAMPLE_H
#define EXAMPLE_H

struct example_status {
	int status;
};

#define THERMIOC_MAGIC			'E'
#define EXAMPLE_SET_STATUS		_IOW(THERMIOC_MAGIC, 0, struct example_status)
#define EXAMPLE_GET_STATUS		_IOR(THERMIOC_MAGIC, 1, struct example_status)

#endif // EXAMPLE_H
