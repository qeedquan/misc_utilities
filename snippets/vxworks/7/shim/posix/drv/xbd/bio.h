#ifndef _BIO_H_
#define _BIO_H_

typedef long long sector_t;
typedef unsigned long block_t;
typedef unsigned long cookie_t;

#define NULLSECTOR -1L

enum {
	BIO_READ = 0x00000001,
	BIO_WRITE = 0x00000002,
	BIO_CACHE_BYPASS = 0x00000004,
	BIO_CACHE_FLUSH = 0x00000008,
	BIO_RELIABLE = 0x40000000,
};

struct bio {
	device_t bio_dev;
	sector_t bio_blkno;
	unsigned bio_bcount;
	void *bio_data;
	unsigned bio_resid;
	unsigned bio_error;
	unsigned bio_flags;
	int bio_priority;
	void (*bio_done)(struct bio *);
	void *bio_caller1;
	struct bio *bio_chain;
	void *bio_context;
};

#endif
