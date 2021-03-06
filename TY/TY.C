/* ----------------------------------------------------------------------- *\
|
|				   TY Filter
|
|		    Copyright (c) 1985, 1990, all rights reserved
|				Brian W Johnson
|				   18-Aug-90
|
\* ----------------------------------------------------------------------- */

#include  <stdio.h>
#include  <stdlib.h>
#include  <ctype.h>
#include  <string.h>

#include  "fwild.h"

#ifndef TRUE
#define FALSE	  0
#define TRUE	  1
#endif

char	copyright [] =
"Copyright (c) 1985 by J & M Software, Dallas TX - All Rights Reserved";

char	 buffer [BUFSIZ];	/* Buffer for stdout */

char	swch = '-';		/* The switch character */

int	f_flag = FALSE;		/* Formfeed flag */
int	g_flag = TRUE;		/* Space between files flag */
int	l_flag = TRUE;		/* List name flag */
int	t_flag = FALSE;		/* Trim lines flag */
int	notfirst = FALSE;	/* Not first input file flag */

static void cantopen (char *);
void usage (void);
void help (void);
void dprint (char **);
static void process (FILE *, char *);

/* ----------------------------------------------------------------------- */
    void
main (argc, argv)
    int    argc;
    char  *argv [];

    {
    int    smode = FW_FILE;		/* File search mode attributes */
    char  *s;				/* Parser temporary */
    void  *hp  = NULL;			/* Pointer to wild file data block */
    char  *fnp = NULL;			/* Input file name pointer */
    FILE  *fp  = NULL;			/* Input file descriptor */


    setbuf(stdout, buffer);
    swch = egetswch();

    while (--argc > 0 && (*++argv)[0] == swch)
	for (s = argv[0] + 1; *s; s++)
	    switch (tolower(*s))
		{
		case 'f':
		    ++f_flag;
		    break;

		case 'g':
		    g_flag = FALSE;
		    break;

		case 'l':
		    l_flag = FALSE;
		    break;

		case 't':
		    ++t_flag;
		    break;

		case '?':
		    help();

		default:
		    usage();
		}

    if (argc == 0)
	process(stdin, "<stdin>");
    else
	{
	do  {
	    hp = fwinit(*argv, smode);		/* Process the input list */
	    if ((fnp = fwild(hp)) == NULL)
		cantopen(*argv);
	    else
		{
		do  {				/* Process one filespec */
		    if (fp = fopen(fnp, "r"))
			{
			process(fp, fnp);
			fclose(fp);
			}
		    else
			cantopen(fnp);
		    } while ((fnp = fwild(hp)));
		}
	    } while (*++argv);
	}
    }

/* ----------------------------------------------------------------------- */
    static void
cantopen (fnp)			/* Inform user of input failure */
    char  *fnp;			/* Input file name */

    {
    fprintf(stderr, "\7Unable to open input file: %s\n", fnp);
    }

/* ----------------------------------------------------------------------- */
    void
usage ()			/* Display usage documentation */

    {
    static char  *udoc [] =
	{
	"Usage:  ty  [%c?fglt]  [input_file_list]  [>output_file]",
	"        ty  %c?  for help",
	NULL
	};

    dprint(udoc);
    exit(1);
    }

/* ----------------------------------------------------------------------- */
    void
help ()				/* Display help documentation */

    {
    static char  *hdoc [] =
	{
	"Usage:  ty  [%c?fglt]  [input_file_list]  [>output_file]",
	"",
	"ty concatenates the files in the input_file_list (or stdin)",
	"to stdout (or output file).  By default, file names are",
	"announced between the files.",
	"",
	"    %cf  interposes formfeeds between files",
	"    %cg  does not interpose a three line gap between files",
	"    %cl  does not list file names as they are typed",
	"    %ct  trims off all trailing white space from each line",
	"",
	copyright,
	NULL
	};

    dprint(hdoc);
    exit(0);
    }

/* ----------------------------------------------------------------------- */
    void
dprint (dp)			/* Print documentation text */
    char  **dp;			/* Document array pointer */

    {
    while (*dp)
	{
	printf(*(dp++), swch);
	putchar('\n');
	}
    }

/* ----------------------------------------------------------------------- */
    static void
process (fp, fnp)		/* Process one input file */
    FILE  *fp;			/* Input file descriptor */
    char  *fnp;			/* Input file name */ 

    {
    char  record [512];
    char  *p;

    if (g_flag && notfirst)
	printf("\n\n\n");

    if (l_flag)
	printf("\nTyping file: %s\n", fnp);

    if (f_flag && notfirst)
	fputc('\f', stdout);

    while (fgets(record, sizeof record, fp))
	{
	if (t_flag)
	    {
	    if (record[0])
		{
		p = record + strlen(record) - 1;
		while (isspace(*p) && (*p != '\f') && (p-- != record))
		    ;
		*(++p) = '\n';
		*(++p) = '\0';
		}
	    }
	fputs(record, stdout);
	fflush(stdout);
	}

    notfirst = TRUE;
    }

/* ----------------------------------------------------------------------- */
