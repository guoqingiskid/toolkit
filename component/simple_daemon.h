//
// Created by kid1412 on 19-1-12.
//

#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include "daemon_init.h"

int daemon_init()
{
    pid_t  pid = fork();   //1.为了子进程时后台进程，且不是进程组组长
    if(pid < 0)
        exit(1);
    else if(pid)
        exit(0);

    if(setsid() < 0)           //子进程可以设置新的会话，从而摆脱远原来会话的控制终端的关联
        exit(1);

    signal(SIGHUP,SIG_IGN);   //忽略掉SIGHUP信号，从而在摆脱终端关联时保证下面的子进程不会被默认给退出，注意信号设置是会被子进程继承的。

    if((pid = fork()) < 0)    //为了防止当前的进程开启一个终端，因为当前会话的会话头进程是自己，且也是新进程组的组长，如果开启了终端，会话会将该进程设置为控制终端的控制进程，那么
							  //之前摆脱的关联就又回来了，因此再fork一个子进程，新的子进程就一定不是会话的头进程，因此该会话就不会将新子进程设置为控制进程。	
        exit(1);
    else if(pid)  
        exit(0);

    chdir("/"); 			 //改变下工作路径，通常daemon进程都这样！！！

    for(int idx = 0; idx < 64; ++idx)
        close(idx);          //这里通常关闭这些描述符，尤其是与终端相关的0 1 2 ，因为如果向这些描述符写数据会发生错误，因为没有与终端关联的缘故。

    open("/dev/null",O_RDONLY);  //这里打开它们是应该的，这样会避免我们忘记它们已经被关闭了，且后续打开的描述符不会占用到0 1 2。因为这三个描述符我们太熟悉了如果被其他占用那么容易忘记出错
    open("dev/null",O_RDWR);
    open("dev/null",O_RDWR);

    return 0;

}