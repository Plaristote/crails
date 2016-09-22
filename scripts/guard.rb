arguments = $*.map {|i| "'#{i}'"}

cmd = "guard #{arguments.join ' '}"
exec cmd
