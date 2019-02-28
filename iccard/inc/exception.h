#ifndef EXCEPTION_H_
#define EXCEPTION_H_

//#define NDEBUG

#include <setjmp.h>
#include<assert.h>


/* MANPROCSIGnals. */
#define MANPROCSIG_HUP 1 /* Hangup (POSIX). */
#define MANPROCSIG_INT 2 /* Interrupt (ANSI). */
#define MANPROCSIG_QUIT 3 /* Quit (POSIX). */
#define MANPROCSIG_ILL 4 /* Illegal instruction (ANSI). */
#define MANPROCSIG_TRAP 5 /* Trace trap (POSIX). */
#define MANPROCSIG_ABRT 6 /* Abort (ANSI). */
#define MANPROCSIG_IOT 6 /* IOT trap (4.2 BSD). */
#define MANPROCSIG_BUS 7 /* BUS error (4.2 BSD). */
#define MANPROCSIG_FPE 8 /* Floating-point exception (ANSI). */
#define MANPROCSIG_KILL 9 /* Kill, unblockable (POSIX). */
#define MANPROCSIG_USR1 10 /* User-defined MANPROCSIG_nal 1 (POSIX). */
#define MANPROCSIG_SEGV 11 /* Segmentation violation (ANSI). */
#define MANPROCSIG_USR2 12 /* User-defined MANPROCSIG_nal 2 (POSIX). */
#define MANPROCSIG_PIPE 13 /* Broken pipe (POSIX). */
#define MANPROCSIG_ALRM 14 /* Alarm clock (POSIX). */
#define MANPROCSIG_TERM 15 /* Termination (ANSI). */
#define MANPROCSIG_STKFLT 16 /* Stack fault. */
#define MANPROCSIG_CLD MANPROCSIG_CHLD /* Same as MANPROCSIG_CHLD (System V). */

#define MANPROCSIG_CHLD 17 /* Child status has changed (POSIX). */
#define MANPROCSIG_CONT 18 /* Continue (POSIX). */
#define MANPROCSIG_STOP 19 /* Stop, unblockable (POSIX). */
#define MANPROCSIG_TSTP 20 /* Keyboard stop (POSIX). */
#define MANPROCSIG_TTIN 21 /* Background read from tty (POSIX). */
#define MANPROCSIG_TTOU 22 /* Background write to tty (POSIX). */
#define MANPROCSIG_URG 23 /* Urgent condition on socket (4.2 BSD). */
#define MANPROCSIG_XCPU 24 /* CPU limit exceeded (4.2 BSD). */
#define MANPROCSIG_XFSZ 25 /* File size limit exceeded (4.2 BSD). */
#define MANPROCSIG_VTALRM 26 /* Virtual alarm clock (4.2 BSD). */
#define MANPROCSIG_PROF 27 /* Profiling alarm clock (4.2 BSD). */
#define MANPROCSIG_WINCH 28 /* Window size change (4.3 BSD, Sun). */
#define MANPROCSIG_POLL MANPROCSIG_IO /* Pollable event occurred (System V). */
#define MANPROCSIG_IO 29 /* I/O now possible (4.2 BSD). */
#define MANPROCSIG_PWR 30 /* Power failure restart (System V). */
#define MANPROCSIG_SYS 31 /* Bad system call. */
#define MANPROCSIG_UNUSED 31

#define T Exception_t
typedef struct Exception_t{
    char *reason;
}Exception_t;

typedef struct Exception_frame{
    struct Exception_frame *prev;
    jmp_buf env;
    const char *file;
    int line;
    const T* exception;
}Exception_frame;

extern Exception_frame *Exception_stack;

enum{
    EXCEPTION_ENTERED=0,
    EXCEPTION_RAISED,
    EXCEPTION_HANDLED,
    EXCEPTION_FINALIZED
};
/* Manage all process signal,and automanage signal by process cause exit directoryly,*/
#define ManProcAllSig \
    int sum = 31; \
    while(sum){ \
        signal(sum,handle_proc_sig); \
        sum--; \
    }
/*Throw a exception*/
#define throw(e) exception_raise(&(e),__FILE__,__LINE__)

#define rethrow exception_raise(exception_frame.exception, \
exception_frame.file,exception_frame.line)

void handle_proc_sig(int signo);

void abort_without_exception(const Exception_t *e,const char *file,int line);

void exception_raise(const T *e,const char *file,int line);

#define try do{ \
    volatile int exception_flag; \
    Exception_frame exception_frame; \
    exception_frame.prev = Exception_stack; \
    Exception_stack = &exception_frame; \
    ManProcAllSig \
	exception_flag = setjmp(exception_frame.env); \
    if (exception_flag == EXCEPTION_ENTERED) \
    {

#define catch(e) \
    if(exception_flag == EXCEPTION_ENTERED) \
    Exception_stack = Exception_stack->prev; \
    }else if(exception_flag == e){ \
    exception_flag = EXCEPTION_HANDLED;

#define try_return \
    switch(Exception_stack = Exception_stack->prev,0) \
    default: return

#define catch_else \
    if(exception_flag == EXCEPTION_ENTERED) \
    Exception_stack = Exception_stack->prev; \
    }else if(exception_flag != EXCEPTION_HANDLED){ \
    exception_flag = EXCEPTION_HANDLED;

#define end_try \
    if(exception_flag == EXCEPTION_ENTERED) \
    Exception_stack = Exception_stack->prev; \
    } \
    if (exception_flag == EXCEPTION_RAISED) \
    exception_raise(exception_frame.exception, \
    exception_frame.file,exception_frame.line); \
    }while(0)

#define finally \
    if(exception_flag == EXCEPTION_ENTERED) \
    Exception_stack = Exception_stack->prev; \
    }{ \
    if(exception_flag == EXCEPTION_ENTERED) \
    exception_flag = EXCEPTION_FINALIZED;


#define LOG_FILE   "/sdcard/test.txt"
#define LOG_FILE_B "/sdcard/test.txt_b"
#define LOG_CACHE_SIZE 1024*1024*10
//#define ENABLE_LOG

void log_write(unsigned char *str);
//#define AAAA ":"
//#define TO_STR(str) AAAA#str
/*#define LOG_WRITE(str) do{\
        char log_cache[512];\
        int log_fd = open(LOG_FILE,O_CREAT|O_RDWR,S_IRWXU|S_IRWXG|S_IRWXO);\
        time_t nowT ;\
        nowT = time(0);\
        strftime(log_cache, sizeof(log_cache), "%Y-%m-%d %H:%M:%S:", localtime(&nowT));\
        lseek(log_fd,cur_position,SEEK_SET);\
        write(log_fd,log_cache,strlen(log_cache)); \
        sprintf(log_cache,"%s",str);\
        write(log_fd,log_cache,strlen(log_cache));\
		cur_position = lseek(log_fd,0,SEEK_CUR);\
		if(cur_position > LOG_CACHE_SIZE){ \
			cur_position = 0;  \
		}\
        close(log_fd);\
    }while(0)*/



#undef T
#endif /* EXCEPTION_H_ */
