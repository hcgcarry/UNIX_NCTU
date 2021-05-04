#include "comm.h"
#include "Logger.h"
#include <stdarg.h>
#define ORIGIN_FUNC(return_type, func_name, param_type, param_name)             \
    {                                                                           \
        return_type(*originFunc) param_type = NULL;                             \
        void *handler = dlopen("libc.so.6", RTLD_LAZY);                         \
        if (handler != NULL)                                                    \
        {                                                                       \
            originFunc = (return_type(*) param_type)dlsym(handler, #func_name); \
            dlclose(handler);                                                   \
        }                                                                       \
        result = originFunc param_name;                                         \
    }

#define MODIFY_FUNC_0ARG(return_type, func_name)         \
    return_type func_name()                              \
    {                                                    \
        return_type result;                              \
        Logger loggerObj(#func_name);                    \
        ORIGIN_FUNC(return_type, func_name, (void), ()); \
        loggerObj.push_ret(#return_type, result);        \
        loggerObj.printLog();                            \
        return result;                                   \
    }
#define MODIFY_FUNC_1ARG(print_before, return_type, func_name, arg1_type, arg1) \
    return_type func_name(arg1_type arg1)                                       \
    {                                                                           \
        return_type result;                                                     \
        Logger loggerObj(#func_name);                                           \
        if (print_before)                                                       \
        {                                                                       \
            loggerObj.push_arg(#arg1_type, arg1);                               \
        }                                                                       \
        ORIGIN_FUNC(return_type, func_name, (arg1_type), (arg1));               \
        if (!print_before)                                                      \
        {                                                                       \
            loggerObj.push_arg(#arg1_type, arg1);                               \
        }                                                                       \
        loggerObj.push_ret(#return_type, result);                               \
        loggerObj.printLog();                                                   \
        return result;                                                          \
    }

#define MODIFY_FUNC_2ARG(print_before, return_type, func_name, arg1_type, arg1, arg2_type, arg2) \
    return_type func_name(arg1_type arg1, arg2_type arg2)                                        \
    {                                                                                            \
        return_type result;                                                                      \
        Logger loggerObj(#func_name);                                                            \
        if (#func_name == "rename")                                                              \
        {                                                                                        \
            loggerObj.push_arg(#arg1_type, arg1);                                                \
        }                                                                                        \
        else if (print_before)                                                                   \
        {                                                                                        \
            loggerObj.push_arg(#arg1_type, arg1);                                                \
            loggerObj.push_arg(#arg2_type, arg2);                                                \
        }                                                                                        \
        ORIGIN_FUNC(return_type, func_name, (arg1_type, arg2_type), (arg1, arg2));               \
        if (#func_name == "rename")                                                              \
        {                                                                                        \
            loggerObj.push_arg(#arg2_type, arg2);                                                \
        }                                                                                        \
        else if (!print_before)                                                                  \
        {                                                                                        \
            loggerObj.push_arg(#arg1_type, arg1);                                                \
            loggerObj.push_arg(#arg2_type, arg2);                                                \
        }                                                                                        \
        loggerObj.push_ret(#return_type, result);                                                \
        loggerObj.printLog();                                                                    \
        return result;                                                                           \
    }
#define MODIFY_FUNC_3ARG(print_before, return_type, func_name, arg1_type, arg1, arg2_type, arg2, arg3_type, arg3) \
    return_type func_name(arg1_type arg1, arg2_type arg2, arg3_type arg3)                                         \
    {                                                                                                             \
        return_type result;                                                                                       \
        Logger loggerObj(#func_name);                                                                             \
        if (print_before)                                                                                         \
        {                                                                                                         \
            loggerObj.push_arg(#arg1_type, arg1);                                                                 \
            loggerObj.push_arg(#arg2_type, arg2);                                                                 \
            loggerObj.push_arg(#arg3_type, arg3);                                                                 \
        }                                                                                                         \
        ORIGIN_FUNC(return_type, func_name, (arg1_type, arg2_type, arg3_type), (arg1, arg2, arg3));               \
        if (!print_before)                                                                                        \
        {                                                                                                         \
            loggerObj.push_arg(#arg1_type, arg1);                                                                 \
            loggerObj.push_arg(#arg2_type, arg2);                                                                 \
            loggerObj.push_arg(#arg3_type, arg3);                                                                 \
        }                                                                                                         \
        loggerObj.push_ret(#return_type, result);                                                                 \
        loggerObj.printLog();                                                                                     \
        return result;                                                                                            \
    }

#define MODIFY_FUNC_4ARG(print_before, return_type, func_name, arg1_type, arg1, arg2_type, arg2, arg3_type, arg3, arg4_type, arg4) \
    return_type func_name(arg1_type arg1, arg2_type arg2, arg3_type arg3, arg4_type arg4)                                          \
    {                                                                                                                              \
        return_type result;                                                                                                        \
        Logger loggerObj(#func_name);                                                                                              \
        if (print_before)                                                                                                          \
        {                                                                                                                          \
            loggerObj.push_arg(#arg1_type, arg1);                                                                                  \
            loggerObj.push_arg(#arg2_type, arg2);                                                                                  \
            loggerObj.push_arg(#arg3_type, arg3);                                                                                  \
            loggerObj.push_arg(#arg4_type, arg4);                                                                                  \
        }                                                                                                                          \
        ORIGIN_FUNC(return_type, func_name, (arg1_type, arg2_type, arg3_type, arg4_type), (arg1, arg2, arg3, arg4));               \
        if (!print_before)                                                                                                         \
        {                                                                                                                          \
            loggerObj.push_arg(#arg1_type, arg1);                                                                                  \
            loggerObj.push_arg(#arg2_type, arg2);                                                                                  \
            loggerObj.push_arg(#arg3_type, arg3);                                                                                  \
            loggerObj.push_arg(#arg4_type, arg4);                                                                                  \
        }                                                                                                                          \
        loggerObj.push_ret(#return_type, result);                                                                                  \
        loggerObj.printLog();                                                                                                      \
        return result;                                                                                                             \
    }

#define MODIFY_FUNC_OPEN(print_before, return_type, func_name, arg1_type, arg1, arg2_type, arg2, arg3_type, arg3) \
    return_type func_name(arg1_type arg1, arg2_type arg2, ...)                                                    \
    {                                                                                                             \
        return_type result;                                                                                       \
        Logger loggerObj(#func_name);                                                                             \
        arg3_type arg3;                                                                                           \
        if (arg2 & O_CREAT)                                                                                       \
        {                                                                                                         \
            va_list va;                                                                                           \
            va_start(va, arg2);                                                                                   \
            arg3 = va_arg(va, arg3_type);                                                                         \
            va_end(va);                                                                                           \
        }                                                                                                         \
         else                                                                                                   \
        {                                                                                                         \
             arg3 = 0;                                                                                           \
        }                                                                                                         \
        ORIGIN_FUNC(return_type, func_name, (arg1_type, arg2_type, arg3_type), (arg1, arg2, arg3));             \
        loggerObj.push_arg(#arg1_type, arg1);                                                                     \
        loggerObj.push_arg(#arg2_type, arg2);                                                                     \
        loggerObj.push_arg(#arg3_type, arg3);                                                                     \
        loggerObj.push_ret(#return_type, result);                                                                 \
        loggerObj.printLog();                                                                                     \
        return result;                                                                                            \
    }
