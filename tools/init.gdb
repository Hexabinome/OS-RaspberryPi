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
b irq_handler
b kmain.c:18
b kmain.c:24
b process
b do_sys_fork

source utils.gdb

continue
