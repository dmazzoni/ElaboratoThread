#ifndef IO_UTILS_H
#define IO_UTILS_H

int read_line(int fd, char *const dest, const int max_length);
void write_results(const char *const pathname, int *results, int length);
void write_to_fd(int fd, const char *const s);
void write_with_int(int fd, const char *const s, int num);

#endif
