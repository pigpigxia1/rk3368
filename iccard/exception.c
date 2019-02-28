#include "exception.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define LOG_ERR "not open log\r\n"
static int cur_position = 0;

Exception_frame *Exception_stack = NULL;

void exception_raise(const Exception_t *e,const char *file,int line){
    Exception_frame *p = Exception_stack;

    assert(e);
    if(p == NULL){
        abort_without_exception(e,file,line);
    }

    p->exception = e;
    p->file = file;
    p->line = line;
    Exception_stack = Exception_stack->prev;
    longjmp(p->env,EXCEPTION_RAISED);
}

void abort_without_exception(const Exception_t *e,const char *file,int line){
    //sprintf(buff,"%s",stderr,"Uncaught exception");
    char buff[512];
    if(e->reason)
        sprintf(buff," %s",e->reason);
    else
        sprintf(buff,"at 0x%p",e);
    if(file && line > 0)
        sprintf(buff,"raised at %s:%d\n",file,line);
    sprintf(buff,"%s","aborting...\n");
    fflush(stderr);
    abort();
}

void handle_proc_sig(int signo){

    char buff[512];
    if( signo == MANPROCSIG_HUP )
        sprintf(buff,"signo = %d:%s",signo," Hangup (POSIX). \r\n");
    else if( signo == MANPROCSIG_INT )
        sprintf(buff,"signo = %d:%s",signo," Interrupt (ANSI). \r\n");
    else if( signo == MANPROCSIG_QUIT )
        sprintf(buff,"signo = %d:%s",signo," Quit (POSIX). \r\n");
    else if( signo == MANPROCSIG_ILL )
        sprintf(buff,"signo = %d:%s",signo," Illegal instruction (ANSI). \r\n");
    else if( signo == MANPROCSIG_TRAP )
        sprintf(buff,"signo = %d:%s",signo," Trace trap (POSIX). \r\n");
    else if( signo == MANPROCSIG_ABRT )
        sprintf(buff,"signo = %d:%s",signo," Abort (ANSI). \r\n");
    else if( signo == MANPROCSIG_IOT )
        sprintf(buff,"signo = %d:%s",signo," IOT trap (4.2 BSD). \r\n");
    else if( signo == MANPROCSIG_BUS )
        sprintf(buff,"signo = %d:%s",signo," BUS error (4.2 BSD). \r\n");
    else if( signo == MANPROCSIG_FPE )
        sprintf(buff,"signo = %d:%s",signo," Floating-point exception (ANSI). \r\n");
    else if( signo == MANPROCSIG_KILL )
        sprintf(buff,"signo = %d:%s",signo," Kill, unblockable (POSIX). \r\n");
    else if( signo == MANPROCSIG_USR1 )
        sprintf(buff,"signo = %d:%s",signo," User-defined signal if( signo == (POSIX). \r\n");
    else if( signo == MANPROCSIG_SEGV )
        sprintf(buff,"signo = %d:%s",signo," Segmentation violation (ANSI). \r\n");
    else if( signo == MANPROCSIG_USR2 )
        sprintf(buff,"signo = %d:%s",signo," User-defined signal 2 (POSIX). \r\n");
    else if( signo == MANPROCSIG_PIPE )
        sprintf(buff,"signo = %d:%s",signo," Broken pipe (POSIX). \r\n");
    else if( signo == MANPROCSIG_ALRM )
        sprintf(buff,"signo = %d:%s",signo," Alarm clock (POSIX). \r\n");
    else if( signo == MANPROCSIG_TERM )
        sprintf(buff,"signo = %d:%s",signo," Termination (ANSI). \r\n");
    else if( signo == MANPROCSIG_STKFLT )
        sprintf(buff,"signo = %d:%s",signo," Stack fault. \r\n");
    else if( signo == MANPROCSIG_CLD )
        sprintf(buff,"signo = %d:%s",signo," Same as SIGCHLD (System V). \r\n");
    else if( signo == MANPROCSIG_CHLD )
        sprintf(buff,"signo = %d:%s",signo," Child status has changed (POSIX). \r\n");
    else if( signo == MANPROCSIG_CONT )
        sprintf(buff,"signo = %d:%s",signo," Continue (POSIX). \r\n");
    else if( signo == MANPROCSIG_STOP )
        sprintf(buff,"signo = %d:%s",signo," Stop, unblockable (POSIX). \r\n");
    else if( signo == MANPROCSIG_TSTP )
        sprintf(buff,"signo = %d:%s",signo," Keyboard stop (POSIX). \r\n");
    else if( signo == MANPROCSIG_TTIN )
        sprintf(buff,"signo = %d:%s",signo," Background read from tty (POSIX). \r\n");
    else if( signo == MANPROCSIG_TTOU )
        sprintf(buff,"signo = %d:%s",signo," Background write to tty (POSIX). \r\n");
    else if( signo == MANPROCSIG_URG )
        sprintf(buff,"signo = %d:%s",signo," Urgent condition on socket (4.2 BSD). \r\n");
    else if( signo == MANPROCSIG_XCPU )
        sprintf(buff,"signo = %d:%s",signo," CPU limit exceeded (4.2 BSD). \r\n");
    else if( signo == MANPROCSIG_XFSZ )
        sprintf(buff,"signo = %d:%s",signo," File size limit exceeded (4.2 BSD). \r\n");
    else if( signo == MANPROCSIG_VTALRM )
        sprintf(buff,"signo = %d:%s",signo," Virtual alarm clock (4.2 BSD). \r\n");
    else if( signo == MANPROCSIG_PROF )
        sprintf(buff,"signo = %d:%s",signo," Profiling alarm clock (4.2 BSD). \r\n");
    else if( signo == MANPROCSIG_WINCH )
        sprintf(buff,"signo = %d:%s",signo," Window size change (4.3 BSD, Sun). \r\n");
    else if( signo == MANPROCSIG_POLL )
        sprintf(buff,"signo = %d:%s",signo," Pollable event occurred (System V). \r\n");
    else if( signo == MANPROCSIG_IO )
        sprintf(buff,"signo = %d:%s",signo," I/O now possible (4.2 BSD). \r\n");
    else if( signo == MANPROCSIG_PWR )
        sprintf(buff,"signo = %d:%s",signo," Power failure restart (System V). \r\n");
    else if( signo == MANPROCSIG_SYS)
        sprintf(buff,"signo = %d:%s",signo," Bad system call. \r\n");
    else if( signo == MANPROCSIG_UNUSED)
        sprintf(buff,"signo = %d:%s",signo," Unknow erroe. \r\n");

    log_write(buff);
    Exception_frame *p = Exception_stack;
    Exception_stack = Exception_stack->prev;
    longjmp(p->env,signo);
    // exit(0);//exit process
}


void log_write(unsigned char *str)
{
#ifdef ENABLE_LOG
	char log_cache[512];
	time_t nowT ;
	int log_fd = open(LOG_FILE,O_CREAT|O_RDWR,S_IRWXU|S_IRWXG|S_IRWXO);
	
	nowT = time(0);
	strftime(log_cache, sizeof(log_cache), "%Y-%m-%d %H:%M:%S:", localtime(&nowT));
	lseek(log_fd,cur_position,SEEK_SET);
	//write(log_fd,log_cache,strlen(log_cache)); 
	sprintf(log_cache,"%s%s\r\n",log_cache,str);
	write(log_fd,log_cache,strlen(log_cache));
	cur_position = lseek(log_fd,0,SEEK_CUR);
	if(cur_position > LOG_CACHE_SIZE){ 
		cur_position = 0;  
	}
	close(log_fd);
#endif
}

