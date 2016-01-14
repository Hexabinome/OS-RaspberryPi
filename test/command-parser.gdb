# -*- mode: gdb-script -*-

set verbose off
set confirm off

break kmain-command-parser.c:12
commands
  p argc
  
# Test like this because testing a string needs malloc implementation
# Our malloc impl works, but gdb is not happy with it
# (could not fetch E14 error)
  p *((char*) (*res))
  p *(((char*) (*res)+1))
  p *(((char*) (*res)+2))
  p *(((char*) (*res)+3))
  p *(((char*) (*res)+4))
  
  p *((char*) (*(res+1)))
  p *((char*) (*(res+1))+1)
  
  p *((char*) (*(res+2)))
  p *((char*) (*(res+2))+1)
  p *((char*) (*(res+2))+2)
  p *((char*) (*(res+2))+3)
  
  p *((char*) (*(res+3)))
  p *((char*) (*(res+3))+1)
  
  p *((char*) (*(res+4)))
  p *((char*) (*(res+4))+1)
  p *((char*) (*(res+4))+2)
  p *((char*) (*(res+4))+3)
  p *((char*) (*(res+4))+4)
  p *((char*) (*(res+4))+5)
  p *((char*) (*(res+4))+6)
  p *((char*) (*(res+4))+7)
  p *((char*) (*(res+4))+8)
  p *((char*) (*(res+4))+9)
  p *((char*) (*(res+4))+10)
  p *((char*) (*(res+4))+11)
  p *((char*) (*(res+4))+12)
  
  p *((char*) (*(res+5)))
  p *((char*) (*(res+5))+1)
  
  continue
end

break kmain-command-parser.c:15
commands
  p argc

  p *((char*) (*res))
  p *(((char*) (*res)+1))
  p *(((char*) (*res)+2))
  p *(((char*) (*res)+3))

  continue
end

break kmain-command-parser.c:18
commands
  p argc

  p *((char*) (*res))
  p *(((char*) (*res)+1))
  p *(((char*) (*res)+2))
  p *(((char*) (*res)+3))

  p *((char*) (*(res+1)))
  p *((char*) (*(res+1))+1)
  p *((char*) (*(res+1))+2)
  p *((char*) (*(res+1))+3)
  p *((char*) (*(res+1))+4)
  p *((char*) (*(res+1))+5)
  p *((char*) (*(res+1))+6)
  p *((char*) (*(res+1))+7)
  p *((char*) (*(res+1))+8)
  p *((char*) (*(res+1))+9)
  p *((char*) (*(res+1))+10)
  p *((char*) (*(res+1))+11)
  
  assert_results
end

define assert_results
  # integer used as boolean
  set $ok = 1 
  # test results
  set $ok *= ($1 == 6)
  
  set $ok *= ($2 == 'h')
  set $ok *= ($3 == 'e')
  set $ok *= ($4 == 'l')
  set $ok *= ($5 == 'l')
  set $ok *= ($6 == 'o')
  
  set $ok *= ($7 == 'm')
  set $ok *= ($8 == 'y')
  
  set $ok *= ($9 == 'n')
  set $ok *= ($10 == 'a')
  set $ok *= ($11 == 'm')
  set $ok *= ($12 == 'e')
  
  set $ok *= ($13 == 'i')
  set $ok *= ($14 == 's')
  
  set $ok *= ($15 == 'd')
  set $ok *= ($16 == 'a')
  set $ok *= ($17 == 'v')
  set $ok *= ($18 == 'i')
  set $ok *= ($19 == 'd')
  set $ok *= ($20 == ' ')
  set $ok *= ($21 == 'w')
  set $ok *= ($22 == 'o')
  set $ok *= ($23 == 'b')
  set $ok *= ($24 == 'r')
  set $ok *= ($25 == 'o')
  set $ok *= ($26 == 'c')
  set $ok *= ($27 == 'k')
  
  set $ok *= ($28 == 'y')
  set $ok *= ($29 == 'o')
  
  set $ok *= ($30 == 1)
  set $ok *= ($31 == 'e')
  set $ok *= ($32 == 'c')
  set $ok *= ($33 == 'h')
  set $ok *= ($34 == 'o')
  
  set $ok *= ($35 == 2)
  set $ok *= ($36 == 'e')
  set $ok *= ($37 == 'c')
  set $ok *= ($38 == 'h')
  set $ok *= ($39 == 'o')
  set $ok *= ($40 == 'H')
  set $ok *= ($41 == 'e')
  set $ok *= ($42 == 'l')
  set $ok *= ($43 == 'l')
  set $ok *= ($44 == 'o')
  set $ok *= ($45 == ' ')
  set $ok *= ($46 == 'w')
  set $ok *= ($47 == 'o')
  set $ok *= ($48 == 'r')
  set $ok *= ($49 == 'l')
  set $ok *= ($50 == 'd')
  set $ok *= ($51 == '!')
  
  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
