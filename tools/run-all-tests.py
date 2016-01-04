# -*- coding: utf-8 -*-

from sys import stdout
from subprocess import check_output
from time import time

def run_all_tests():
	counter = 1
	for test in tests:
		kmain_file = test[0]
		gdb_file = test[1]
		
		command_line = "./run-test.sh ../test/{0} ../test/{1}".format(kmain_file, gdb_file)
		print("{0}. Test {1}".format(counter, command_line), end=" : ")
		stdout.flush()
		start_time = time()
		test_output = check_output(command_line, shell=True).decode(encoding='UTF-8')
		exec_time = time() - start_time
		
		if test_output.endswith("OK\n"):
			print("OK ({0}s)".format(exec_time))
		elif test_output.endswith("ERROR\n"):
			print("ERROR ({0}s)".format(exec_time))
		else:
			print("ERROR (unexpected) ({0}s)".format(exec_time))
		counter += 1



tests = [
	["kmain-bidule.c", "bidule-called-twice.gdb"],
	["kmain-gettime.c", "sys-gettime-returns-value.gdb"],
	["kmain-settime.c", "sys-settime-passes-argument.gdb"],
	["kmain-reboot.c", "sys-reboot-does-reboot.gdb"],
	["kmain-nop-reboot.c", "sys-nop-does-return.gdb"],
	["kmain-nop-reboot.c", "swi-handler-preserves-SP.gdb"],
# No yieldto tests because create_process returned the process pcb
#	["kmain-yieldto.c", "sys-yieldto-jumps-to-dest.gdb"],
#	["kmain-yieldto.c", "sys-yieldto-preserves-locals.gdb"],
#	["kmain-yieldto.c", "sys-yieldto-preserves-status-register.gdb"],
# No yield test, because now we have IRQ interruptions
#	["kmain-yield.c", "round-robin-distinct-stacks.gdb"],
#	["kmain-yield.c", "round-robin-fairness.gdb"],
	["kmain-pagination.c", "pagination.gdb"],
	["kmain-vmem-alloc-free.c", "vmem-alloc-free.gdb"],
	["kmain-mmap-munmap.c", "sys-mmap-munmap.gdb"],
	["kmain-isolation.c", "isolation.gdb"],
	["kmain-sem-mutex.c", "sem-mutex.gdb"],
	["kmain-schedulers.c", "schedulers.gdb"],
	["kmain-sem.c", "sem.gdb"],
	["kmain-fork.c", "fork.gdb"],
	["kmain-fork-gmalloc.c", "fork-gmalloc.gdb"],
	["kmain-gmalloc-gfree.c", "gmalloc-gfree.gdb"],
	["kmain-fork-wait-waitpid.c", "fork-wait-waitpid.gdb"]
]

if __name__ == "__main__":
	run_all_tests()
