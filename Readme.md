# TP OS Raspberry Pi

## Quelques commandes utiles sur la première partie

#### Général
Commandes à éxécuter à la racine du projet.
 - Pour compiler : ``` make KMAIN=test/kmain-bidule.c```. Par défaut c'est le `kmain.c` qui est lancé.
 - Pour lancer un test : ``` tools/run-test.sh test/kmain-bidule.c test/sys-bidule.gdb```

#### GDB
 - Liste des variables locales : ``` info locals```
 - Liste des arguments d'une fonction : ``` info args```
 - Info général sur une frame : ``` info registers ```
 - Liste des registres : ``` info registers ```
 - Afficher le mode actuel : ``` print_sr ```
 - Affiche les valuers contenues dans la pile : ``` dump_top_of_stack ```
 - Reset : ``` reset ```
