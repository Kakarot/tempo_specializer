#!/usr/bin/perl

$REMOVE_INT = $ARGV[0];

$NOMC = $ARGV[1];
$NOMD = $ARGV[1].".patched";
$NOM = $ARGV[1];
$NOM =~ s/(.*\/)?(.*).rtspec.c$/\2/;
$PATH = $1;

print "==> Patch du fichier $NOMC\n";

# Ouverture des differents fichiers
if (!open(FILEC,$NOMC)) {
  print "Erreur : le fichier $NOMC n'existe pas.\n";
  exit(1);
}

if (!open(FILED,">$NOMD")) {
  print "Erreur a la creation du fichier de sauvegarde $NOMD .\n";
  exit(1);
}

# Lecture du fichier a patcher
@lines_c = <FILEC>;


$PATCHVER="/* Patch 1.0 by CROQ/PICARD */\n";


# patchs : met en correspondance la ligne APRES laquelle il faut patcher
#          et le texte a inserer

$first_function = 1;
$inlined_function = 0;

for ($i=0; $i<$#lines_c; $i++) {

  $_= $lines_c[$i];

  if (/\Q$PATCHVER\E/) {
    print "Le fichier $NOMC a deja ete patche.\n";
    close(FILEC);
    close(FILED);
    close(FILEH);
    exit(1);
  }


  if (/^char \*\(\*rts_alloc_data.*malloc;$/) {
      $omit{$i}="yes";
  }
   
  if (/^char \*\(\*rts_alloc_code.*malloc;$/) {
      $omit{$i}="yes";
  }
   
  if (/^extern.*rts.*;$/) {
      $start_decls = $i+1;
  }
      
  # Insertion de la ligne #include "optim_params.h" apres les externs
  if (/^extern.*tmp.*\);$/) {
      while ($lines_c[$i] =~ /^extern.*tmp.*\);$/) {
	  if ($i>$#lines_c) {
	      printf("unexpected EOF\n");
	      exit(0);
	  }
	  $i++;
      }
      $patchs{$i}.= "extern char *get_code_mem(unsigned int);\n";
      $patchs{$i}.= "extern char *get_data_mem(unsigned int);\n";
      $patchs{$i}.= "\nGLOBAL_DECLS\n";
  }

  # Patch d'une fonction rts_
  if (/(.*)(rts_.*)\/\*.*\*\/(\(.*)\)  \{/) {

    $decl = "$1$2$3";

    if ($first_function) {
      $inlined_function = 0;
      $prefix = " FIRST_";
      $iprefix = "";
    }
    else {
      if ($lines_c[$i] =~ /inline_ptr/) {
        $inlined_function = 1;
        $prefix = " REST_INLINED_";
        $iprefix = " INLINED_";
      }
      else {
	$inlined_function = 0;
        $prefix = " REST_";
	$iprefix = " ";
      }
    }

    if (!($first_function)) {
      $patchs{$i} = $lines_c[$i];
      $patchs{$i} =~ s/\)  \{/$iprefix\)  \{/;
      $patchs{$i} =~ s/\)  \{/PARAMS\)  \{/;
      $omit{$i}="yes";
      $patchs{$start_decls} = "$decl$iprefix";
      $patchs{$start_decls} .= "PARAMS);\n";
      $omit{$start_decls} = "yes";
      $start_decls++;
    }

    while ($lines_c[$i] !~
	   /char \*buffer = \(\*rts_alloc_code\)\(20000\);/ &&
	   $lines_c[$i] !~
	   /char \*buffer = inline_ptr;/) {
	if ($i>$#lines_c) {
	    printf("unexpected EOF\n");
	    exit(0);
	}
	$i++;
    }
    $omit{$i}="yes";
    while ($lines_c[$i] !~ /char \*code_ptr = buffer;/) {
	if ($i>$#lines_c) {
	    printf("unexpected EOF\n");
	    exit(0);
	}
	if ($lines_c[$i] =~
	    /char \*data_buffer = \(\*rts_alloc_data\)\(20000\);/) {
	    $omit{$i} = "yes";
	    #forget about the data buffer
	    #$patchs{$i} = "    char *data_ptr = (*get_data_mem)(20000);\n";
	    $patchs{$i} = "";
	    }
	$i++;
    }
    $omit{$i}="yes";
    while ($lines_c[$i] !~ /char \*spec_ptr = code_ptr;/) {
	if ($i>$#lines_c) {
	    printf("unexpected EOF\n");
	    exit(0);
	}
	if ($lines_c[$i] =~ /char \*data_ptr = data_buffer;/) {
	    $omit{$i} = "yes";
	}
      $i++;
    }
    $omit{$i}="yes";

    while ($lines_c[$i] !~ /^[ ]*$/) {
	if ($i>$#lines_c) {
	    printf("unexpected EOF\n");
	    exit(0);
	}
	$i++;
    }

    $patchs{$i-1}.= "    char *code_ptr, *spec_ptr;\n";

    # body of function, until either code_ptr or spec_ptr
    while ($lines_c[$i] !~ /return.*_ptr;/) {
	if ($i>$#lines_c) {
	    printf("unexpected EOF\n");
	    exit(0);
	}
	if ($lines_c[$i] =~ /\(int\)rts_/) {
	    $patchs{$i} = $lines_c[$i];
            if ($REMOVE_INT == "1") {
		$patchs{$i} =~ s/\(int\)rts_/rts_/;
	    }
	    if ($lines_c[$i] =~ /INLINE_CALL/) {
		$patchs{$i} =~ s/\), ([0-9]*), ([0-9]*), (0x[0-9a-f]*)\);/ INLINED_ARGS\), $1, $2, $3\);/;
	    }
	    else {
		$patchs{$i} =~ s/\), ([0-9]*), (0x[0-9a-f]*), ([0-9]*)\);/ ARGS\), $1, $2, $3\);/;
	    }
	    $omit{$i}="yes";
	}
	$i++;
    }
    #$omit{$i} = "yes";

    if($first_function) {
      $first_function = 0;
    }
  }
}


print FILED $PATCHVER;
for  ($i=0; $i<=$#lines_c;$i++) {
    if (!$omit{$i}) {
	print FILED $lines_c[$i];
    }
    if ($RES=$patchs{$i}) {
	print FILED $RES;
    }  
}

close(FILEC);
close(FILED);
close(FILEH);

system("mv $NOMC $NOMC.backup");
system("mv $NOMD $NOMC");
