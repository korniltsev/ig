//
// Created by anatoly.korniltsev on 21/06/2018.
//

#ifndef INSTASCRAMBLERCALL_SCRAMBLER_H
#define INSTASCRAMBLERCALL_SCRAMBLER_H

#include <dlfcn.h>
#include <stdio.h>

typedef int (*t_JNI_OnLoad)(int *unused);

typedef char *(*t_Scrambler_GetString)(void *gnuStringAppKey);

typedef void *(*t_String_Alllocator)();

typedef void (*t_String_Constructor)(void *thiz, const char *data, void *allocator);

#define IG_KEY "fb26667d85c4432ee34e8e69876575a2"


void log_dl_error();

char *scrambler_get_string(const char *key);
#endif //INSTASCRAMBLERCALL_SCRAMBLER_H
