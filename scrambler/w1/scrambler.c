
#include <dlfcn.h>
#include <stdio.h>
#include <malloc.h>
#include "scrambler.h"


char *scrambler_get_string(const char *key) {
    //region dlopen
    void *h_scrambler = dlopen("libscrambler.so", RTLD_NOW);
    if (h_scrambler == NULL) {
        log_dl_error();
        return 0;
    }
    void *h_stl = dlopen("libgnustl_shared.so", RTLD_NOW);
    if (h_stl == NULL) {
        log_dl_error();
        return 0;
    }
    //endregion

    //region dlsym
    t_JNI_OnLoad JNI_OnLoad = dlsym(h_scrambler, "JNI_OnLoad");
    if (JNI_OnLoad == NULL) {
        log_dl_error();
        return 0;
    }
    t_Scrambler_GetString Scrambler_GetString = dlsym(h_scrambler, "_ZN9Scrambler9getStringESs");
    if (Scrambler_GetString == NULL) {
        log_dl_error();
        return 0;
    }
    t_String_Alllocator String_Alllocator = dlsym(h_stl, "_ZNSaIcEC1Ev");
    if (String_Alllocator == NULL) {
        log_dl_error();
        return 0;
    }
    t_String_Constructor String_Constructor = dlsym(h_stl, "_ZNSsC2EPKcRKSaIcE");
    if (String_Constructor == NULL) {
        log_dl_error();
        return 0;
    }
    //endregion

    //region Scrambler::GetString(std::string const)
    JNI_OnLoad(NULL);
    void *allocatorInstance = String_Alllocator();
    void *appKeyStlString = malloc(12);
    String_Constructor(appKeyStlString, key, allocatorInstance);
    char *res = Scrambler_GetString(appKeyStlString);
    free(appKeyStlString);

    dlclose(h_scrambler);
    dlclose(h_stl);
    return res;
    //endregion
}

void log_dl_error() {
    char *e = dlerror();
    if (e != NULL) {
        printf("dlerror: %s", e);
    }
}
