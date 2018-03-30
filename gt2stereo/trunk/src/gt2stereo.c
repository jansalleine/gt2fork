//
// GOATTRACKER v2.74 Stereo
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#define GOATTRK2_C

#ifdef __WIN32__
#include <windows.h>
#endif

#include "goattrk2.h"
#include "bme.h"

int menu = 0;
int editmode = EDIT_PATTERN;
int recordmode = 1;
int followplay = 0;
int hexnybble = -1;
int stepsize = 4;
int autoadvance = 0;
int defaultpatternlength = 64;
int cursorflash = 0;
int cursorcolortable[] = {1,2,7,2};
int exitprogram = 0;
int eacolumn = 0;
int eamode = 0;

unsigned keypreset = KEY_TRACKER;
unsigned playerversion = 0;
int fileformat = FORMAT_PRG;
int zeropageadr = 0xfc;
int playeradr = 0x1000;
unsigned sidmodel = 0;
unsigned multiplier = 1;
unsigned adparam = 0x0f00;
unsigned ntsc = 0;
unsigned patternhex = 0;
unsigned sidaddress = 0xd500d400;
unsigned finevibrato = 1;
unsigned optimizepulse = 1;
unsigned optimizerealtime = 1;
unsigned customclockrate = 0;
unsigned usefinevib = 0;
unsigned b = DEFAULTBUF;
unsigned mr = DEFAULTMIXRATE;
unsigned writer = 0;
unsigned hardsid = 0;
unsigned catweasel = 0;
unsigned interpolate = 0;
unsigned residdelay = 0;
unsigned hardsidbufinteractive = 20;
unsigned hardsidbufplayback = 400;
float basepitch = 0.0f;

char configbuf[MAX_PATHNAME];
char loadedsongfilename[MAX_FILENAME];
char songfilename[MAX_FILENAME];
char songfilter[MAX_FILENAME];
char songpath[MAX_PATHNAME];
char instrfilename[MAX_FILENAME];
char instrfilter[MAX_FILENAME];
char instrpath[MAX_PATHNAME];
char packedpath[MAX_PATHNAME];

char *programname = "$VER: GoatTracker v2.74 Stereo";

char textbuffer[MAX_PATHNAME];

