//
// GOATTRACKER v2 file selector
//

#define GFILE_C

#ifdef __WIN32__
#include <windows.h>
#endif

#include "gt2fork.h"

DIRENTRY direntry[MAX_DIRFILES];

void initpaths(void)
{
    int c;

    for (c = 0; c < MAX_DIRFILES; c++)
        direntry[c].name = NULL;

    memset(loadedsongfilename, 0, sizeof loadedsongfilename);
    memset(songfilename, 0, sizeof songfilename);
    memset(instrfilename, 0, sizeof instrfilename);
    memset(songpath, 0, sizeof songpath);
    memset(instrpath, 0, sizeof instrpath);
    memset(packedpath, 0, sizeof packedpath);
    strcpy(songfilter, "*.sng");
    strcpy(instrfilter, "*.ins");

    getcwd(songpath, MAX_PATHNAME);
    strcpy(instrpath, songpath);
    strcpy(packedpath, songpath);
}

int fileselector(char *name, char *path, char *filter, char *title, int filemode)
{
    int c, d, scrrep;
    int color;
    int files;
    int filepos = 0;
    int fileview = 0;
    int lastclick = 0;
    int lastfile = 0;
    int lowest;
    int exitfilesel;

    DIR *dir;
    struct dirent *de;
    struct stat st;
#ifdef __WIN32__
    char drivestr[] = "A:\\";
    char driveexists[26];
#endif
    char cmpbuf[MAX_PATHNAME];
    char tempname[MAX_PATHNAME];

    // Set initial path (if any)
    if (strlen(path)) chdir(path);

    // Scan for all existing drives
#ifdef __WIN32__
    for (c = 0; c < 26; c++)
    {
        drivestr[0] = 'A'+c;
        if (GetDriveType(drivestr) > 1) driveexists[c] = 1;
        else driveexists[c] = 0;
    }
#endif

    // Read new directory
NEWPATH:
    getcwd(path, MAX_PATHNAME);
    files = 0;
    // Deallocate old names
    for (c = 0; c < MAX_DIRFILES; c++)
    {
        if (direntry[c].name)
        {
            free(direntry[c].name);
            direntry[c].name = NULL;
        }
    }
#ifdef __WIN32__
    // Create drive letters
    for (c = 0; c < 26; c++)
    {
        if (driveexists[c])
        {
            drivestr[0] = 'A'+c;
            direntry[files].name = strdup(drivestr);
            direntry[files].attribute = 2;
            files++;
        }
    }
#endif

    // Process directory
#ifdef __amigaos__
    dir = opendir("");
#else
    dir = opendir(".");
#endif
    if (dir)
    {
        char *filtptr = strstr(filter, "*");
        if (!filtptr) filtptr = filter;
        else filtptr++;
        for (c = 0; c < strlen(filter); c++)
            filter[c] = tolower(filter[c]);

        while ((de = readdir(dir)))
        {
            if ((files < MAX_DIRFILES) && (strlen(de->d_name) < MAX_FILENAME))
            {
                direntry[files].name = strdup(de->d_name);
                direntry[files].attribute = 0;
                stat(de->d_name, &st);
                if (st.st_mode & S_IFDIR)
                {
                    direntry[files].attribute = 1;
                    files++;
                }
                else
                {
                    int c;
                    // If a file, must match filter
                    strcpy(cmpbuf, de->d_name);
                    if ((!strcmp(filtptr, "*")) || (!strcmp(filtptr, ".*")))
                        files++;
                    else
                    {
                        for (c = 0; c < strlen(cmpbuf); c++)
                            cmpbuf[c] = tolower(cmpbuf[c]);
                        if (strstr(cmpbuf, filtptr))
                            files++;
                        else
                        {
                            free(direntry[files].name);
                            direntry[files].name = NULL;
                        }
                    }
                }
            }
        }
        closedir(dir);
    }
    // Sort the filelist in a most horrible fashion
    for (c = 0; c < files; c++)
    {
        lowest = c;
        for (d = c+1; d < files; d++)
        {
            if (direntry[d].attribute < direntry[lowest].attribute)
            {
                lowest = d;
            }
            else
            {
                if (direntry[d].attribute == direntry[lowest].attribute)
                {
                    if (cmpname(direntry[d].name, direntry[lowest].name) < 0)
                    {
                        lowest = d;
                    }
                }
            }
        }
        if (lowest != c)
        {
            DIRENTRY swaptemp = direntry[c];
            direntry[c] = direntry[lowest];
            direntry[lowest] = swaptemp;
        }
    }

    // Search for the current filename
    fileview = 0;
    filepos = 0;
    for (c = 0; c < files; c++)
    {
        if ((!direntry[c].attribute) && (!cmpname(name, direntry[c].name)))
        {
            filepos = c;
        }
    }

    exitfilesel = -1;
    while (exitfilesel < 0)
    {
        int cc = cursorcolortable[cursorflash];
        if (cursorflashdelay >= 6)
        {
            cursorflashdelay %= 6;
            cursorflash++;
            cursorflash &= 3;
        }
        fliptoscreen();
        getkey();
        if (lastclick) lastclick--;

        if (win_quitted)
        {
            exitprogram = 1;
            for (c = 0; c < MAX_DIRFILES; c++)
            {
                if (direntry[c].name)
                {
                    free(direntry[c].name);
                    direntry[c].name = NULL;
                }
            }
            return 0;
        }

        if (mouseb)
        {
            // Cancel (click outside)
            if ((mousey < 3) || (mousey > 3+VISIBLEFILES+6) || (mousex <= 4+10) || (mousex >= 75+10))
            {
                if ((!prevmouseb) && (lastclick)) exitfilesel = 0;
            }

            // Select dir,name,filter
            if ((mousey >= 3+VISIBLEFILES+3) && (mousey <= 3+VISIBLEFILES+5) && (mousex >= 14+10) && (mousex <= 73+10))
            {
                filemode = mousey - (3+VISIBLEFILES+3) + 1;
                if ((filemode == 3) && (!prevmouseb) && (lastclick)) goto ENTERFILE;
            }

            // Select file from list
            if ((mousey >= 3) && (mousey <= 3+VISIBLEFILES+2) && (mousex >= 6+10) && (mousex <= 73+10))
            {
                filemode = 0;
                filepos = mousey - 4 - 1 + fileview;
                if (filepos < 0) filepos = 0;
                if (filepos > files-1) filepos = files - 1;

                if (!direntry[filepos].attribute)
                    strcpy(name, direntry[filepos].name);

                if ((!prevmouseb) && (lastclick) && (lastfile == filepos)) goto ENTERFILE;
            }
        }

        if (!filemode)
        {
            if (((key >= '0') && (key <= '0')) || ((key >= 'a') && (key <= 'z')) || ((key >= 'A') && (key <= 'Z')))
            {
                char k = tolower(key);
                int oldfilepos = filepos;

                for (filepos = oldfilepos + 1; filepos < files; filepos++)
                    if (tolower(direntry[filepos].name[0]) == k) break;
                if (filepos >= files)
                {
                    for (filepos = 0; filepos < oldfilepos; filepos++)
                        if (tolower(direntry[filepos].name[0]) == k) break;
                }

                if (!direntry[filepos].attribute) strcpy(name, direntry[filepos].name);
            }
        }

        switch(rawkey)
        {
        case KEY_ESC:
            exitfilesel = 0;
            break;

        case KEY_BACKSPACE:
            if (!filemode)
            {
#ifdef __amigaos__
                chdir("/");
#else
                chdir("..");
#endif
                goto NEWPATH;
            }
            break;

        case KEY_HOME:
            if (!filemode)
            {
                filepos = 0;
                if (!direntry[filepos].attribute) strcpy(name, direntry[filepos].name);
            }
            break;

        case KEY_END:
            if (!filemode)
            {
                filepos = files-1;
                if (!direntry[filepos].attribute) strcpy(name, direntry[filepos].name);
            }
            break;

        case KEY_PGUP:
            for (scrrep = PGUPDNREPEAT; scrrep; scrrep--)
            {
                if ((!filemode) && (filepos > 0))
                {
                    filepos--;
                    if (!direntry[filepos].attribute) strcpy(name, direntry[filepos].name);
                }
            }
            break;

        case KEY_UP:
            if ((!filemode) && (filepos > 0))
            {
                filepos--;
                if (!direntry[filepos].attribute) strcpy(name, direntry[filepos].name);
            }
            break;

        case KEY_PGDN:
            for (scrrep = PGUPDNREPEAT; scrrep; scrrep--)
            {
                if ((!filemode) && (filepos < files-1))
                {
                    filepos++;
                    if (!direntry[filepos].attribute) strcpy(name, direntry[filepos].name);
                }
            }
            break;

        case KEY_DOWN:
            if ((!filemode) && (filepos < files-1))
            {
                filepos++;
                if (!direntry[filepos].attribute) strcpy(name, direntry[filepos].name);
            }
            break;

        case KEY_TAB:
            if (!shiftpressed)
            {
                filemode++;
                if (filemode > 3) filemode = 0;
            }
            else
            {
                filemode--;
                if (filemode < 0) filemode = 3;
            }
            break;

        case KEY_ENTER:
ENTERFILE:
            switch(filemode)
            {
            case 0:
                switch (direntry[filepos].attribute)
                {
                case 0:
                    strcpy(name, direntry[filepos].name);
                    exitfilesel = 1;
                    break;

                case 1:
                    chdir(direntry[filepos].name);
                    goto NEWPATH;

                case 2:
                    strcpy(tempname, direntry[filepos].name);
                    if (strlen(tempname))
                    {
                        if (tempname[strlen(tempname)-1] != '\\')
                            strcat(tempname, "\\");
                    }
                    chdir(tempname);
                    goto NEWPATH;
                }
                break;

            case 1:
                chdir(path);
            case 2:
                filemode = 0;
                goto NEWPATH;

            case 3:
                exitfilesel = 1;
                break;
            }
            break;
        }

        switch(filemode)
        {
        case 1:
            editstring(path, MAX_PATHNAME);
            break;

        case 2:
            editstring(filter, MAX_FILENAME);
            break;

        case 3:
            editstring(name, MAX_FILENAME);
            break;
        }

        // Validate filelist view
        if (filepos < fileview) fileview = filepos;
        if (filepos - fileview >= VISIBLEFILES) fileview = filepos - VISIBLEFILES + 1;

        // Refresh fileselector display
        if (isplaying()) printstatus();
        for (c = 0; c < VISIBLEFILES+7; c++)
        {
            printblank(dpos.loadboxX-(MAX_FILENAME+10)/2, dpos.loadboxY+c, MAX_FILENAME+10);
        }
        // drawbox(50-(MAX_FILENAME+10)/2, 3, 15, MAX_FILENAME+10, VISIBLEFILES+7);
        drawbox(dpos.loadboxX-(MAX_FILENAME+10)/2, dpos.loadboxY, colscheme.boxcolor, MAX_FILENAME+10, VISIBLEFILES+7);
        // printblankc(50-(MAX_FILENAME+10)/2+1, 4, 15+16,MAX_FILENAME+8);
        printblankc(dpos.loadboxX-(MAX_FILENAME+10)/2+1, dpos.loadboxY+1, colscheme.title,MAX_FILENAME+8);
        // printtext(50-(MAX_FILENAME+10)/2+1, 4, 15+16, title);
        printtext(dpos.loadboxX-(MAX_FILENAME+10)/2+1, dpos.loadboxY+1, colscheme.title, title);

        for (c = 0; c < VISIBLEFILES; c++)
        {
            if ((fileview+c >= 0) && (fileview+c < files))
            {
                switch (direntry[fileview+c].attribute)
                {
                case 0:
                    sprintf(textbuffer, "%-60s        ", direntry[fileview+c].name);
                    break;

                case 1:
                    sprintf(textbuffer, "%-60s   <DIR>", direntry[fileview+c].name);
                    break;

                case 2:
                    sprintf(textbuffer, "%-60s   <DRV>", direntry[fileview+c].name);
                    break;
                }
            }
            else
            {
                sprintf(textbuffer, "                                                                    ");
            }
            color = colscheme.normal;
            if ((fileview+c) == filepos) color = colscheme.edit;
            textbuffer[68] = 0;
            printtext(dpos.loadboxX-(MAX_FILENAME+10)/2+1, dpos.loadboxY+2+c, color, textbuffer);
            if ((!filemode) && ((fileview+c) == filepos)) printbg(dpos.loadboxX-(MAX_FILENAME+10)/2+1, dpos.loadboxY+2+c, cc, 68);
        }
        //
        printbg(dpos.loadboxX-(MAX_FILENAME+10)/2+1, dpos.loadboxY+2+VISIBLEFILES, colscheme.bgcolor, 68);
        //
        printtext(dpos.loadboxX-(MAX_FILENAME+10)/2+1, dpos.loadboxY+3+VISIBLEFILES, colscheme.title, "PATH:   ");
        sprintf(textbuffer, "%-60s", path);
        textbuffer[MAX_FILENAME] = 0;
        color = colscheme.normal;
        if (filemode == 1) color = colscheme.edit;
        printtext(dpos.loadboxX-(MAX_FILENAME+10)/2+9, dpos.loadboxY+3+VISIBLEFILES, color, textbuffer);
        if ((filemode == 1) && (strlen(path) < MAX_FILENAME)) printbg(dpos.loadboxX-(MAX_FILENAME+10)/2+9+strlen(path), dpos.loadboxY+3+VISIBLEFILES, cc, 1);

        printtext(dpos.loadboxX-(MAX_FILENAME+10)/2+1, dpos.loadboxY+4+VISIBLEFILES, colscheme.title, "FILTER: ");
        sprintf(textbuffer, "%-60s", filter);
        textbuffer[MAX_FILENAME] = 0;
        color = colscheme.normal;
        if (filemode == 2) color = colscheme.edit;
        printtext(dpos.loadboxX-(MAX_FILENAME+10)/2+9, dpos.loadboxY+4+VISIBLEFILES, color, textbuffer);
        if (filemode == 2) printbg(dpos.loadboxX-(MAX_FILENAME+10)/2+9+strlen(filter), dpos.loadboxY+4+VISIBLEFILES, cc, 1);

        printtext(dpos.loadboxX-(MAX_FILENAME+10)/2+1, dpos.loadboxY+5+VISIBLEFILES, colscheme.title, "NAME:   ");
        sprintf(textbuffer, "%-60s", name);
        textbuffer[MAX_FILENAME] = 0;
        color = colscheme.normal;
        if (filemode == 3) color = colscheme.edit;
        printtext(dpos.loadboxX-(MAX_FILENAME+10)/2+9, dpos.loadboxY+5+VISIBLEFILES, color, textbuffer);
        if (filemode == 3) printbg(dpos.loadboxX-(MAX_FILENAME+10)/2+9+strlen(name), dpos.loadboxY+5+VISIBLEFILES, cc, 1);

        if (win_quitted) exitfilesel = 0;

        if ((mouseb) && (!prevmouseb))
        {
            lastclick = DOUBLECLICKDELAY;
            lastfile = filepos;
        }
    }

    // Deallocate all used names
    for (c = 0; c < MAX_DIRFILES; c++)
    {
        if (direntry[c].name)
        {
            free(direntry[c].name);
            direntry[c].name = NULL;
        }
    }

    // Restore screen & exit
    printmainscreen();
    return exitfilesel;
}

void editstring(char *buffer, int maxlength)
{
    int len = strlen(buffer);

    if (key)
    {
        if ((key >= 32) && (key < 256))
        {
            if (len < maxlength-1)
            {
                buffer[len] = key;
                buffer[len+1] = 0;
            }
        }
        if ((key == 8) && (len > 0))
        {
            buffer[len-1] = 0;
        }
    }
}

int cmpname(char *string1, char *string2)
{
    for (;;)
    {
        unsigned char char1 = tolower(*string1++);
        unsigned char char2 = tolower(*string2++);
        if (char1 < char2) return -1;
        if (char1 > char2) return 1;
        if ((!char1) || (!char2)) return 0;
    }
}

