#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"


void internal_openMessageQueue(){
  //1 get from the PCB the resource id of the resource to open
  int id=running->syscall_args[0];
  int open_mode=running->syscall_args[1];

  //printf("Il processo con ID: %d sta eseguendo un'operazione di tipo Message Queue\n", running->pid);

  MessageQueue* res=MessageQueueList_byId(&mq_list, id);

  //2 if the resource is opened in CREATE mode, create the resource
  //  and return an error if the resource is already existing
  // otherwise fetch the resource from the system list, and if you don't find it
  // throw an error
  //printf ("CREATING id %d, type: %d, open mode %d\n", id, type, open_mode);
  if (open_mode == DSOS_CREATE){
    //printf("Il processo con ID: %d sta creando una Message Queue\n", running->pid);

    if (res) {
      running->syscall_retvalue=DSOS_ERESOURCECREATE;
      return;
    }

    res=MessageQueue_alloc(id);
    List_insert(&mq_list, mq_list.last, (ListItem*) res);

    //printf("Il processo con ID: %d ha terminato di creare la Message Queue con ID: %d\n", running->pid, id);
  }

  // at this point we should have the resource, if not something was wrong
  if (!res) {
     running->syscall_retvalue=DSOS_ERESOURCEOPEN;
     return;
  }
  
  if (open_mode == DSOS_EXCL && res->descriptors_ptrs.size){
     running->syscall_retvalue=DSOS_ERESOURCENOEXCL;
     return;
  }

  
  //5 create the descriptor for the resource in this process, and add it to
  //  the process descriptor list. Assign to the resource a new fd
  Descriptor* des=Descriptor_alloc_mq(running->last_fd, res, running);
  if (! des){
     running->syscall_retvalue=DSOS_ERESOURCENOFD;
     return;
  }
  running->last_fd++; // we increment the fd value for the next call
  DescriptorPtr* desptr=DescriptorPtr_alloc(des);
  List_insert(&running->descriptors, running->descriptors.last, (ListItem*) des);
  
  //6 add to the resource, in the descriptor ptr list, a pointer to the newly
  //  created descriptor
  des->ptr=desptr;
  List_insert(&res->descriptors_ptrs, res->descriptors_ptrs.last, (ListItem*) desptr);

  // return the FD of the new descriptor to the process
  running->syscall_retvalue = des->fd;
}
