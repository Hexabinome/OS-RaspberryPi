# -*- mode: gdb-script -*-

set verbose off
set confirm off

break vmem_init
commands
  print $pc
  continue
end

break kmain-gmalloc-gfree.c:10
commands
  p ((uint32_t) t2) - ((uint32_t) t1)
  p sizeof(int) + sizeof(struct heap_block)
  continue
end

break kmain-gmalloc-gfree.c:22
commands
  p/x test1
  p/x test2
  continue
end
break kmain-gmalloc-gfree.c:32
commands
  p/x a
  p/x aa
  continue
end
break kmain-gmalloc-gfree.c:34
commands
  p/x start_heap
  p/x current_process->heap
  continue
end

break kmain-gmalloc-gfree.c:38
commands
	assert_results
end


define assert_results
  set $ok = 1
  set $ok *= ($1 == vmem_init+4)
  set $ok *= ($2 == $3)

  set $ok *= ($4 == $6)

  set $ok *= ($6 == $7)
  
  set $ok *= ($8 == $9)

 if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
