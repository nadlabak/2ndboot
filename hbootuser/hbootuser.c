#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "../hbootmod/hboot.h"

#define MAX_DESCS 8
int ctrlfd;
struct desc {
	int fd;
	uint32_t tag;
	size_t size;
	int buffer;
};

int open_ctrl() {
	ctrlfd = open("/dev/hbootctrl", O_RDWR);
	if (ctrlfd < 0) {
		perror("open");
	}
	return ctrlfd;
}
size_t fdsize(int fd) {
	off_t seek;

	seek = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	return (size_t)seek;
}
int try_file(const char *path, size_t *size) {
	int fd;

	if (access(path, R_OK) != 0) {
		perror("access");
		return -1;
	}
	if ((fd = open(path, O_RDONLY)) < 0) {
		perror("open");
		return fd;
	}
	if ((*size = fdsize(fd)) == (size_t)-1) {
		perror("fdsize");
		close(fd);
		return -1;
	}
	return fd;
}
int allocate_buffer(int type, size_t size, uint32_t tag) {
	struct hboot_buffer_req req = {type, size, tag};
	int ret;
	if ((ret = ioctl(ctrlfd, HBOOT_ALLOCATE_BUFFER, &req)) < 0) {
		perror("ioctl");
	}
	return ret;
}
int free_buffer(int buffer) {
	int ret;

	if ((ret = ioctl(ctrlfd, HBOOT_FREE_BUFFER, buffer)) < 0) {
		perror("ioctl");
	}
	return ret;
}
int select_buffer(int buffer) {
	int ret;

	if ((ret = ioctl(ctrlfd, HBOOT_SELECT_BUFFER, buffer)) < 0) {
		perror("ioctl");
	}
	return ret;
}
int fd2fd(int in, size_t size, int out) {
	char buf[4096];
	size_t written = 0;
	int ret;
	while ((written < size) && ((ret = read(in, buf, sizeof buf)) > 0)) {
		if (written + ret > size) {
			ret = size - written;
		}
		if (write(out, buf, ret) < ret) {
			fprintf(stderr, "invalid write\n");
			return -1;
		}
		written += ret;
	}
	return 0;
}
int load_desc(struct desc *d) {
	if (select_buffer(d->buffer) < 0) {
		return -1;
	}
	return fd2fd(d->fd, d->size, ctrlfd);
}

void boot(int handle) {
	ioctl(ctrlfd, HBOOT_BOOT, handle);
}

int main(int argc, char **argv) {
	FILE *fp;
	uint32_t tag;
	char fname[1024];
	struct desc descs[MAX_DESCS], *loader = NULL;
	int descs_cnt = 0;
	int i;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <descriptor>\n", argv[0]);
		return 1;
	}
	if (open_ctrl() < 0) {
		fprintf(stderr, "Failed to open device\n");
		return 1;
	}
	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		fprintf(stderr, "fopen");
		return 1;
	}
	while ((fscanf(fp, "%s %u", fname, &tag) == 2) && (descs_cnt < MAX_DESCS)) {
		int fd;
		size_t size;

		fd = try_file(fname, &size);
		if (fd < 0) {
			goto out;
		}

		descs[descs_cnt].fd = fd;
		descs[descs_cnt].tag = tag;
		descs[descs_cnt].size = size;
		descs[descs_cnt].buffer = -1;

		descs_cnt++;
	}

	for (i = 0; i < descs_cnt; ++i) {
		int type;
		if (descs[i].tag == 0) {
			type = BUFFER_PLAIN;
			loader = &descs[i];
		} else {
			type = BUFFER_SCATTERED;
		}
		if ((descs[i].buffer = allocate_buffer(type, descs[i].size, descs[i].tag)) < 0) {
			fprintf(stderr, "Failed to alloc buffer\n");
			goto out;
		}
	}
	fprintf(stderr, "Buffers allocated\n");
	if (loader == NULL) {
		fprintf(stderr, "Sorry, no loader provided\n");
		goto out;
	}
	for (i = 0; i < descs_cnt; ++i) {
		if (load_desc(&descs[i]) < 0) {
			fprintf(stderr, "Failed to load file\n");
			goto out;
		}
	}
	printf("Everythin is loaded, booting\n");
	fflush(stdout);
	boot(loader->buffer);
out:
	if (fp) fclose(fp);
	printf("buffers: %d\n", descs_cnt);
	while (descs_cnt-- > 0) {
		printf("%d\n", descs[descs_cnt].buffer);
		close(descs[descs_cnt].fd);
		if (descs[descs_cnt].buffer >= 0) free_buffer(descs[descs_cnt].buffer);
	}
	return 1;
}
