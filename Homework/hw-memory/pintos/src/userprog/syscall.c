#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/palloc.h"

static void syscall_handler(struct intr_frame*);

void syscall_init(void) { intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall"); }

void syscall_exit(int status) {
  printf("%s: exit(%d)\n", thread_current()->name, status);
  thread_exit();
}

static void check_user_buffer(const void* buffer, size_t length) {
  uintptr_t delta = PHYS_BASE - buffer;
  if (!is_user_vaddr(buffer) || length > delta)
    syscall_exit(-1);
}

static void check_user_string(const char* string) {
  uintptr_t delta = PHYS_BASE - (const void*)string;
  if (!is_user_vaddr(string) || strnlen(string, delta) == delta)
    syscall_exit(-1);
}

static int syscall_open(const char* filename) {
  struct thread* curr_thread = thread_current();
  if (curr_thread->open_file != NULL)
    return -1;

  curr_thread->open_file = filesys_open(filename);
  if (curr_thread->open_file == NULL)
    return -1;

  return 2;
}

static int syscall_write(int fd, void* buffer, unsigned size) {
  struct thread* curr_thread = thread_current();
  if (fd == STDOUT_FILENO) {
    putbuf(buffer, size);
    return size;
  } else if (fd != 2 || curr_thread->open_file == NULL)
    return -1;

  return (int)file_write(curr_thread->open_file, buffer, size);
}

static int syscall_read(int fd, void* buffer, unsigned size) {
  struct thread* curr_thread = thread_current();
  if (fd != 2 || curr_thread->open_file == NULL)
    return -1;

  return (int)file_read(curr_thread->open_file, buffer, size);
}

static void syscall_close(int fd) {
  struct thread* curr_thread = thread_current();
  if (fd == 2 && curr_thread->open_file != NULL) {
    file_close(curr_thread->open_file);
    curr_thread->open_file = NULL;
  }
}

static void unallocate(uint8_t* start, uint8_t* end) {
  struct thread* curr_thread = thread_current();
  for (int i = 0; PGSIZE * i <= end - start; i++) {
    void* upage = start + PGSIZE * i;
    void* kpage = pagedir_get_page(curr_thread->pagedir, upage);
    palloc_free_page(kpage);
    pagedir_clear_page(curr_thread->pagedir, upage);
  }
}

static int allocate(uint8_t* start, uint8_t* end) {
  struct thread* curr_thread = thread_current();
  for (int i = 0; PGSIZE * i <= end - start; i++) {
    void* upage = start + PGSIZE * i;
    void* kpage = palloc_get_page(PAL_USER | PAL_ZERO);
    if (kpage == NULL) {
      unallocate(start, end);
      return 1;
    }
    if (!pagedir_set_page(curr_thread->pagedir, (void*)upage, kpage, true)) {
      palloc_free_page(kpage);
      unallocate(start, end);
      return 1;
    }
  }
  return 0;
}

static void* syscall_sbrk(intptr_t increment) {
  struct thread* curr_thread = thread_current();
  uint8_t* prev = (uint8_t*)curr_thread->heap_break;
  uint8_t* new = (uint8_t*)((intptr_t)curr_thread->heap_break + increment);

  if (((intptr_t) new < (intptr_t)curr_thread->heap_start)) {
    return (void*)-1;
  }
  uint8_t* prev_page = (uint8_t*)pg_round_down(prev - 1);
  uint8_t* new_page = (uint8_t*)pg_round_down(new - 1);

  if (increment == 0) {
    return prev;
  } else if (increment > 0) {
    if (prev == curr_thread->heap_start || prev_page != new_page) {
      if (allocate(prev_page + PGSIZE, new_page)) {
        return (void*)-1;
      }
    }
  } else {
    if (new == curr_thread->heap_start || prev_page != new_page) {
      unallocate(new_page + PGSIZE, prev_page);
    }
  }
  curr_thread->heap_break = new;
  return prev;
}

static void syscall_handler(struct intr_frame* f) {
  uint32_t* args = (uint32_t*)f->esp;
  struct thread* curr_thread = thread_current();
  curr_thread->in_syscall = true;

  check_user_buffer(args, sizeof(uint32_t));
  switch (args[0]) {
    case SYS_EXIT:
      check_user_buffer(&args[1], sizeof(uint32_t));
      syscall_exit((int)args[1]);
      break;
    case SYS_OPEN:
      check_user_buffer(&args[1], sizeof(uint32_t));
      check_user_string((char*)args[1]);
      f->eax = (uint32_t)syscall_open((char*)args[1]);
      break;

    case SYS_WRITE:
      check_user_buffer(&args[1], 3 * sizeof(uint32_t));
      check_user_buffer((void*)args[2], (unsigned)args[3]);
      f->eax = (uint32_t)syscall_write((int)args[1], (void*)args[2], (unsigned)args[3]);
      break;

    case SYS_READ:
      check_user_buffer(&args[1], 3 * sizeof(uint32_t));
      check_user_buffer((void*)args[2], (unsigned)args[3]);
      f->eax = (uint32_t)syscall_read((int)args[1], (void*)args[2], (unsigned)args[3]);
      break;

    case SYS_CLOSE:
      check_user_buffer(&args[1], sizeof(uint32_t));
      syscall_close((int)args[1]);
      break;

    case SYS_SBRK:
      check_user_buffer(&args[1], sizeof(intptr_t));
      f->eax = (uint32_t)syscall_sbrk((intptr_t)args[1]);
      break;

    default:
      printf("Unimplemented system call: %d\n", (int)args[0]);
      break;
  }

  curr_thread->in_syscall = false;
}