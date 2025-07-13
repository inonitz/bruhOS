#ifndef __KERNEL_C_DEFINITION_STD_ATOMIC_HEADER__
#define __KERNEL_C_DEFINITION_STD_ATOMIC_HEADER__
#include <std/int.h>
#include <std/enable_bool.h>
#include <std/bool.h>
#include <std/macro.h>


typedef int8_t   alignsz(sizeof(int8_t  )) atomic8_t  ; 
typedef int16_t  alignsz(sizeof(int16_t )) atomic16_t ; 
typedef int32_t  alignsz(sizeof(int32_t )) atomic32_t ; 
typedef int64_t  alignsz(sizeof(int64_t )) atomic64_t ; 
typedef uint8_t  alignsz(sizeof(uint8_t )) atomic8u_t ; 
typedef uint16_t alignsz(sizeof(uint16_t)) atomic16u_t; 
typedef uint32_t alignsz(sizeof(uint32_t)) atomic32u_t; 
typedef uint64_t alignsz(sizeof(uint64_t)) atomic64u_t; 


typedef enum __atomic_memory_model_values
{
    memory_model_relaxed = __ATOMIC_RELAXED,
    memory_model_consume = __ATOMIC_CONSUME,
    memory_model_acquire = __ATOMIC_ACQUIRE,
    memory_model_release = __ATOMIC_RELEASE,
    memory_model_acq_rel = __ATOMIC_ACQ_REL,
    memory_model_seq_cst = __ATOMIC_SEQ_CST  
} memory_model;


#define MEMORY_MODEL __ATOMIC_SEQ_CST


#define ATOMIC_SET_MACRO(ValueType, identifier_str) \
    static inline __force_inline void atomic_set_explicit_##identifier_str(ValueType* dest, ValueType* src, int32_t memOrder) \
    { \
        __atomic_load(src, dest, memOrder); \
    } \
\
\
    static inline __force_inline void atomic_set_##identifier_str(ValueType* dest, ValueType* src) \
    { \
        __atomic_load(src, dest, MEMORY_MODEL); \
    } \




#define ATOMIC_LOAD_MACRO(ValueType, identifier_str) \
    static inline __force_inline ValueType atomic_load_explicit_##identifier_str(ValueType* dest, int32_t memOrder) \
    { \
        return __atomic_load_n(dest, memOrder); \
    } \
\
\
    static inline __force_inline ValueType atomic_load_##identifier_str(ValueType* dest) \
    { \
        return __atomic_load_n(dest, MEMORY_MODEL); \
    } \




#define ATOMIC_STORE_MACRO(ValueType, identifier_str) \
    static inline __force_inline void atomic_store_explicit_##identifier_str(ValueType* dest, ValueType* src, int32_t memOrder) \
    { \
        __atomic_store(dest, src, memOrder); \
        return; \
    } \
\
\
    static inline __force_inline void atomic_store_##identifier_str(ValueType* dest, ValueType* src) \
    { \
        __atomic_store(dest, src, MEMORY_MODEL); \
        return; \
    } \




#define ATOMIC_EXCHANGE_MACRO(ValueType, identifier_str) \
    static inline __force_inline void atomic_exchange_explicit_##identifier_str(ValueType* dest, ValueType* src, ValueType* destPrevVal, int32_t memOrder) \
    { \
        __atomic_exchange(dest, src, destPrevVal, memOrder); \
        return; \
    } \
\
\
    static inline __force_inline void atomic_exchange_##identifier_str(ValueType* dest, ValueType* src, ValueType* destPrevVal) \
    { \
        __atomic_exchange(dest, src, destPrevVal, MEMORY_MODEL); \
        return; \
    } \
\
\
    static inline __force_inline ValueType atomic_exchange_ret_explicit_##identifier_str(ValueType* dest, ValueType src, int32_t memOrder) \
    { \
        /* destPrevVal is returned instead of being explicitly set with a pointer. */ \
        return __atomic_exchange_n(dest, src, memOrder); \
    } \
