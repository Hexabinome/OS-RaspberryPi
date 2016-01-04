# -*- mode: gdb-script -*-

set verbose off
set confirm off

break vmem_init
commands
  print $pc
  continue
end

break kmain-gmalloc-gfree.c:12
commands
  p diff
  continue
end

break kmain-gmalloc-gfree.c:24
commands
  p/x test1
  p/x test2
  continue
end
break kmain-gmalloc-gfree.c:34
commands
  p/x a
  p/x aa
  continue
end
break kmain-gmalloc-gfree.c:36
commands
  p/x start_heap
  p/x current_process->heap
  continue
end

break kmain-gmalloc-gfree.c:40
commands
	assert_results
end


define assert_results
  set $ok = 1
  set $ok *= ($1 == vmem_init+4)
  set $ok *= $2

  set $ok *= ($3 == $4)

  set $ok *= ($5 == $6)
  
  set $ok *= ($7 == $8)

 if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
