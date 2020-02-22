#ifndef _M_UTIL_H__
#define _M_UTIL_H__

#define ERROR (-1)
#define OK (0)
#define RETURN_ERROR return -1
#define RETURN_OK return 0
#define CHECK_ERROR(err) do { if ((err) < 0) { printf("status: error"); return -1; } } while (0)
#define CHECK_NULL(ptr) do { if ((ptr) == NULL) return NULL; } while (0)
#define CHECK_NULL_ERROR(ptr) do { if ((ptr) == NULL) return -1; } while (0)
#define ASSERT_NOT_NULL(ptr) do { if ((ptr) == NULL) exit(-1); } while (0)

void m_strndup(char **dest, const char *src, size_t n);
void m_strdup(char **dest, const char *src);

unsigned int get_next_map_capacity(unsigned int cur);

#endif