\
\
    static inline __force_inline ValueType atomic_exchange_ret_##identifier_str(ValueType* dest, ValueType src) \
    { \
        /* destPrevVal is returned instead of being explicitly set with a pointer. */ \
        return __atomic_exchange_n(dest, src, MEMORY_MODEL); \
    } \




/*  
    if cmpValue == expected:
        set cmpValue to setTo 
    else
        set expected to cmpValue 
*/
#define ATOMIC_CMP_XCHG_MACRO(ValueType, identifier_str) \
    static inline __force_inline bool_t atomic_cmpxchg_explicit_##identifier_str(     \
        ValueType* cmpValue,             \
        ValueType* expected,             \
        ValueType  setTo,                \
        int32_t    memOrderSuccess,      \
        int32_t    memOrderFail          \
    ) { \
        return __atomic_compare_exchange_n(cmpValue, expected, setTo, BOOLEAN_FALSE, memOrderSuccess, memOrderFail); \
    } \
\
\
    static inline __force_inline bool_t atomic_cmpxchg_##identifier_str(   \
        ValueType* cmpValue, \
        ValueType* expected, \
        ValueType  setTo     \
    ) { \
        /*  if cmpValue == expected: 
                set cmpValue to setTo 
            else
                set expected to cmpValue 
        */ \
        return __atomic_compare_exchange_n(cmpValue, expected, setTo, BOOLEAN_FALSE, MEMORY_MODEL, MEMORY_MODEL); \
    } \
\
\
    static inline __force_inline bool_t atomic_cmpxchg_ptr_explicit_##identifier_str( \
        ValueType* cmpValue,            \
        ValueType* expected,            \
        ValueType* setTo,               \
        int32_t    memOrderSuccess,     \
        int32_t    memOrderFail         \
    ) { \
        /* The same except that setTo isn't a variable, but a pointer to a variable that may set cmpValue */ \
        return __atomic_compare_exchange(cmpValue, expected, setTo, BOOLEAN_FALSE, memOrderSuccess, memOrderFail); \
    } \
\
\
    static inline __force_inline bool_t atomic_cmpxchg_ptr_##identifier_str( \
        ValueType* cmpValue, \
        ValueType* expected, \
        ValueType* setTo     \
    ) { \
        /* The same except that setTo isn't a variable, but a pointer to a variable that may set cmpValue */ \
        return __atomic_compare_exchange(cmpValue, expected, setTo, BOOLEAN_FALSE, MEMORY_MODEL, MEMORY_MODEL); \
    } \




#define ATOMIC_OPERATION_THEN_FETCH(OPERATION, ValueType, identifier_str) \
    /* Perform OPERATION on dest | return the value in dest => this is analogous to return_variable = ++counter */ \
    static inline __force_inline ValueType \
    \
    atomic_##OPERATION##_fetch_explicit_##identifier_str(ValueType* dest, ValueType src, int32_t memOrder) \
    { \
        return __atomic_##OPERATION##_fetch(dest, src, memOrder); \
    } \
\
\
    static inline __force_inline ValueType \
    \
    atomic_##OPERATION##_fetch_##identifier_str(ValueType* dest, ValueType src) \
    { \
        return __atomic_##OPERATION##_fetch(dest, src, MEMORY_MODEL); \
    } \




#define ATOMIC_FETCH_THEN_OPERATION(OPERATION, ValueType, identifier_str) \
    /* store ptr in var tmp | Perform OPERATION on dest | return the value in tmp | this is analogous to return_variable = counter++ */ \
    static inline __force_inline ValueType \
    \
    atomic_fetch_##OPERATION##_explicit_##identifier_str(ValueType* dest, ValueType src, int32_t memOrder) \
    { \
        return __atomic_fetch_##OPERATION(dest, src, memOrder); \
    } \
