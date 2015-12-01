# -*- mode: gdb-script -*-

define dump_top_of_stack
       print/x *((unsigned int*) $sp)@16
end

define reset
       # change to SYSTEM mode
       set $cpsr=0x000001dF
       # jump back to reset vector
       set $pc=0
end

define print_sr
       if $argc == 1
           set $value = $arg0
           printf "%d=", $value
       else
         printf "CPSR="
         set $value=$cpsr
       end
       printf "0x%08x (", $value
       if ( $value & 0x80000000)
          printf "N"
       else
          printf "n"
       end
       if ( $value & 0x40000000)
          printf "Z"
       else
          printf "z"
       end
       if ( $value & 0x20000000)
          printf "C"
       else
          printf "c"
       end
       if ( $value & 0x10000000)
          printf "V"
       else
          printf "v"
       end
       printf "..."
       if ( $value & 0x00000100)
          printf "A"
       else
          printf "a"
       end
       if ( $value & 0x00000080)
          printf "I"
       else
          printf "i"
       end
       if ( $value & 0x00000040)
           printf "F"
       else
           printf "f"
       end
       # brace yourselves, nested if/else are coming
       if ( ($value & 0x0000001F) == 0x1F )
           printf "[SYSTEM]"
       else
         if ( ($value & 0x0000001F) == 0x13 )
           printf "[SVC]"
         else
           if ( ($value & 0x0000001F) == 0x12 )
             printf "[IRQ]"
           else
             if ( ($value & 0x0000001F) == 0x11 )
               printf "[FIQ]"
             else
               if ( ($value & 0x0000001F) == 0x10 )
                 printf "[USER]"
               else
                 printf "[%02x]", (($value) & 0x0000001F)
               end
             end
           end
         end
       end
       printf ")\n"
end
