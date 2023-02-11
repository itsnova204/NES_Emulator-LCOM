#pragma once

// context information
typedef struct lcf_ctx_info_t {
  const char *file;   // filename
  const int line;     // line number
  const char *caller; // function from where it was called
  const char *code;   // "code" written in between parentheses for each argument (null separated)
} ctx_t;

// macro to ease capturing the content information
#ifdef __cplusplus
#  define CTX_INFO(code) \
    { __FILE__, __LINE__, __FUNCTION__, code "\0" }
#else
#  define CTX_INFO(code) \
    (const ctx_t) { __FILE__, __LINE__, __func__, code "\0" }
#endif