\
\
    static inline __force_inline ValueType \
    \
    atomic_fetch_##OPERATION##_##identifier_str(ValueType* dest, ValueType src) \
    { \
        return __atomic_fetch_##OPERATION(dest, src, MEMORY_MODEL); \
    } \




#define ATOMIC_TEST_SET_MACRO(ValueType, identifier_str) \
    static inline __force_inline bool_t atomic_test_and_set_explicit_##identifier_str(ValueType* ptr, int32_t memOrder) \
    { \
        /* 
            returns BOOLEAN_TRUE if ptr was set to some predefined constant (non-zero), during the atomic operation. 
            if ptr was set already, this will return BOOLEAN_FALSE 
        */ \
        return __atomic_test_and_set(ptr, memOrder); \
    } \
\
\
    static inline __force_inline bool_t atomic_test_and_set_##identifier_str(ValueType* ptr) \
    { \
        /* 
            returns BOOLEAN_TRUE if ptr was set to some predefined constant (non-zero), during the atomic operation. 
            if ptr was set already, this will return BOOLEAN_FALSE 
        */ \
        return __atomic_test_and_set(ptr, MEMORY_MODEL); \
    } \




#define ATOMIC_CLEAR_MACRO(ValueType, identifier_str) \
    static inline __force_inline void atomic_clear_explicit_##identifier_str(ValueType* ptr, int32_t memOrder) \
    { \
        /* sets the variable to 0 atomically */ \
        __atomic_clear(ptr, memOrder); \
        return; \
    } \
    static inline __force_inline void atomic_clear_##identifier_str(ValueType* ptr) \
    { \
        /* sets the variable to 0 atomically */ \
        __atomic_clear(ptr, MEMORY_MODEL); \
        return; \
    } \




#define ATOMIC_THREAD_FENCE() \
    static inline __force_inline void atomic_thread_fence_explicit(int32_t memOrder) \
    { \
        /* Used for synchronization between threads */ \
        __atomic_thread_fence(memOrder); \
        return; \
    } \
\
\
    static inline __force_inline void atomic_thread_fence(void) \
    { \
        __atomic_thread_fence(MEMORY_MODEL); \
        return; \
    } \




#define ATOMIC_SIGNAL_FENCE() \
    static inline __force_inline void atomic_signal_fence_explicit(int32_t memOrder) \
    { \
        /* Used for synchronization signal handlers in the same thread of execution */ \
        __atomic_signal_fence(memOrder); \
        return; \
    } \
\
\
    static inline __force_inline void atomic_signal_fence(void) \
    { \
        __atomic_signal_fence(MEMORY_MODEL); \
        return; \
    } \




#define MEMORY_PAUSE() \
    static inline __force_inline void pause(void) \
    { \
        __asm__ volatile("pause"); \
    } \




#define CREATE_ATOMIC_FUNCTION_WRAPPERS_TWO_ARGS(FUNCTION_NAME) \
    FUNCTION_NAME(atomic8u_t , u8 ) \
    FUNCTION_NAME(atomic16u_t, u16) \
    FUNCTION_NAME(atomic32u_t, u32) \
    FUNCTION_NAME(atomic64u_t, u64) \
    FUNCTION_NAME(atomic8_t  , s8 ) \
    FUNCTION_NAME(atomic16_t , s16) \
    FUNCTION_NAME(atomic32_t , s32) \
    FUNCTION_NAME(atomic64_t , s64) \




#define CREATE_ATOMIC_FUNCTION_WRAPPERS_THREE_ARGS(FUNCTION_NAME) \
    FUNCTION_NAME(add, atomic8u_t , u8 ) \
    FUNCTION_NAME(add, atomic16u_t, u16) \
    FUNCTION_NAME(add, atomic32u_t, u32) \
    FUNCTION_NAME(add, atomic64u_t, u64) \
    FUNCTION_NAME(add, atomic8_t  , s8 ) \
    FUNCTION_NAME(add, atomic16_t , s16) \
    FUNCTION_NAME(add, atomic32_t , s32) \
    FUNCTION_NAME(add, atomic64_t , s64) \
