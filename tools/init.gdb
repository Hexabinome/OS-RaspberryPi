# -*- mode:gdb-script -*-

layout split
focus cmd
winheight cmd 25
target remote:1234

set history filename ~/.gdb_history
set history save

b *reset_asm_handler
b *after_kmain
b kernel_panic

b *kmain
#b *swi_handler
b *irq_handler
b *sys_exit

b *user_process_1
b *user_process_2
b *user_process_3

source utils.gdb

continue
