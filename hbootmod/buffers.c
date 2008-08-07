#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include "hboot.h"
#include "crc32.h"

#define GOOD_ADDRESS(p)		    (__pa(p) >= 0x92000000)

#define CHUNK_SIZE_ORDER	    0
#define PAGES_PER_CHUNK		    (1 << CHUNK_SIZE_ORDER)
#define CHUNK_SIZE		    (PAGE_SIZE*PAGES_PER_CHUNK)

#define MAX_BUFFERS_COUNT	    8
#pragma pack(push, 1)
struct plain_buffer {
	uint32_t size;
	void *data;
};
struct scattered_buffer {
	uint32_t size;
	uint32_t checksum;
	uint32_t tag;
	uint32_t chunk_size;
	uint32_t allocated_chunks;
	uint32_t chunks[0];
};
#pragma pack(pop)

struct generic_buffer {
	union {
		struct plain_buffer *plain;
		struct scattered_buffer *scattered;
		void *generic;
	} container;
	int type;
	spinlock_t buf_lock;
};

struct buffers_ctx {
	spinlock_t ctx_lock;
	int used;
	int handle;
	void *bootlist;
	struct generic_buffer bufs[MAX_BUFFERS_COUNT];
};

struct buffers_ctx buffers;

static struct generic_buffer *__get_buffer_nolock(int handle) {
	struct generic_buffer *buf;

	if ((handle < 0) || (handle > buffers.used) || (buffers.bufs[handle].container.generic == NULL)) {
		return NULL;
	}
	buf = &buffers.bufs[handle];
	spin_lock(&buf->buf_lock);
	return buf;
}

static struct generic_buffer *get_buffer(int handle) {
	struct generic_buffer *ret;

	spin_lock(&buffers.ctx_lock);
	ret = __get_buffer_nolock(handle);
	spin_unlock(&buffers.ctx_lock);
	return ret;
}

static void put_buffer(struct generic_buffer *buf) {
	if (buf) {
		spin_unlock(&buf->buf_lock);
	}
}

static int get_new_buffer(struct generic_buffer **pbuf) {
	int handle;

	spin_lock(&buffers.ctx_lock);
	if (buffers.used > 0 && (spin_trylock(&buffers.bufs[buffers.used-1].buf_lock))) {
		if (buffers.bufs[buffers.used-1].container.generic == NULL) {
			buffers.used--;
		}
		spin_unlock(&buffers.bufs[buffers.used-1].buf_lock);
	}
	if (buffers.used >= MAX_BUFFERS_COUNT-1) {
		spin_unlock(&buffers.ctx_lock);
		return INVALID_BUFFER_HANDLE;
	}
	handle = buffers.used++;
	*pbuf = &buffers.bufs[handle];
	(*pbuf)->buf_lock = SPIN_LOCK_UNLOCKED;
	(*pbuf)->container.generic = NULL;
	spin_unlock(&buffers.ctx_lock);
	spin_lock(&(*pbuf)->buf_lock);
	return handle;
}

uint32_t get_high_mem(size_t size) {
	uint32_t p;
	while (1) {
		p = (uint32_t)kmalloc(size, GFP_KERNEL);
		if (p == 0) {
			return 0;
		} else if (GOOD_ADDRESS(p)) {
			return p;
		}
	}
}

void free_high_mem(void *ptr) {
	kfree(ptr);
}

uint32_t get_high_pages(unsigned int order) {
	uint32_t p;
	while (1) {
		p = (uint32_t)__get_free_pages(GFP_KERNEL, order);
		if (p == 0) {
			return 0;
		} else if (GOOD_ADDRESS(p)) {
			return p;
		}
	}
}

void free_high_pages(void *p, unsigned int order) {
	free_pages((unsigned long)p, order);
}

uint32_t get_high_page(void) {
	return get_high_pages(0);
}

void free_high_page(void *p) {
	return free_high_pages(p, 0);
}

static void free_scattered_buffer(struct scattered_buffer *sc) {
	uint32_t i;
	for (i = 0; i < sc->allocated_chunks; ++i) {
		free_high_pages((void*)sc->chunks[i], CHUNK_SIZE_ORDER);
	}
	free_high_mem((void*)sc);
}

static struct scattered_buffer *allocate_scattered_buffer(uint32_t bufsize, uint32_t tag) {
	struct scattered_buffer *sc;
	uint32_t p;
	size_t sc_size;

	if (bufsize == 0) {
		return NULL;
	}

	sc_size = sizeof(struct scattered_buffer) + 4*((bufsize / CHUNK_SIZE) + (bufsize % CHUNK_SIZE ? 1 : 0));
	sc = (struct scattered_buffer*)get_high_mem(sc_size);
	if (sc == NULL) {
		return NULL;
	}

	sc->size = bufsize;
	crc32_init_ctx(&sc->checksum);
	sc->allocated_chunks = 0;
	sc->chunk_size = CHUNK_SIZE;
	sc->tag = tag;
	while (bufsize > 0) {
		p = get_high_pages(CHUNK_SIZE_ORDER);
		if (p == 0) {
			goto err;
		}
		sc->chunks[sc->allocated_chunks++] = p;
		if (bufsize < sc->chunk_size) {
			bufsize = 0;
		} else {
			bufsize -= sc->chunk_size;
		}
	}
	return sc;
err:
	free_scattered_buffer(sc);
	return NULL;
}