\
\
    FUNCTION_NAME(sub, atomic8u_t , u8 ) \
    FUNCTION_NAME(sub, atomic16u_t, u16) \
    FUNCTION_NAME(sub, atomic32u_t, u32) \
    FUNCTION_NAME(sub, atomic64u_t, u64) \
    FUNCTION_NAME(sub, atomic8_t  , s8 ) \
    FUNCTION_NAME(sub, atomic16_t , s16) \
    FUNCTION_NAME(sub, atomic32_t , s32) \
    FUNCTION_NAME(sub, atomic64_t , s64) \
\
\
    FUNCTION_NAME(and, atomic8u_t , u8 ) \
    FUNCTION_NAME(and, atomic16u_t, u16) \
    FUNCTION_NAME(and, atomic32u_t, u32) \
    FUNCTION_NAME(and, atomic64u_t, u64) \
    FUNCTION_NAME(and, atomic8_t  , s8 ) \
    FUNCTION_NAME(and, atomic16_t , s16) \
    FUNCTION_NAME(and, atomic32_t , s32) \
    FUNCTION_NAME(and, atomic64_t , s64) \
\
\
    FUNCTION_NAME(or, atomic8u_t , u8 ) \
    FUNCTION_NAME(or, atomic16u_t, u16) \
    FUNCTION_NAME(or, atomic32u_t, u32) \
    FUNCTION_NAME(or, atomic64u_t, u64) \
    FUNCTION_NAME(or, atomic8_t  , s8 ) \
    FUNCTION_NAME(or, atomic16_t , s16) \
    FUNCTION_NAME(or, atomic32_t , s32) \
    FUNCTION_NAME(or, atomic64_t , s64) \
\
\
    FUNCTION_NAME(nand, atomic8u_t , u8 ) \
    FUNCTION_NAME(nand, atomic16u_t, u16) \
    FUNCTION_NAME(nand, atomic32u_t, u32) \
    FUNCTION_NAME(nand, atomic64u_t, u64) \
    FUNCTION_NAME(nand, atomic8_t  , s8 ) \
    FUNCTION_NAME(nand, atomic16_t , s16) \
    FUNCTION_NAME(nand, atomic32_t , s32) \
    FUNCTION_NAME(nand, atomic64_t , s64) \




CREATE_ATOMIC_FUNCTION_WRAPPERS_TWO_ARGS(ATOMIC_SET_MACRO)
CREATE_ATOMIC_FUNCTION_WRAPPERS_TWO_ARGS(ATOMIC_LOAD_MACRO)
CREATE_ATOMIC_FUNCTION_WRAPPERS_TWO_ARGS(ATOMIC_STORE_MACRO)
CREATE_ATOMIC_FUNCTION_WRAPPERS_TWO_ARGS(ATOMIC_EXCHANGE_MACRO)
CREATE_ATOMIC_FUNCTION_WRAPPERS_TWO_ARGS(ATOMIC_CMP_XCHG_MACRO)
CREATE_ATOMIC_FUNCTION_WRAPPERS_TWO_ARGS(ATOMIC_CLEAR_MACRO)
CREATE_ATOMIC_FUNCTION_WRAPPERS_THREE_ARGS(ATOMIC_OPERATION_THEN_FETCH)
CREATE_ATOMIC_FUNCTION_WRAPPERS_THREE_ARGS(ATOMIC_FETCH_THEN_OPERATION)


ATOMIC_TEST_SET_MACRO(atomic8u_t, u8)
ATOMIC_TEST_SET_MACRO(atomic8_t,  s8)




ATOMIC_THREAD_FENCE()
ATOMIC_SIGNAL_FENCE()
MEMORY_PAUSE()

#endif
