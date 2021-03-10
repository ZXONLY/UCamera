//
// Created by bytedance on 09/03/2021.
//
#pragma once

#include <unistd.h>

#include <cstdlib>
#include <pthread.h>

#include <sys/prctl.h>
typedef pid_t pthread_id_t;
#define THREAD_NONE -10000

static pthread_id_t getCurTid(){
    return gettid();
}