unsigned char hexkeytbl[] = {'0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

extern unsigned char datafile[];

int main(int argc, char **argv)
{
  char filename[MAX_PATHNAME];
  FILE *configfile;
  int c,d;

  programname += sizeof "$VER:";
  // Open datafile
  io_openlinkeddatafile(datafile);

  // Load configuration
  #ifdef __WIN32__
  GetModuleFileName(NULL, filename, MAX_PATHNAME);
  filename[strlen(filename)-3] = 'c';
  filename[strlen(filename)-2] = 'f';
  filename[strlen(filename)-1] = 'g';
  #elif __amigaos__
  strcpy(filename, "PROGDIR:goattrk2.cfg");
  #else
  strcpy(filename, getenv("HOME"));
  strcat(filename, "/.goattrk/gt2stereo.cfg");
  #endif
  configfile = fopen(filename, "rt");
  if (configfile)
  {
    getparam(configfile, &b);
    getparam(configfile, &mr);
    getparam(configfile, &hardsid);
    getparam(configfile, &sidmodel);
    getparam(configfile, &ntsc);
    getparam(configfile, (unsigned *)&fileformat);
    getparam(configfile, (unsigned *)&playeradr);
    getparam(configfile, (unsigned *)&zeropageadr);
    getparam(configfile, &playerversion);
    getparam(configfile, &keypreset);
    getparam(configfile, (unsigned *)&stepsize);
    getparam(configfile, &multiplier);
    getparam(configfile, &catweasel);
    getparam(configfile, &adparam);
    getparam(configfile, &interpolate);
    getparam(configfile, &patternhex);
    getparam(configfile, &sidaddress);
    getparam(configfile, &finevibrato);
    getparam(configfile, &optimizepulse);
    getparam(configfile, &optimizerealtime);
    getparam(configfile, &residdelay);
    getparam(configfile, &customclockrate);
    getparam(configfile, &hardsidbufinteractive);
    getparam(configfile, &hardsidbufplayback);
    getfloatparam(configfile, &filterparams.distortionrate);
    getfloatparam(configfile, &filterparams.distortionpoint);
    getfloatparam(configfile, &filterparams.distortioncfthreshold);
    getfloatparam(configfile, &filterparams.type3baseresistance);
    getfloatparam(configfile, &filterparams.type3offset);
    getfloatparam(configfile, &filterparams.type3steepness);
    getfloatparam(configfile, &filterparams.type3minimumfetresistance);
    getfloatparam(configfile, &filterparams.type4k);
    getfloatparam(configfile, &filterparams.type4b);
    getfloatparam(configfile, &filterparams.voicenonlinearity);
    getparam(configfile, &win_fullscreen);
    getfloatparam(configfile, &basepitch);
    fclose(configfile);
  }

  // Init pathnames
  initpaths();

  // Scan command line
  for (c = 1; c < argc; c++)
  {
    #ifdef __WIN32__
    if ((argv[c][0] == '-') || (argv[c][0] == '/'))
    #else
    if (argv[c][0] == '-')
    #endif
    {
      int y = 0;
      switch(toupper(argv[c][1]))
      {
        case '?':
        if (!initscreen())
          return 1;       
        if (argv[c][2]=='?') {
          onlinehelp(1,0);
          return 0;
        }

        printtext(0,y++,15,"Usage: GT2STEREO [songname] [options]");
        printtext(0,y++,15,"Options:");
        printtext(0,y++,15,"-Axx Set ADSR parameter for hardrestart in hex. DEFAULT=0F00");
        printtext(0,y++,15,"-Bxx Set sound buffer length in milliseconds DEFAULT=100");
        printtext(0,y++,15,"-Cxx Use CatWeasel MK3 PCI SID (0 = off, 1 = on)");
        printtext(0,y++,15,"-Dxx Pattern row display (0 = decimal, 1 = hexadecimal)");
        printtext(0,y++,15,"-Exx Set emulated SID model (0 = 6581 1 = 8580) DEFAULT=6581");
        printtext(0,y++,15,"-Fxx Set custom SID clock cycles per second (0 = use PAL/NTSC default)");
        printtext(0,y++,15,"-Gxx Set pitch of A-4 in Hz (0 = use default frequencytable, close to 440Hz)");
        printtext(0,y++,15,"-Hxx Use HardSID (0 = off, 1 = HardSID ID0 2 = HardSID ID1 etc.)");
        printtext(0,y++,15,"     Use high nybble (it's hexadecimal) to specify right HardSID ID");
        printtext(0,y++,15,"-Ixx Set reSID interpolation (0 = off, 1 = on, 2 = distortion, 3 = distortion & on) DEFAULT=off");
        printtext(0,y++,15,"-Kxx Note-entry mode (0 = PROTRACKER 1 = DMC) DEFAULT=PROTRK.");
        printtext(0,y++,15,"-Lxx SID memory locations in hex. DEFAULT=D500D400");
        printtext(0,y++,15,"-Mxx Set sound mixing rate DEFAULT=44100");
        printtext(0,y++,15,"-Oxx Set pulseoptimization/skipping (0 = off, 1 = on) DEFAULT=on");
        printtext(0,y++,15,"-Rxx Set realtime-effect optimization/skipping (0 = off, 1 = on) DEFAULT=on");
        printtext(0,y++,15,"-Sxx Set speed multiplier (0 for 25Hz, 1 for 1x, 2 for 2x etc.)");
        printtext(0,y++,15,"-Txx Set HardSID interactive mode sound buffer length in milliseconds DEFAULT=20, max.buffering=0");
        printtext(0,y++,15,"-Uxx Set HardSID playback mode sound buffer length in milliseconds DEFAULT=400, max.buffering=0");
        printtext(0,y++,15,"-Vxx Set finevibrato conversion (0 = off, 1 = on) DEFAULT=on");
        printtext(0,y++,15,"-Xxx Set window type (0 = window, 1 = fullscreen) DEFAULT=window");
        printtext(0,y++,15,"-Zxx Set random reSID write delay in cycles (0 = off) DEFAULT=off");
        printtext(0,y++,15,"-N   Use NTSC timing");
        printtext(0,y++,15,"-P   Use PAL timing (DEFAULT)");
        printtext(0,y++,15,"-W   Write sound output to a file SIDAUDIO.RAW");
        printtext(0,y++,15,"-?   Show this info again");
        printtext(0,y++,15,"-??  Standalone online help window");
        waitkeynoupdate();
        return 0;

        case 'Z':
        sscanf(&argv[c][2], "%u", &residdelay);
        break;

        case 'A':
        sscanf(&argv[c][2], "%x", &adparam);
        break;

        case 'S':
        sscanf(&argv[c][2], "%u", &multiplier);
        break;

        case 'B':
        sscanf(&argv[c][2], "%u", &b);
        break;

        case 'D':
        sscanf(&argv[c][2], "%u", &patternhex);
        break;

        case 'E':
        sscanf(&argv[c][2], "%u", &sidmodel);
        break;

        case 'I':
        sscanf(&argv[c][2], "%u", &interpolate);
        break;

        case 'K':
        sscanf(&argv[c][2], "%u", &keypreset);
        break;

        case 'L':
        sscanf(&argv[c][2], "%x", &sidaddress);
        break;

        case 'N':
        ntsc = 1;
        customclockrate = 0;
        break;

        case 'P':
        ntsc = 0;
        customclockrate = 0;
        break;

        case 'F':
        sscanf(&argv[c][2], "%u", &customclockrate);
        break;

        case 'M':
        sscanf(&argv[c][2], "%u", &mr);
        break;

        case 'O':
        sscanf(&argv[c][2], "%u", &optimizepulse);
        break;

        case 'R':
        sscanf(&argv[c][2], "%u", &optimizerealtime);
        break;

        case 'H':
        sscanf(&argv[c][2], "%x", &hardsid);
        break;

        case 'V':
        sscanf(&argv[c][2], "%u", &finevibrato);
        break;

        case 'T':
        sscanf(&argv[c][2], "%u", &hardsidbufinteractive);
        break;

        case 'U':
        sscanf(&argv[c][2], "%u", &hardsidbufplayback);
        break;

        case 'W':
        writer = 1;
        break;

        case 'X':
        sscanf(&argv[c][2], "%u", &win_fullscreen);
        break;

        case 'C':
        sscanf(&argv[c][2], "%u", &catweasel);
        break;

        case 'G':
        sscanf(&argv[c][2], "%f", &basepitch);
        break;
      }
    }
    else
    {
      char startpath[MAX_PATHNAME];

      strcpy(songfilename, argv[c]);
      for (d = strlen(argv[c])-1; d >= 0; d--)
      {
        if ((argv[c][d] == '/') || (argv[c][d] == '\\'))
        {
          strcpy(startpath, argv[c]);
          startpath[d+1] = 0;
          chdir(startpath);
          initpaths();
          strcpy(songfilename, &argv[c][d+1]);
          break;
        }
      }
    }
  }

  // Validate parameters
  sidmodel &= 1;
  adparam &= 0xffff;
  zeropageadr &= 0xff;
  playeradr &= 0xff00;
  if (!stepsize) stepsize = 4;
  if (multiplier > 16) multiplier = 16;
  if (keypreset > 2) keypreset = 0;
  if ((finevibrato == 1) && (multiplier < 2)) usefinevib = 1;
  if (finevibrato > 1) usefinevib = 1;
  if (optimizepulse > 1) optimizepulse = 1;
  if (optimizerealtime > 1) optimizerealtime = 1;
  if (residdelay > 63) residdelay = 63;
  if (customclockrate < 100) customclockrate = 0;

  // Calculate frequencytable if necessary
  if (basepitch < 0.0f)
    basepitch = 0.0f;
  if (basepitch > 0.0f)
    calculatefreqtable();

  // Set screenmode
  if (!initscreen())
    return 1;

  // Reset channels/song
  initchannels();
  clearsong(1,1,1,1,1);

  // Init sound
  if (!sound_init(b, mr, writer, hardsid, sidmodel, ntsc, multiplier, catweasel, interpolate, customclockrate))
  {
    printtextc(MAX_ROWS/2-1,15,"Sound init failed. Press any key to run without sound (notice that song timer won't start)");
    waitkeynoupdate();
  }

  // Load song if applicable
  if (strlen(songfilename)) loadsong();

  // Start editor mainloop
  printmainscreen();
  while (!exitprogram)
  {
    waitkeymouse();
    docommand();
  }

  // Shutdown sound output now
  sound_uninit();

  // Save configuration
  #ifndef __WIN32__
  #ifdef __amigaos__
  strcpy(filename, "PROGDIR:goattrk2.cfg");
  #else
  strcpy(filename, getenv("HOME"));
  strcat(filename, "/.goattrk");
  mkdir(filename, S_IRUSR | S_IWUSR | S_IXUSR);
  strcat(filename, "/gt2stereo.cfg");
  #endif
  #endif
  configfile = fopen(filename, "wt");
  if (configfile)
  {
    fprintf(configfile, ";------------------------------------------------------------------------------\n"
                        ";GT2 config file. Rows starting with ; are comments. Hexadecimal parameters are\n"
                        ";to be preceded with $ and decimal parameters with nothing.                    \n"
                        ";------------------------------------------------------------------------------\n"
                        "\n"
                        ";reSID buffer length (in milliseconds)\n%d\n\n"
                        ";reSID mixing rate (in Hz)\n%d\n\n"
                        ";Hardsid device number (0 = off)\n%d\n\n"
                        ";reSID model (0 = 6581, 1 = 8580)\n%d\n\n"
                        ";Timing mode (0 = PAL, 1 = NTSC)\n%d\n\n"
                        ";Packer/relocator fileformat (0 = SID, 1 = PRG, 2 = BIN)\n%d\n\n"
                        ";Packer/relocator player address\n$%04x\n\n"
                        ";Packer/relocator zeropage baseaddress\n$%02x\n\n"
                        ";Packer/relocator player type (0 = standard ... 3 = minimal)\n%d\n\n"
                        ";Key entry mode (0 = Protracker, 1 = DMC)\n%d\n\n"
                        ";Pattern highlight step size\n%d\n\n"
                        ";Speed multiplier (0 = 25Hz, 1 = 1X, 2 = 2X etc.)\n%d\n\n"
                        ";Use CatWeasel SID (0 = off, 1 = on)\n%d\n\n"
                        ";Hardrestart ADSR parameter\n$%04x\n\n"
                        ";reSID interpolation (0 = off, 1 = on, 2 = distortion, 3 = distortion & on)\n%d\n\n"
                        ";Hexadecimal pattern display (0 = off, 1 = on)\n%d\n\n"
                        ";SID baseaddresses\n$%08x\n\n"
                        ";Finevibrato mode (0 = off, 1 = on)\n%d\n\n"
                        ";Pulseskipping (0 = off, 1 = on)\n%d\n\n"
                        ";Realtime effect skipping (0 = off, 1 = on)\n%d\n\n"
                        ";Random reSID write delay in cycles (0 = off)\n%d\n\n"
                        ";Custom SID clock cycles per second (0 = use PAL/NTSC default)\n%d\n\n"
                        ";HardSID interactive mode buffer size (in milliseconds, 0 = maximum/no flush)\n%d\n\n"
                        ";HardSID playback mode buffer size (in milliseconds, 0 = maximum/no flush)\n%d\n\n"
                        ";reSID-fp distortion rate\n%f\n\n"
                        ";reSID-fp distortion point\n%f\n\n"
                        ";reSID-fp distortion CF threshold\n%f\n\n"
                        ";reSID-fp type 3 base resistance\n%f\n\n"
                        ";reSID-fp type 3 base offset\n%f\n\n"
                        ";reSID-fp type 3 base steepness\n%f\n\n"
                        ";reSID-fp type 3 minimum FET resistance\n%f\n\n"
                        ";reSID-fp type 4 k\n%f\n\n"
                        ";reSID-fp type 4 b\n%f\n\n"
                        ";reSID-fp voice nonlinearity\n%f\n\n"
                        ";Window type (0 = window, 1 = fullscreen)\n%d\n\n"
                        ";Base pitch of A-4 in Hz (0 = use default frequencytable)\n%f\n\n",
    b,
    mr,
    hardsid,
    sidmodel,
    ntsc,
    fileformat,
    playeradr,
    zeropageadr,
    playerversion,
    keypreset,
    stepsize,
    multiplier,
    catweasel,
    adparam,
    interpolate,
    patternhex,
    sidaddress,
    finevibrato,
    optimizepulse,
    optimizerealtime,
    residdelay,
    customclockrate,
    hardsidbufinteractive,
    hardsidbufplayback,
    filterparams.distortionrate,
    filterparams.distortionpoint,
    filterparams.distortioncfthreshold,
    filterparams.type3baseresistance,
    filterparams.type3offset,
    filterparams.type3steepness,
    filterparams.type3minimumfetresistance,
    filterparams.type4k,
    filterparams.type4b,
    filterparams.voicenonlinearity,
    win_fullscreen,
    basepitch);
    fclose(configfile);
  }

  // Exit
  return 0;
}

void waitkey(void)
{
  for (;;)
  {
    displayupdate();
    getkey();
    if ((rawkey) || (key)) break;
    if (win_quitted) break;
  }

  converthex();
}

void waitkeymouse(void)
{
  for (;;)
  {
    displayupdate();
    getkey();
    if ((rawkey) || (key)) break;
    if (win_quitted) break;
    if (mouseb) break;
  }

  converthex();
}

void waitkeymousenoupdate(void)
{
  for (;;)
  {
  	fliptoscreen();
    getkey();
    if ((rawkey) || (key)) break;
    if (win_quitted) break;
    if (mouseb) break;
  }

  converthex();
}

void waitkeynoupdate(void)
{
  for (;;)
  {
  	fliptoscreen();
    getkey();
    if ((rawkey) || (key)) break;
    if ((mouseb) && (!prevmouseb)) break;
    if (win_quitted) break;
  }
}

void converthex()
{
  int c;

  hexnybble = -1;
  for (c = 0; c < 16; c++)
  {
    if (tolower(key) == hexkeytbl[c])
    {
      if (c >= 10)
      {
        if (!shiftpressed) hexnybble = c;
      }
      else
      {
        hexnybble = c;
      }
    }
  }
}


void docommand(void)
{
  // "GUI" operation :)
  mousecommands();

  // Mode-specific commands
  switch(editmode)
  {
    case EDIT_ORDERLIST:
    orderlistcommands();
    break;

    case EDIT_INSTRUMENT:
    instrumentcommands();
    break;

    case EDIT_TABLES:
    tablecommands();
    break;

    case EDIT_PATTERN:
    patterncommands();
    break;

    case EDIT_NAMES:
    namecommands();
    break;
  }

  // General commands
  generalcommands();
}

void mousecommands(void)
{
  int c;

  if (!mouseb) return;

  // Pattern editpos & pattern number selection
  for (c = 0; c < MAX_CHN; c++)
  {
    if ((mousey == 2) && (mousex >= 11 + c*9) && (mousex <= 12 + c*9))
    {
    	if ((!prevmouseb) || (mouseheld > HOLDDELAY))
    	{
        if (mouseb & MOUSEB_LEFT)
        {
          epchn = c;
          nextpattern();
        }
        if (mouseb & MOUSEB_RIGHT)
        {
          epchn = c;
          prevpattern();
        }
      }
    }
    else
    {
      if ((mousey >= 2) && (mousey <= 34) && (mousex >= 5 + c*9) && (mousex <= 12 + c*9))
      {
        int x = mousex - 5 - c*9;
        int newpos = mousey - 3 + epview;
        if (newpos < 0) newpos = 0;
        if (newpos > pattlen[epnum[epchn]]) newpos = pattlen[epnum[epchn]];

        editmode = EDIT_PATTERN;

        if ((mouseb & (MOUSEB_RIGHT|MOUSEB_MIDDLE)) && (!prevmouseb))
        {
          if ((epmarkchn != c) || (newpos != epmarkend))
          {
            epmarkchn = c;
            epmarkstart = epmarkend = newpos;
          }
        }

        if (mouseb & MOUSEB_LEFT)
        {
          epchn = c;
          if (x < 3) epcolumn = 0;
          if (x >= 3) epcolumn = x - 2;
        }

        if (!prevmouseb)
        {
          if (mouseb & MOUSEB_LEFT)
            eppos = newpos;
        }
        else
        {
        	if (mouseb & MOUSEB_LEFT)
        	{
            if (mousey == 2) eppos--;
            if (mousey == 34) eppos++;
          }
        }
        if (eppos < 0) eppos = 0;
        if (eppos > pattlen[epnum[epchn]]) eppos = pattlen[epnum[epchn]];

        if (mouseb & (MOUSEB_RIGHT|MOUSEB_MIDDLE)) epmarkend = newpos;
      }
    }
  }


  // Song editpos & songnumber selection
  if ((mousey >= 3) && (mousey <= 8) && (mousex >= 40+20))
  {
    int newpos = esview + (mousex - 44 - 20) / 3;
    int newcolumn = (mousex - 44 - 20) % 3;
    int newchn = mousey - 3;
    if (newcolumn < 0) newcolumn = 0;
    if (newcolumn > 1) newcolumn = 1;
    if (newpos < 0)
    {
      newpos = 0;
      newcolumn = 0;
    }
    if (newpos == songlen[esnum][eschn])
    {
      newpos++;
      newcolumn = 0;
    }
    if (newpos > songlen[esnum][eschn]+1)
    {
      newpos = songlen[esnum][eschn] + 1;
      newcolumn = 1;
    }

    editmode = EDIT_ORDERLIST;

    if ((mouseb & (MOUSEB_RIGHT|MOUSEB_MIDDLE)) && (!prevmouseb) && (newpos < songlen[esnum][eschn]))
    {
      if ((esmarkchn != newchn) || (newpos != esmarkend))
      {
        esmarkchn = newchn;
        esmarkstart = esmarkend = newpos;
      }
    }

    if (mouseb & MOUSEB_LEFT)
    {
      eschn = newchn;
      eseditpos = newpos;
      escolumn = newcolumn;
    }

    if ((mouseb & (MOUSEB_RIGHT|MOUSEB_MIDDLE)) && (newpos < songlen[esnum][eschn])) esmarkend = newpos;
  }
  if (((!prevmouseb) || (mouseheld > HOLDDELAY)) && (mousey == 2) && (mousex >= 63+20) && (mousex <= 64+20))
  {
    if (mouseb & MOUSEB_LEFT) nextsong();
    if (mouseb & MOUSEB_RIGHT) prevsong();
  }

  // Instrument editpos & instrument number selection
  if ((mousey >= 8+3) && (mousey <= 12+3) && (mousex >= 56+20) && (mousex <= 57+20))
  {
    editmode = EDIT_INSTRUMENT;
    eipos = mousey - 8 - 3;
    eicolumn = mousex - 56 - 20;
  }
  if ((mousey >= 8+3) && (mousey <= 11+3) && (mousex >= 76+20) && (mousex <= 77+20))
  {
    editmode = EDIT_INSTRUMENT;
    eipos = mousey - 8 - 3 + 5;
    eicolumn = mousex - 76 - 20;
  }
  if ((mousey == 7+3) && (mousex >= 60+20))
  {
    editmode = EDIT_INSTRUMENT;
    eipos = 9;
  }
  if (((!prevmouseb) || (mouseheld > HOLDDELAY)) && (mousey == 7+3) && (mousex >= 56+20) && (mousex <= 57+20))
  {
    if (mouseb & MOUSEB_LEFT) nextinstr();
    if (mouseb & MOUSEB_RIGHT) previnstr();
  }

  // Table editpos
  for (c = 0; c < MAX_TABLES; c++)
  {
    if ((mousey >= 14+3) && (mousey <= 20+3+9) && (mousex >= 43 + 20 + c*10) && (mousex <= 47 + 20 + c*10))
    {
      int newpos = mousey - 15 - 3 + etview[etnum];
      if (newpos < 0) newpos = 0;
      if (newpos >= MAX_TABLELEN) newpos = MAX_TABLELEN-1;

      editmode = EDIT_TABLES;

      if ((mouseb & (MOUSEB_RIGHT|MOUSEB_MIDDLE)) && (!prevmouseb))
      {
        if ((etmarknum != etnum) || (newpos != etmarkend))
        {
          etmarknum = c;
          etmarkstart = etmarkend = newpos;
        }
      }
      if (mouseb & MOUSEB_LEFT)
      {
        etnum = c;
        etpos = mousey - 15 - 3 + etview[etnum];
        etcolumn = mousex - 43 - 20 - c*10;
      }
      if (etcolumn >= 2) etcolumn--;
      if (etpos < 0) etpos = 0;
      if (etpos > MAX_TABLELEN-1) etpos = MAX_TABLELEN-1;

      if (mouseb & (MOUSEB_RIGHT|MOUSEB_MIDDLE)) etmarkend = newpos;
    }
  }

  // Name editpos
  if ((mousey == 21+3+9) && (mousex >= 47+20))
  {
    editmode = EDIT_NAMES;
    if ((!prevmouseb) && (mouseb & MOUSEB_RIGHT))
    {
      enpos++;
      if (enpos > 2) enpos = 0;
    }
  }

  // Status panel
  if ((!prevmouseb) && (mousex == 7) && (mousey == 23+3+9))
  {
    if (mouseb & (MOUSEB_LEFT))
      if (epoctave < 7) epoctave++;
    if (mouseb & (MOUSEB_RIGHT))
      if (epoctave > 0) epoctave--;
  }
  if ((!prevmouseb) && (mousex <= 7) && (mousey == 24+3+9))
  {
    recordmode ^= 1;
  }
  for (c = 0; c < MAX_CHN; c++)
  {
    if ((!prevmouseb) && (mousey >= 23+3+9) && (mousex >= 59 + 7*c) && (mousex <= 64 + 7*c))
      mutechannel(c);
  }

  // Titlebar actions
  if (!menu)
  {
    if ((mousey == 0) && (!prevmouseb) && (mouseb == MOUSEB_LEFT))
    {
      if ((mousex >= 40+20) && (mousex <= 41+20))
      {
        usefinevib ^= 1;
      }
      if ((mousex >= 43+20) && (mousex <= 44+20))
      {
        optimizepulse ^= 1;
      }
      if ((mousex >= 46+20) && (mousex <= 47+20))
      {
        optimizerealtime ^= 1;
      }
      if ((mousex >= 49+20) && (mousex <= 52+20))
      {
        ntsc ^= 1;
        sound_init(b, mr, writer, hardsid, sidmodel, ntsc, multiplier, catweasel, interpolate, customclockrate);
      }
      if ((mousex >= 54+20) && (mousex <= 57+20))
      {
        sidmodel ^= 1;
        sound_init(b, mr, writer, hardsid, sidmodel, ntsc, multiplier, catweasel, interpolate, customclockrate);
      }
      if ((mousex >= 62+20) && (mousex <= 65+20)) editadsr();
      if ((mousex >= 67+20) && (mousex <= 68+20)) prevmultiplier();
      if ((mousex >= 69+20) && (mousex <= 70+20)) nextmultiplier();
    }
  }
  else
  {
    if ((!mousey) && (mouseb & MOUSEB_LEFT) && (!(prevmouseb & MOUSEB_LEFT)))
    {
      if ((mousex >= 0) && (mousex <= 5))
      {
        initsong(esnum, PLAY_BEGINNING);
        followplay = shiftpressed;
      }
      if ((mousex >= 7) && (mousex <= 15))
      {
        initsong(esnum, PLAY_POS);
        followplay = shiftpressed;
      }
      if ((mousex >= 17) && (mousex <= 26))
      {
        initsong(esnum, PLAY_PATTERN);
        followplay = shiftpressed;
      }
      if ((mousex >= 28) && (mousex <= 33))
        stopsong();
      if ((mousex >= 35) && (mousex <= 40))
        load();
      if ((mousex >= 42) && (mousex <= 47))
        save();
      if ((mousex >= 49) && (mousex <= 57))
        relocator();
      if ((mousex >= 59) && (mousex <= 64))
        onlinehelp(0,0);
      if ((mousex >= 66) && (mousex <= 72))
        clear();
      if ((mousex >= 74) && (mousex <= 79))
        quit();
    }
  }
}

void generalcommands(void)
{
  int c;

  switch(key)
  {
    case '?':
    case '-':
    if ((editmode != EDIT_NAMES) && (editmode != EDIT_ORDERLIST))
    {
      if (!((editmode == EDIT_INSTRUMENT) && (eipos == 9))) previnstr();
    }
    break;

    case '+':
    case '_':
    if ((editmode != EDIT_NAMES) && (editmode != EDIT_ORDERLIST))
    {
      if (!((editmode == EDIT_INSTRUMENT) && (eipos >= 9))) nextinstr();

    }
    break;

    case '*':
    if (editmode != EDIT_NAMES)
    {
      if (!((editmode == EDIT_INSTRUMENT) && (eipos >= 9)))
      {
        if (epoctave < 7) epoctave++;
      }
    }
    break;

    case '/':
    case '\'':
    if (editmode != EDIT_NAMES)
    {
      if (!((editmode == EDIT_INSTRUMENT) && (eipos >= 9)))
      {
        if (epoctave > 0) epoctave--;
      }
    }
    break;

    case '<':
    if (((editmode == EDIT_INSTRUMENT) && (eipos != 9)) || (editmode == EDIT_TABLES))
      previnstr();
    break;

    case '>':
    if (((editmode == EDIT_INSTRUMENT) && (eipos != 9)) || (editmode == EDIT_TABLES))
      nextinstr();
    break;

    case ';':
    for (c = 0; c < MAX_CHN; c++)
    {
      if (espos[c]) espos[c]--;
      if (espos[c] < esview)
      {
        esview = espos[c];
        eseditpos = espos[c];
      }
    }
    updateviewtopos();
    rewindsong();
    break;

    case ':':
    for (c = 0; c < MAX_CHN; c++)
    {
      if (espos[c] < songlen[esnum][c]-1)
        espos[c]++;
      if (espos[c] - esview >= VISIBLEORDERLIST)
      {
        esview = espos[c] - VISIBLEORDERLIST + 1;
        eseditpos = espos[c];
      }
    }
    updateviewtopos();
    rewindsong();
    break;

  }
  if (win_quitted) exitprogram = 1;
  switch(rawkey)
  {
    case KEY_ESC:
    if (!shiftpressed)
      quit();
    else
      clear();
    break;

    case KEY_KPMULTIPLY:
    if ((editmode != EDIT_NAMES) && (!key))
    {
      if (!((editmode == EDIT_INSTRUMENT) && (eipos >= 9)))
      {
        if (epoctave < 7) epoctave++;
      }
    }
    break;

    case KEY_KPDIVIDE:
    if ((editmode != EDIT_NAMES) && (!key))
    {
      if (!((editmode == EDIT_INSTRUMENT) && (eipos >= 9)))
      {
        if (epoctave > 0) epoctave--;
      }
    }
    break;

    case KEY_F12:
    case SDLK_HELP:
      onlinehelp(0,shiftpressed);
    break;

    case KEY_TAB:
    if (!shiftpressed) editmode++;
    else editmode--;
    if (editmode > EDIT_NAMES) editmode = EDIT_PATTERN;
    if (editmode < EDIT_PATTERN) editmode = EDIT_NAMES;
    break;

    case KEY_F1:
    initsong(esnum, PLAY_BEGINNING);
    followplay = shiftpressed;
    break;

    case KEY_F2:
    initsong(esnum, PLAY_POS);
    followplay = shiftpressed;
    break;

    case KEY_F3:
    initsong(esnum, PLAY_PATTERN);
    followplay = shiftpressed;
    break;

    case KEY_F4:
    if (shiftpressed)
      mutechannel(epchn);
    else
    stopsong();
    break;

    case KEY_F5:
    if (!shiftpressed)
      editmode = EDIT_PATTERN;
    else prevmultiplier();
    break;

    case KEY_F6:
    if (!shiftpressed)
      editmode = EDIT_ORDERLIST;
    else nextmultiplier();
    break;

    case KEY_F7:
    if (!shiftpressed)
    {
      if (editmode == EDIT_INSTRUMENT)
        editmode = EDIT_TABLES;
      else
        editmode = EDIT_INSTRUMENT;
    }
    else editadsr();
    break;

    case KEY_F8:
    if (!shiftpressed)
      editmode = EDIT_NAMES;
    else
    {
      sidmodel ^= 1;
      sound_init(b, mr, writer, hardsid, sidmodel, ntsc, multiplier, catweasel, interpolate, customclockrate);
    }
    break;

    case KEY_F9:
    relocator();
    break;

    case KEY_F10:
    load();
    break;

    case KEY_F11:
    save();
    break;
  }
}

void load(void)
{
  if ((editmode != EDIT_INSTRUMENT) && (editmode != EDIT_TABLES))
  {
    if (fileselector(songfilename, songpath, songfilter, "LOAD SONG", 0))
      loadsong();
  }
  else
  {
    if (einum)
    {
      if (fileselector(instrfilename, instrpath, instrfilter, "LOAD INSTRUMENT", 0))
        loadinstrument();
    }
  }
  key = 0;
  rawkey = 0;
}

void save(void)
{
  if ((editmode != EDIT_INSTRUMENT) && (editmode != EDIT_TABLES))
  {
    int done = 0;

    // Repeat until quit or save successful
    while (!done)
    {
      if (strlen(loadedsongfilename)) strcpy(songfilename, loadedsongfilename);
      if (fileselector(songfilename, songpath, songfilter, "SAVE SONG", 3))
        done = savesong();
      else done = 1;
    }
  }
  else
  {
    if (einum)
    {
      int done = 0;
      int useinstrname = 0;
      char tempfilename[MAX_FILENAME];

      // Repeat until quit or save successful
      while (!done)
      {
        if ((!strlen(instrfilename)) && (strlen(instr[einum].name)))
        {
          useinstrname = 1;
          strcpy(instrfilename, instr[einum].name);
          strcat(instrfilename, ".ins");
          strcpy(tempfilename, instrfilename);
        }

        if (fileselector(instrfilename, instrpath, instrfilter, "SAVE INSTRUMENT", 3))
          done = saveinstrument();
        else done = 1;

        if (useinstrname)
        {
          if (!strcmp(tempfilename, instrfilename))
            memset(instrfilename, 0, sizeof instrfilename);
        }
      }
    }
  }
  key = 0;
  rawkey = 0;
}

void quit(void)
{
  if ((!shiftpressed) || (mouseb))
  {
    printtextcp(38, 36, 15, "Really Quit (y/n)?");
    waitkey();
    printblank(20, 36, 39);
    if ((key == 'y') || (key == 'Y')) exitprogram = 1;
  }
  key = 0;
  rawkey = 0;
}

void clear(void)
{
  int cs = 0;
  int cp = 0;
  int ci = 0;
  int ct = 0;
  int cn = 0;

  printtextcp(38, 36, 15, "Optimize everything (y/n)?");
  waitkey();
  printblank(20, 36, 39);
  if ((key == 'y') || (key == 'Y'))
  {
    optimizeeverything(1, 1);
    key = 0;
    rawkey = 0;
    return;
  }

  printtextcp(38, 36, 15, "Clear orderlists (y/n)?");
  waitkey();
  printblank(20, 36, 39);
  if ((key == 'y') || (key == 'Y')) cs = 1;

  printtextcp(38, 36, 15, "Clear patterns (y/n)?");
  waitkey();
  printblank(20, 36, 39);
  if ((key == 'y') || (key == 'Y')) cp = 1;

  printtextcp(38, 36, 15, "Clear instruments (y/n)?");
  waitkey();
  printblank(20, 36, 39);
  if ((key == 'y') || (key == 'Y')) ci = 1;

  printtextcp(38, 36, 15, "Clear tables (y/n)?");
  waitkey();
  printblank(20, 36, 39);
  if ((key == 'y') || (key == 'Y')) ct = 1;

  printtextcp(38, 36, 15, "Clear songname (y/n)?");
  waitkey();
  printblank(20, 36, 39);
  if ((key == 'y') || (key == 'Y')) cn = 1;

  if (cp == 1)
  {
    int selectdone = 0;
    int olddpl = defaultpatternlength;

    printtext(29, 36, 15,"Pattern length:");
    while (!selectdone)
    {
      sprintf(textbuffer, "%02d ", defaultpatternlength);
      printtext(44, 36, 15, textbuffer);
      waitkey();
      switch(rawkey)
      {
        case KEY_LEFT:
        defaultpatternlength -= 7;
        case KEY_DOWN:
        defaultpatternlength--;
        if (defaultpatternlength < 1) defaultpatternlength = 1;
        break;

        case KEY_RIGHT:
        defaultpatternlength += 7;
        case KEY_UP:
        defaultpatternlength++;
        if (defaultpatternlength > MAX_PATTROWS) defaultpatternlength = MAX_PATTROWS;
        break;

        case KEY_ESC:
        defaultpatternlength = olddpl;
        selectdone = 1;
        break;

        case KEY_ENTER:
        selectdone = 1;
        break;
      }
    }
    printblank(20, 36, 39);
  }

  if (cs | cp | ci | ct | cn)
    memset(songfilename, 0, sizeof songfilename);
  clearsong(cs, cp, ci, ct, cn);

  key = 0;
  rawkey = 0;
}


void editadsr(void)
{
  eamode = 1;
  eacolumn = 0;

  for (;;)
  {
    waitkeymouse();

    if (win_quitted)
    {
      exitprogram = 1;
      key = 0;
      rawkey = 0;
      return;
    }

    if (hexnybble >= 0)
    {
      switch(eacolumn)
      {
        case 0:
        adparam &= 0x0fff;
        adparam |= hexnybble << 12;
        break;

        case 1:
        adparam &= 0xf0ff;
        adparam |= hexnybble << 8;
        break;

        case 2:
        adparam &= 0xff0f;
        adparam |= hexnybble << 4;
        break;

        case 3:
        adparam &= 0xfff0;
        adparam |= hexnybble;
        break;
      }
      eacolumn++;
    }

    switch(rawkey)
    {
      case KEY_F7:
      if (!shiftpressed) break;

      case KEY_ESC:
      case KEY_ENTER:
      case KEY_TAB:
      eamode = 0;
      key = 0;
      rawkey = 0;
      return;

      case KEY_BACKSPACE:
      if (!eacolumn) break;
      case KEY_LEFT:
      eacolumn--;
      break;

      case KEY_RIGHT:
      eacolumn++;
    }
    eacolumn &= 3;

    if ((mouseb) && (!prevmouseb))
    {
      eamode = 0;
      return;
    }
  }
}

void getparam(FILE *handle, unsigned *value)
{
  char *configptr;

  for (;;)
  {
    if (feof(handle)) return;
    fgets(configbuf, MAX_PATHNAME, handle);
    if ((configbuf[0]) && (configbuf[0] != ';') && (configbuf[0] != ' ') && (configbuf[0] != 13) && (configbuf[0] != 10)) break;
  }

  configptr = configbuf;
  if (*configptr == '$')
  {
    *value = 0;
    configptr++;
    for (;;)
    {
      char c = tolower(*configptr++);
      int h = -1;

      if ((c >= 'a') && (c <= 'f')) h = c - 'a' + 10;
      if ((c >= '0') && (c <= '9')) h = c - '0';

      if (h >= 0)
      {
        *value *= 16;
        *value += h;
      }
      else break;
    }
  }
  else
  {
    *value = 0;
    for (;;)
    {
      char c = tolower(*configptr++);
      int d = -1;

      if ((c >= '0') && (c <= '9')) d = c - '0';

      if (d >= 0)
      {
        *value *= 10;
        *value += d;
      }
      else break;
    }
  }
}

void getfloatparam(FILE *handle, float *value)
{
  char *configptr;

  for (;;)
  {
    if (feof(handle)) return;
    fgets(configbuf, MAX_PATHNAME, handle);
    if ((configbuf[0]) && (configbuf[0] != ';') && (configbuf[0] != ' ') && (configbuf[0] != 13) && (configbuf[0] != 10)) break;
  }

  configptr = configbuf;
  *value = 0.0f;
  sscanf(configptr, "%f", value);
}

void prevmultiplier(void)
{
  if (multiplier > 0)
  {
    multiplier--;
    sound_init(b, mr, writer, hardsid, sidmodel, ntsc, multiplier, catweasel, interpolate, customclockrate);
  }
}

void nextmultiplier(void)
{
  if (multiplier < 16)
  {
    multiplier++;
    sound_init(b, mr, writer, hardsid, sidmodel, ntsc, multiplier, catweasel, interpolate, customclockrate);
  }
}

void calculatefreqtable()
{
  double basefreq = (double)basepitch * (16777216.0 / 985248.0) * pow(2.0, 0.25) / 32.0;
  int c;

  for (c = 0; c < 8*12 ; c++)
  {
    double note = c;
    double freq = basefreq * pow(2.0, note/12.0);
    int intfreq = freq + 0.5;
    if (intfreq > 0xffff)
        intfreq = 0xffff;
    freqtbllo[c] = intfreq & 0xff;
    freqtblhi[c] = intfreq >> 8;
  }
}
