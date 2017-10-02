#pragma once

#include "Define.h"
#include "common.h"

_EXTERN_C_ char * GetFileInfoList();

_EXTERN_C_ char * ClearItem(char * id);

_EXTERN_C_ int ComputeDirSize(char *id);

_EXTERN_C_ char * GetTargetPath();

_EXTERN_C_ void SetTargetPath(char * path);