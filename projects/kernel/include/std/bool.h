#ifndef __KERNEL_C_DEFINITION_STD_BOOL_HEADER__
#define __KERNEL_C_DEFINITION_STD_BOOL_HEADER__

#ifdef __KERNEL_BOOL_IS_UNSIGNED_CHAR_TYPE
    typedef unsigned char bool_t;
#endif
#ifndef __KERNEL_BOOL_FALSE_VALUE
#   define __KERNEL_BOOL_FALSE_VALUE
#   define bool_false (0)
#endif
#ifndef __KERNEL_BOOL_TRUE_VALUE
#   define __KERNEL_BOOL_TRUE_VALUE
#   define bool_true (1)
#endif

#endif /* __KERNEL_C_DEFINITION_STD_BOOL_HEADER__ */