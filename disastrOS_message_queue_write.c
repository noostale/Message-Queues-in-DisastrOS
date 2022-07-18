#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_message_queue.h"
#include "disastrOS_message.h"

void internal_MessageQueue_write(){

    //Estraggo i parametri dalla system call
    int fd = running -> syscall_args[0];
    char* write_buffer = running -> syscall_args[1];
    int message_length = running -> syscall_args[2];


    Descriptor* mq_destination = DescriptorList_byFd(&running -> descriptors, fd);
    MessageQueue* mq = (MessageQueue*) mq_destination -> resource;

    //Allochiamo un messaggio 
    Message* message = Message_alloc(running -> pid, write_buffer, message_length);

    //Controllo se è stato possibile allocare il messaggio
    if (message==0){
        running -> syscall_retvalue = -64;
        return;
    }

    //Inseriamo il messaggio nella lista, dopo l'ultimo elemento
    List_insert(&mq->messages, mq->messages.last, (ListItem*)message);


    return;

}