static int append_scattered_buffer(struct scattered_buffer *sc, const char __user *data, size_t size, loff_t pos, uint32_t *checksum) {
	size_t written = 0;
	size_t cur_size = 0;
	int chunk;
	int chunk_off;

	if (pos >= sc->size) {
		return -EINVAL;
	}
	if (sc->size < pos + size) {
		size = sc->size - pos;
	}

	chunk = (uint32_t)pos / sc->chunk_size;
	chunk_off = (uint32_t)pos % sc->chunk_size;
	while (written < size) {
		cur_size = min(size, sc->chunk_size - chunk_off);
		copy_from_user((char*)sc->chunks[chunk] + chunk_off, data + written, cur_size);
		if (checksum) {
			crc32_update(checksum, (const uint8_t*)sc->chunks[chunk] + chunk_off, cur_size);
		}
		written += cur_size;
		chunk++;
		chunk_off = 0;
	}
	return written;
}
	
static struct plain_buffer *allocate_plain_buffer(uint32_t bufsize, uint32_t tag) {
	struct plain_buffer *pb;

	pb = (struct plain_buffer*)get_high_mem(sizeof(struct plain_buffer));
	if (pb == NULL) {
		return NULL;
	}
	pb->size = bufsize;
	pb->data = (void*)get_high_mem(bufsize);
	if (pb->data == 0) {
		free_high_mem(pb);
		return NULL;
	}
	return pb;
}

static void free_plain_buffer(struct plain_buffer *pb) {
	free_high_mem(pb->data);
	free_high_mem(pb);
}

static int append_plain_buffer(struct plain_buffer *pb, const char __user *data, size_t size, loff_t pos, uint32_t *checksum) {
	(void)checksum;

	if (pos >= pb->size) {
		return -EINVAL;
	}
	if (pb->size < pos + size) {
		size = pb->size - pos;
	}
	copy_from_user((char*)pb->data + pos, data, size);
	return (int)size;
}

void free_typed_buffer(void *buffer, int type) {
	switch (type) {
		case BUFFER_PLAIN:
			free_plain_buffer((struct plain_buffer*)buffer);
			break;
		case BUFFER_SCATTERED:
			free_scattered_buffer((struct scattered_buffer*)buffer);
			break;
	}
}
int allocate_buffer(int type, uint32_t bufsize, uint32_t tag) {
	struct generic_buffer *buf;
	void *data;
	int handle;

	switch (type) {
		case BUFFER_PLAIN:
			data = allocate_plain_buffer(bufsize, tag);
			break;
		case BUFFER_SCATTERED:
			data = allocate_scattered_buffer(bufsize, tag);
			break;
		default:
			data = NULL;
			break;
	}
	if (data == NULL) {
		return INVALID_BUFFER_HANDLE;
	}
	handle = get_new_buffer(&buf);
	if (handle == INVALID_BUFFER_HANDLE) {
		free_typed_buffer(data, type);
		return INVALID_BUFFER_HANDLE;
	}
	buf->type = type;
	buf->container.generic = data;
	put_buffer(buf);
	printk("%s buffer of size %u allocated\n", (type == BUFFER_PLAIN?"plain":"scattered"), bufsize);
	return handle;
}

int free_buffer(int handle) {
	struct generic_buffer *buf;
	int type;
	void *data;

	buf = get_buffer(handle);
	if (buf == NULL) {
		return -1;
	}
	data = buf->container.generic;
	type = buf->type;
	buf->container.generic = NULL;
	put_buffer(buf);

	free_typed_buffer(data, type);
	return 0;
}

int select_buffer(int handle) {
	spin_lock(&buffers.ctx_lock);
	buffers.handle = handle;
	spin_unlock(&buffers.ctx_lock);
	return 0;
}

int buffer_append_userdata(const char __user *data, size_t size, loff_t *ppos) {
	struct generic_buffer *buf;
	int ret;

	buf = get_buffer(buffers.handle);
	if (buf == NULL) {
		return -EINVAL;
	}
	switch (buf->type) {
		case BUFFER_PLAIN:
			ret = append_plain_buffer(buf->container.plain, data, size, *ppos, NULL);
			break;
		case BUFFER_SCATTERED:
			ret = append_scattered_buffer(buf->container.scattered, data, size, *ppos, &buf->container.scattered->checksum);
			break;
		default:
			ret = -EINVAL;
			break;
	}
	put_buffer(buf);
	if (ret > 0) {
		*ppos += ret;
	}
	return ret;
}

bootfunc_t get_bootentry(uint32_t *bootsize, int handle) {
	struct generic_buffer *buf;
	bootfunc_t ret;

	buf = get_buffer(handle);
	if (buf == NULL) {
		return NULL;
	}
	if (buf->type != BUFFER_PLAIN ||
	    buf->container.plain->size < 4 ||
	    ((uint32_t)buf->container.plain->data) & 0x3) {
		ret =  NULL;
	} else {
		ret = (bootfunc_t)buf->container.plain->data;
		*bootsize = buf->container.plain->size;
	}
	put_buffer(buf);
	return ret;
}

void *get_bootlist(uint32_t *listsize) {
	void **list;
	int i, j = 0;

	spin_lock(&buffers.ctx_lock);
	list = (void**)buffers.bootlist;
	for (i = 0; i < buffers.used; ++i) {
		struct generic_buffer *buf;

		buf = &buffers.bufs[i];
		if (buf->type == BUFFER_SCATTERED && buf->container.generic != NULL) {
			crc32_final(&buf->container.scattered->checksum);
			list[j++] = buf->container.generic;
		}
	}
	*listsize = j;
	spin_unlock(&buffers.ctx_lock);
	return (void*)list;
}

int buffers_init(void) {
	buffers.ctx_lock = SPIN_LOCK_UNLOCKED;
	buffers.bootlist = (void*)get_high_mem(MAX_BUFFERS_COUNT*sizeof(void*));
	if (buffers.bootlist == NULL) {
		return -1;
	}
	return 0;
}

void buffers_destroy(void) {
	if (buffers.bootlist) {
		free_high_mem(buffers.bootlist);
	}
}
