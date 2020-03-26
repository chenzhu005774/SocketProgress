//
// Created by Administrator on 2020/3/25 0025.

#ifndef SOCKETPROGRESS_NATIVE_LIB_H
#define SOCKETPROGRESS_NATIVE_LIB_H

#endif //SOCKETPROGRESS_NATIVE_LIB_H

#include <sys/select.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <android/log.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <stdlib.h>
#include <linux/signal.h>
#include <android/log.h>
#include <string.h>

#define SUCCESS 0;
#define FAIL -1;
#define LOG_TAG "chenzhu"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG ,__VA_ARGS__)


void child_do_work();
int child_create_channel();
void child_listen_meg();