#include "fb.h"
#include "config.h"
#include "kheap.h"
#include "vmem.h"

/*
 * Adresse du framebuffer, taille en byte, résolution de l'écran, pitch et depth (couleurs)
 */
static int32_t fb_address;
static uint32_t fb_size_bytes;
static uint32_t pitch,depth;
uint32_t fb_x, fb_y;


/*
 * Fonction pour lire et écrire dans les mailboxs
 */

/*
 * Fonction permettant d'écrire un message dans un canal de mailbox
 */
void MailboxWrite(uint32_t message, uint32_t mailbox)
{
  uint32_t status;

  if(mailbox > 10)            // Il y a que 10 mailbox (0-9) sur raspberry pi
    return;

  // On attend que la mailbox soit disponible i.e. pas pleine
  do{
    /*
     * Permet de flusher
     */
    data_mem_barrier();
    status = mmio_read(MAILBOX_STATUS);

    /*
     * Permet de flusher
     */
    data_mem_barrier();
  }while(status & 0x80000000);             // Vérification si la mailbox est pleine

  data_mem_barrier();
  mmio_write(MAILBOX_WRITE, message | mailbox);   // Combine le message à envoyer et le numéro du canal de la mailbox puis écrit en mémoire la combinaison
}

/*
 * Fonction permettant de lire un message et le retourner depuis un canal de mailbox
 */
uint32_t MailboxRead(uint32_t mailbox)
{
  uint32_t status;

  if(mailbox > 10)             // Il y a que 10 mailbox (0-9) sur raspberry pi
    return 0;

  while(1){
    // On attend que la mailbox soit disponible pour la lecture, i.e. qu'elle n'est pas vide
    do{
      data_mem_barrier();
      status = mmio_read(MAILBOX_STATUS);
      data_mem_barrier();
    }while(status & 0x40000000);             // On vérifie que la mailbox n'est pas vide

    data_mem_barrier();
    status = mmio_read(MAILBOX_BASE);
    data_mem_barrier();

    // On conserve uniquement les données et on les retourne
    if(mailbox == (status & 0x0000000f))
      return status & 0x0000000f;
  }
}

/*
 * Fonction permettant de dessiner un pixel à l'adresse x,y avec la couleur rgb red.green.blue
 */
void put_pixel_RGB24(uint32_t x, uint32_t y, uint8_t red, uint8_t green, uint8_t blue)
{
	volatile uint32_t *ptr=0;
	uint32_t offset=0;

	offset = (y * pitch) + (x * 3);
	ptr = (uint32_t*)(fb_address + offset);
	*((uint8_t*)ptr) = red;
	*((uint8_t*)(ptr+1)) = green;
	*((uint8_t*)(ptr+2)) = blue;
}

// Returns in the last three parameters the all colors at this indicated coordinates
void get_pixel_RGB24(uint32_t x, uint32_t y, uint8_t* red, uint8_t* green, uint8_t* blue)
{
	volatile uint32_t *ptr=0;
	uint32_t offset=0;

	offset = (y * pitch) + (x * 3);
	ptr = (uint32_t*)(fb_address + offset);
	*red = *((uint8_t*)ptr);
	*green = *((uint8_t*)(ptr+1));
	*blue = *((uint8_t*)(ptr+2));
}

void paint_all_black()
{
	uint32_t x, y;
	for (x = 0; x < fb_x; ++x)
	{
		for (y = 0; y < fb_y; ++y)
		{
			put_pixel_RGB24(x, y, 0, 0, 0);
		}
	}
}

/*
 * Fonction pour initialiser et écrire dans le framebuffer
 */
