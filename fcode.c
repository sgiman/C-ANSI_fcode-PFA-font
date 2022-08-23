/*************************** FCODE.C ****************************
 *  Writing by S.Gasanov
 *  Ukrainian, Kiev, KM STUDIO
 *
 *  Encoding PostScript fonts type 3 (*.PFA)
 *******************************************************************
 *  Date create original:  17.08.95
 *  Last modification:     22.08.95
 *  Last modification:     03.09.2013
 *
 * Version for Barland C++ compilator (17.08.95)
 *-----------------------------------------------------------------
 * Version for Visual Studio 2010 
 * Modified by sgiman @ 2022
 *-----------------------------------------------------------------*/
#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "fcode.h"

#define  SIZE       256     /* Length input PFA-string */

/* Varieties */
static char  s_val[SIZE], cs[SIZE], st[SIZE];
static int flg = 0;

/* User functions */
void    encode_process(void);

char*   ffgets(char *, FILE *);
int     ffputs (char *, FILE *);
char*   strcopy (char *, char *);
void    pass1(void);
void    pass2(void);
void    encoding (void);
void    fexists (char *);
void    def_pfa_name(char *);
void    control_pfa(void);
void    msg_title(void);

static  char *file_in, *file_out;
static  FILE *fin, *fout, *ftmp;	/* Descriptors working files */

/*================================
              M A I N
=================================*/
main (int argc, char *argv[])
{
   
   int i;
	
   //====== PAUSE TIME ======
   fprintf (stderr, "******************************\n");	
   fprintf (stderr, "  PRESS ANY KEY TO CONTINUE!\n");	
   fprintf (stderr, "******************************\n");	
   system("pause");

   /* Testing arguments */
   if (argc < 2)  msg_title();						/* If not arguments */
   for(i=1; i <= argc; i++)
   {
      switch(argv[i][0])
      {
	case '-':
		   if(memcmp(argv[i],"-h",2) == 0)			/* if help */
		   {
		      msg_title();
		      break;
		   }
       case 'h':
       case '?':
		   if((memcmp(argv[i], "help", 4) == 0) ||  /* If help */
		      (memcmp(argv[i], "?", 1) == 0))
		   {
		      msg_title();
		      break;
		   }
       default:    break;

      } /* end switch */
   } /* end for */

   /*------- Open input PFA file --------*/
   file_in = argv[1];
   if ((fin = fopen (file_in, "rb")) == NULL)
   {
     fprintf(stderr, "\nCan't open input PFA file for reading...  %s\n", file_in);
     exit(1);
   }

   /*------- Open OUTPUT file  --------*/
   file_out = argv[2];
   if (argv[2] == NULL)
   {
      fprintf (stderr, "\n-- ARGV(2) = NULL --\n");
      _exit(0);
    }

   fexists(file_out);
   if ((fout = fopen(file_out, "wb")) == NULL)
   {
     fprintf(stderr, "\nCan't open output PFA file for writing: %s\n", file_out);
     _exit(0);
   }

   /*-------------------------------------
	       Encoding process
   --------------------------------------*/
   encoding();
   return(0);

} //******* M A I N - E N D *******


/*----------------------- 
        encoding() 
--------------------------*/
void encoding (void)
{
    /*------- Open TEMP file for writing --------*/
    if ((ftmp = fopen("$tmp.$$$", "wb")) == NULL)
    {
       fprintf(stderr, "\nCan't open tempore file for writing... \n");
       exit(1);
    }
    pass1();
    fclose(ftmp);

    /*------- Reopen TEMP file reading --------*/
    if ((ftmp = fopen("$tmp.$$$", "rb")) == NULL)
    {
       fprintf(stderr, "\nCan't reopen tempore file for reading... \n");
       exit(1);
    }
    pass2();

    printf ("\nEnd of PFA file");
    fprintf(stderr,"\nConverting complete...\n");
    fclose(ftmp);
    remove("$tmp.$$$");
    fclose(fin);
    fclose (fout);
}

