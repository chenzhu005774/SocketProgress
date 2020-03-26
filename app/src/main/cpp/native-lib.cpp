#include <jni.h>
#include <string>

#include "native-lib.h"


int m_client_fd;

const char *PATH ="/data/data/com.chenzhu.socketprogress/my.socket";
const char *userId;
extern "C"
JNIEXPORT void JNICALL
Java_com_chenzhu_socketprogress_Watcher_createWatcher(JNIEnv *env, jobject instance,  jstring userId_) {
    userId = env->GetStringUTFChars(userId_, 0);
    LOGD("userId %s", userId);
    //fork 一个新的进程
    pid_t pid = fork();
    //新建进程失败
    if (pid < 0) {

    }
        //子进程
    else if (pid == 0) {
        child_do_work();
    }
        //父进程返回新建的子进程的pid
    else if (pid > 0) {}
    env->ReleaseStringUTFChars(userId_, userId);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_chenzhu_socketprogress_Watcher_connectMonitor(JNIEnv *env, jobject instance) {
    int socketfd;
    struct sockaddr_un addr;
    while (1) {
        socketfd = socket(AF_LOCAL, SOCK_STREAM, 0);
        if (socketfd < 0) {
            LOGD("客户端新建创建失败");
            return;
        }
        memset(&addr, 0, sizeof(sockaddr_un));
        addr.sun_family = AF_LOCAL;
        strcpy(addr.sun_path, PATH);
        if (connect(socketfd, reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un))) {
            LOGD("客户端连接失败");
            close(socketfd);
            sleep(1);
            continue;
        }
        LOGD("客户端连接成功");
        break;
    }

}


void child_do_work(){
 //开启socket 为服务端

    //如果创建成功那么就去读写
  if(child_create_channel()){
      child_listen_meg();
  }


}




int child_create_channel() {
    LOGD("开始创建服务端socket");
    //AF_LOCAL:本地类型socket，SOCK_STREAM：面向tcp的流，参数0，代表自己选择网络协议
    int sfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    unlink(PATH);
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(sockaddr_un));
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, PATH);
    if (bind(sfd, reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr_un)) < 0) {
        LOGD("服务端绑定socket出错");
        return 0;
    }
    //5:最大连接数量，实际上由系统决定，
    listen(sfd, 5);
    int coonfd = 0;
    //监听客户端的连接
    while (1) {
        //没有客户端请求到来，会阻塞直到一个请求的到来，返回客户端地址
        if ((coonfd = accept(sfd, NULL, NULL)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                LOGD("服务端 accept 出错");
                return 0;
            }
        }
        m_client_fd = coonfd;
        LOGD("服务端连接成功，客户端的 fd %d", m_client_fd);
        break;
    }
    return 1;
}

void child_listen_meg() {
    LOGD("服务端监听客户端的消息");
    fd_set fds;
    struct timeval timeout = {10, 0};
    while (1) {
        //使用select I/O多路复用
        //所有位置0
        FD_ZERO(&fds);
        //指定位上的值置为1,
        FD_SET(m_client_fd, &fds);
        //最长等待时间10s，timeout为null，表示一直阻塞直到收到客户端的消息
        int r = select(m_client_fd + 1, &fds, NULL, NULL, &timeout);
//        LOGD("读取消息前 select %d", r);
        if (r > 0) {
            char pkg[256] = {0};
            if (FD_ISSET(m_client_fd, &fds)) {
                //如果阻塞结束，说明客户端进程被kill
                int result = read(m_client_fd, pkg, sizeof(pkg));
                LOGD("服务端开始重启服务");

                //api<17
                /*execlp("am", "am", "startservice", "-n"
                               "com.example.chen.servicealivetest/com.example.chen.servicealivetest.KeepService",
                       (char *) NULL);*/

                //>=api17  不过好像两种 api 这种方式都能启动服务
                //执行linux命令：这里是开启一个service
                //am startservice -n com.example.chen.servicealivetest/com.example.chen.servicealivetest.KeepService
                //execlp()会从PATH 环境变量所指的目录中查找符合参数file的文件名, 找到后便执行该文件,
                //然后将第二个以后的参数当做该文件的argv[0]、argv[1]……, 最后一个参数必须用空指针(NULL)作结束。
                execlp("am", "am", "startservice", "--user", userId, "-n"
                               "com.chenzhu.socketprogress/com.chenzhu.socketprogress.ProgressService",
                       (char *) NULL);
                break;
            }
        }
    }
}