int FramebufferInitialize() {

  uint32_t retval=0;
  
  volatile unsigned int mb[100] __attribute__ ((aligned(16)));

  depth = 24;

  //
  // Tout d'abord, on veut récupérer l'adresse en mémoire du framebuffer
  //
  mb[0] = 8 * 4;		// Taille du buffer i.e. de notre message à envoyer dans la mailbox
  mb[1] = 0;			// On spécifie qu'on demande quelque chose
  mb[2] = 0x00040003;	// La question que l'on pose: https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface
  mb[3] = 2*4;		// La taille de la réponse
  mb[4] = 0;			// On indique que c'est une question ou un réponse (0 question)
  mb[5] = 0;			// Largeur
  mb[6] = 0;			// Hauteur
  mb[7] = 0;			// Marqueur de fin

  MailboxWrite((uint32_t)(mb+0x40000000), 8); // On écrit le message dans la mailbox

  if(((retval = MailboxRead(8)) == 0) || (mb[1] != 0x80000000)){ // On vérifie que le message est passé
    return 0;
  }

  fb_x = mb[5]; // On récupére la largeur en pixel de l'écran
  fb_y = mb[6]; // On récupére la hauteur en pixel de l'écran

  uint32_t mb_pos=1;

  mb[mb_pos++] = 0;			// C'est une requête
  mb[mb_pos++] = 0x00048003;	// On définit la hauteur et la largeur du framebuffer
  mb[mb_pos++] = 2*4;			// On envoi 2 int pour la taille donc on spécifie la taille du buffer
  mb[mb_pos++] = 2*4;			// Taille du message (tag + indicateur de requête)
  mb[mb_pos++] = fb_x;		// On passe la largeur
  mb[mb_pos++] = fb_y;		// On passe la hauteur

  mb[mb_pos++] = 0x00048004;	// On définit la hauteur et la largeur virtuel du framebuffer
  mb[mb_pos++] = 2*4;			// On envoi 2 int pour la taille donc on spécifie la taille du buffer
  mb[mb_pos++] = 2*4;			// Taille du message (tag + indicateur de requête)
  mb[mb_pos++] = fb_x;		// On passe la largeur
  mb[mb_pos++] = fb_y;		// On passe la hauteur

  mb[mb_pos++] = 0x00048005;	// On définit la profondeur du frame buffer
  mb[mb_pos++] = 1*4;
  mb[mb_pos++] = 1*4;
  mb[mb_pos++] = depth;		// Profondeur i.e. nombre de couleur (24bit dans notre cas)

  mb[mb_pos++] = 0x00040001;	// On demande l'allocation du buffer
  mb[mb_pos++] = 2*4;
  mb[mb_pos++] = 2*4;
  mb[mb_pos++] = 16;
  mb[mb_pos++] = 0;

  mb[mb_pos++] = 0;			// Tag de fin de message
  mb[0] = mb_pos*4;			// Taille du message dans son entier

  MailboxWrite((uint32_t)(mb+0x40000000), 8); // On écrit dans la mailbox

  if(((retval = MailboxRead(8)) == 0) || (mb[1] != 0x80000000)){ // On vérifie que le message a bien été passé
    return 0;
  }

  /*
   * On récupére les différentes informations récupérer de la requête pour pouvoir reconstruire l'adresse du framebuffer et sa taille
   */
  mb_pos = 2;
  unsigned int val_buff_len=0;
  while(mb[mb_pos] != 0){
    switch(mb[mb_pos++])
    {
      case 0x00048003:
	val_buff_len = mb[mb_pos++];
	mb_pos+= (val_buff_len/4)+1;
	break;
      case 0x00048004:
	val_buff_len = mb[mb_pos++];
	mb_pos+= (val_buff_len/4)+1;
	break;
      case 0x00048005:
	val_buff_len = mb[mb_pos++];
	mb_pos+= (val_buff_len/4)+1;
	break;
      case 0x00040001:
	val_buff_len = mb[mb_pos++];
	mb_pos++;
	fb_address = mb[mb_pos++];
	fb_size_bytes = mb[mb_pos++];
	break;
    }
  }

  //
  // Récupére le pitch (This indicates the number of bytes between rows. Usually it will be related to the width, but there are exceptions such as when drawing only part of an image.)
  //
  mb[0] = 8 * 4;		// Taille du buffer
  mb[1] = 0;			// C'est une requête
  mb[2] = 0x00040008;	// On veut récupérer le pitch
  mb[3] = 4;			// Taille du buffer
  mb[4] = 0;			// Taille de la demande
  mb[5] = 0;			// Le pitch sera stocké ici
  mb[6] = 0;			// Tag de fin de message
  mb[7] = 0;

  MailboxWrite((uint32_t)(mb+0x40000000), 8);

  if(((retval = MailboxRead(8)) == 0) || (mb[1] != 0x80000000)){
    return 0;
  }

  pitch = mb[5];

  insert_fb_address_in_mem(fb_address, fb_address + (fb_y*pitch + fb_x*3));

  fb_x--;
  fb_y--;
  
  return 1;
}


/*
 * Dessine sur tous les pixels des couleurs différentes
 */
void draw() {
  uint8_t red=0,green=0,blue=0;
  uint32_t x=0, y=0;
  for (x = 0; x < fb_x; x++) {
    for (y = 0; y < fb_y; y++) {
      if (blue > 254) {
	if (green > 254) {
	  if (red > 254) {
	    red=0; green=0; blue=0;
	  } else {
	    red++;
	  }
	} else {
	  green++;
	}
      } else blue++;
      put_pixel_RGB24(x,y,red,green,blue);
    }
  }
}

/*
 * Rempli l'écran de rouge
 */
void drawRed() {
  uint32_t x=0, y=0;
  for (x = 0; x < fb_x; x++) {
    for (y = 0; y < fb_y; y++) {
      put_pixel_RGB24(x,y,255,0,0);
    }
  }
}

/*
 * Rempli l'écran de blanc
 */
void drawBlue() {
  uint32_t x=0, y=0;
  for (x = 0; x < fb_x; x++) {
    for (y = 0; y < fb_y; y++) {
      put_pixel_RGB24(x,y,0,0,255);
    }
  }
}