/*------------------------ 
           pass1 
--------------------------*/
void pass1 (void)
{
     int i;

     /* Passing Latin charcters */

     fprintf (stderr, "\nPlease wait....PASS 1");
     flg = 0;
     while (ffgets(s_val, fin) != NULL)
     {  /* Passing Latin characters */
	i = 0;
	for (i=0; strcmp(cliptbl[i],"END") !=0 &&
	      strcmp(s_val, "") !=0  && flg; i++)
	{
	  if (strstr(s_val, cliptbl[i]))
	  {
	      while (strstr(s_val,">dc") == NULL)
	      {
		 /* printf("\nS_VAL=%s",s_val);;;; */
		 fprintf (stderr,"_");
		 ffgets(s_val, fin);
	      }
	      /* getchar();;;;; */
	      ffgets(s_val, fin);
	  }
	}

	ffputs(s_val, ftmp);
	if (strstr(s_val, "/CharStrings") &&
	    strstr(s_val, "dict def"))
	    flg = 1;
    }
}


/*-------------------- 
        pass2 
--------------------*/
void pass2 (void)
{
    int i;

    fprintf (stderr, "\nPlease wait....PASS 2");
    flg = 0;
    while (fgets(s_val, SIZE, ftmp) != NULL)
    {  /* Encoding Cyrilic characters */
       for (i=0; strcmp(ctab[i].c_old,"END") != 0 && flg; i++)
       {
	  if (strstr(s_val, ctab[i].c_old))
	     {
		strcopy (st, strstr(s_val," <"));
		strcopy (cs, ctab[i].c_new);
		strcopy (s_val, strcat(cs, st));
	     }
       }
       fprintf(stderr,".");
       fputs(s_val, fout);
       if (strstr(s_val, "/CharStrings") &&
	   strstr(s_val, "dict def"))
	   flg = 1;
    }
}

/*------------------ 
      fexists() 
--------------------*/
void fexists (char *fname)
{
     FILE *f;
     char *s;

     if ((f = fopen(fname,"r")) == NULL)
	{
	  fclose(f);
	  return;
	}
     else
     {
	   fprintf(stderr,"\nThe file \"%s\" already exists....", fname);
	   fprintf(stderr,"\nContinue - Y, Exit - N <Y>: ");
	   gets(s);
	   switch(s[0])
	   {
	      case 'y':
	      case 'Y':
	      case '\\n':
			  return;
			  break;
	      case 'n':
	      case 'N':
			  exit(-1);
			  break;
	      default:     return;
	   }
     }
}


/*---------------------- 
      msg_title() 
-----------------------*/
void msg_title(void)
{
    printf("\n**************************************************");
    fprintf(stderr,"\n(c) Copyright by S.Gasanov, August, 1995, v1.0");
    fprintf(stderr,"\nUkrainian, Kiev, KM STUDIO, tel/fax:(044)224-67-96");
    fprintf(stderr,"\nEncoding PostScript fonts type 3 (PFA)");
    fprintf(stderr,"\nUSAGE: fcode.exe input_pfa [output_pfa]");
    printf("\n**************************************************");
    printf("\nGoodbye...\n");
    exit(1);
}


/****************************** 
            ffgets 
*******************************/
char *ffgets(char *s, FILE *iop)
{
    register char c = 0;
    register char *str;

    str = s;
    while ((c = getc(iop)) != EOF)
    {
       if (( c == '\n') || (c == '\r'))
	   break;
       else
	  *str++ = c;
    }
    *str = '\0';

    return (c == EOF) ? NULL: s;
}


/****************************** 
            ffputs 
*******************************/
int ffputs (char *s, FILE *iop)
{
   int  c;

   while (c = *s++)
     putc(c,iop);

   putc (0xD, iop);
   putc (0xA, iop);

   return ferror(iop) ? EOF : 0;
}


/****************************** 
             strcopy 
*******************************/
char* strcopy (char *str1, char *str2)
{
    while ((*str1++ = *str2++) !='\0')
	 ;
    *--str1 = '\0';
    return str1;
}

/*-------------------- E N D --------------------*/
