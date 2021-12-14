
#include <mqueue.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>  /* For O_* constants */
#include<string.h>
#include<errno.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

#define MAX_MSG_SIZE  10
#define MAX_NO_MSGS  6

const char* dummy_messages[MAX_NO_MSGS+1]={"hello",
                        "bye",
                        "cdac",
                        "desd",
                        "hyderabad",
                        NULL};
const char **temp=dummy_messages;

mqd_t msg_que;

void errExit(const char* exitstatus)
{
    if(strncmp(exitstatus,"mq_open",7)==0)
    {
        exit(EXIT_FAILURE);
    }
    else if(strncmp(exitstatus,"mq_setattr",10)==0)
    {
        mq_close(msg_que);
        fprintf(stderr,"couldnt set attributes\n");
        exit(EXIT_FAILURE);
    }
    else if(strncmp(exitstatus,"mq_send",7)==0)
    {
        switch (errno)
        {
            case EAGAIN:
                perror("error");
                break;
            case EINTR:
                perror("error");
                break;
            
            default:
                break;
        }
    }
}



int main()
{
    struct mq_attr msg_attr;
    msg_attr.mq_msgsize=(long)MAX_MSG_SIZE;
    msg_attr.mq_maxmsg=(long)MAX_NO_MSGS;
    msg_attr.mq_flags=O_NONBLOCK;
    msg_attr.mq_curmsgs=0;

    msg_que = mq_open("/msg_queue",(O_CREAT|O_RDWR),(S_IRUSR|S_IWUSR),&msg_attr);

    FILE* fptr=fopen("dictionary.txt","r");
    

    char str[100]={'\0'};

    

    size_t count=0;
    while(fgets(str,100,fptr)!=NULL)
    {
        if(str[0] == 'm')
        {
            if(mq_send(msg_que,str,strlen(str),0)==-1)
            {
                errExit("mq_send");
            }
        }
    }
    fprintf(stderr,"total %ld messages we sent\n",count);

    mq_close(msg_que);
    fclose(fptr);

    return 0;
    
}