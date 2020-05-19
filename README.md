 The program constructs an FTree representing a file tree rooted at the file specified by the user. This can then be visualized.
 
 Sample Usage on the terminal( on a populated folder):
 $ make
 
 $ ../print_ftree .
 
 ===== . (d700) =====
 
  ftree.c (-640)
  
  Makefile (-640)
  
  ===== temp (d750) =====

    a.txt (-640)
    
    link (l777)
    
    ftree_link.c (-640)
    
  ftree.h (-644)
  
  print_ftree.c (-644)
 
 
 The numbers above in the brackets are file persmissions.
