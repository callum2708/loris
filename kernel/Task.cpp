#include <stdint.h>
#include <paging.h>

#include <string.h>
#include <Task.h>
#include <kmalloc.h>
#include <stdio.h>

// The currently running task.
volatile Task *current_task = nullptr;

// The start of the task linked list.
volatile Task *ready_queue;

// Some externs are needed to access members in paging.c...
extern PageDirectory *kernel_directory;
extern PageDirectory *current_directory;
extern void alloc_frame(Page *, int, int);
extern uint32_t initial_esp;
extern "C" uint32_t read_eip();
extern "C" void perform_task_switch(uint32_t, uint32_t, uint32_t, uint32_t);

// The next available process ID.
uint32_t next_pid = 1;

void initialise_tasking()
{
    asm volatile("cli");

    // Relocate the stack so we know where it is.
    // move_stack((void *)0xE0000000, 0x2000);

    // Initialise the first task (kernel task)
    current_task = ready_queue = (Task *)kmalloc(sizeof(Task), false, nullptr);
    current_task->id = next_pid++;
    current_task->esp = current_task->ebp = 0;
    current_task->eip = 0;
    current_task->page_directory = current_directory;
    current_task->next = nullptr;

    // Reenable interrupts.
    asm volatile("sti");
}

int getpid()
{
    return current_task->id;
}

int fork()
{
    // We are modifying kernel structures, and so cannot be interrupted.
    asm volatile("cli");

    // Take a pointer to this process' task struct for later reference.
    Task *parent_task = (Task *)current_task;

    // Clone the address space.
    PageDirectory *directory = clone_directory(current_directory);
    // Create a new process.
    Task *new_task = (Task *)kmalloc(sizeof(Task), false, nullptr);
    new_task->id = next_pid++;
    new_task->esp = new_task->ebp = 0;
    new_task->eip = 0;
    new_task->page_directory = directory;
    new_task->next = nullptr;

    // Add it to the end of the ready queue.
    // Find the end of the ready queue...
    Task *tmp_task = (Task *)ready_queue;
    while (tmp_task->next)
        tmp_task = tmp_task->next;
    // ...And extend it.
    tmp_task->next = new_task;

    // This will be the entry point for the new process.
    uint32_t eip = read_eip();

    // We could be the parent or the child here - check.
    if (current_task == parent_task)
    {
        // We are the parent, so set up the esp/ebp/eip for our child.
        uint32_t esp;
        asm volatile("mov %%esp, %0"
                     : "=r"(esp));
        uint32_t ebp;
        asm volatile("mov %%ebp, %0"
                     : "=r"(ebp));
        new_task->esp = esp;
        new_task->ebp = ebp;
        new_task->eip = eip;
        // All finished: Reenable interrupts.
        asm volatile("sti");
        return new_task->id;
    }
    else
    {
        // We are the child - by convention return 0.
        return 0;
    }
}

void switch_task()
{
    // If we haven't initialised tasking yet, just return.
    if (!current_task)
        return;

    uint32_t esp, ebp, eip;
    asm volatile("mov %%esp, %0"
                 : "=r"(esp));
    asm volatile("mov %%ebp, %0"
                 : "=r"(ebp));
    // Read the instruction pointer. We do some cunning logic here:
    // One of two things could have happened when this function exits -
    // (a) We called the function and it returned the EIP as requested.
    // (b) We have just switched tasks, and because the saved EIP is essentially
    // the instruction after read_eip(), it will seem as if read_eip has just
    // returned.
    // In the second case we need to return immediately. To detect it we put a dummy
    // value in EAX further down at the end of this function. As C returns values in EAX,
    // it will look like the return value is this dummy value! (0x12345).
    eip = read_eip();

    // No, we didn't switch tasks. Let's save some register values and switch.
    current_task->eip = eip;
    current_task->esp = esp;
    current_task->ebp = ebp;

    current_task = current_task->next;
    if (!current_task)
    {
        current_task = ready_queue;
    }

    eip = current_task->eip;
    esp = current_task->esp;
    ebp = current_task->ebp;

    current_directory = current_task->page_directory;

    // Here we:
    // * Stop interrupts so we don't get interrupted.
    // * Temporarily put the new EIP location in ECX.
    // * Load the stack and base pointers from the new task struct.
    // * Change page directory to the physical address (physicalAddr) of the new directory.
    // * Put a dummy value (0x12345) in EAX so that above we can recognise that we've just
    // switched task.
    // * Restart interrupts. The STI instruction has a delay - it doesn't take effect until after
    // the next instruction.
    // * Jump to the location in ECX (remember we put the new EIP in there).
    perform_task_switch(eip, current_directory->PhysicalAddress, ebp, esp);
}

void move_stack(void *new_stack_start, uint32_t size)
{
    uint32_t i;
    // Allocate some space for the new stack.
    for (i = (uint32_t)new_stack_start;
         i >= ((uint32_t)new_stack_start - size);
         i -= 0x1000)
    {
        // General-purpose stack is in user-mode.
        alloc_frame(get_page(i, true, current_directory), false, true);
    }

    // Flush the TLB by reading and writing the page directory address again.
    uint32_t pd_addr;
    asm volatile("mov %%cr3, %0"
                 : "=r"(pd_addr));
    asm volatile("mov %0, %%cr3"
                 :
                 : "r"(pd_addr));

    // Old ESP and EBP, read from registers.
    uint32_t old_stack_pointer;
    asm volatile("mov %%esp, %0"
                 : "=r"(old_stack_pointer));
    uint32_t old_base_pointer;
    asm volatile("mov %%ebp, %0"
                 : "=r"(old_base_pointer));
    uint32_t offset = (uint32_t)new_stack_start - initial_esp;
    uint32_t new_stack_pointer = old_stack_pointer + offset;
    uint32_t new_base_pointer = old_base_pointer + offset;

    printf("test");
    while (true)
        ;
    memcpy((void *)new_stack_pointer, (void *)old_stack_pointer, initial_esp - old_stack_pointer);
    // Backtrace through the original stack, copying new values into
    // the new stack.
    for (i = (uint32_t)new_stack_start; i > (uint32_t)new_stack_start - size; i -= 4)
    {
        uint32_t tmp = *(uint32_t *)i;
        // If the value of tmp is inside the range of the old stack, assume it is a base pointer
        // and remap it. This will unfortunately remap ANY value in this range, whether they are
        // base pointers or not.
        if ((old_stack_pointer < tmp) && (tmp < initial_esp))
        {
            tmp = tmp + offset;
            uint32_t *tmp2 = (uint32_t *)i;
            *tmp2 = tmp;
        }
    }
    // Change stacks.
    asm volatile("mov %0, %%esp"
                 :
                 : "r"(new_stack_pointer));
    asm volatile("mov %0, %%ebp"
                 :
                 : "r"(new_base_pointer));
}