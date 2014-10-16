/*
 * MSXLIST.COM for MS-DOS Version 1.07
 * Copyright 1989-1990 by [ITOCHI]
 * Reright 1992-1996 by (YMD)A
 */

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <io.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#ifdef	TURBOC
#define	_write(fd,buf,len)	write(fd,buf,len)
#define	_read(fd,buf,len)	read(fd,buf,len)
typedef unsigned int uint;
typedef unsigned char uchar;
typedef signed char schar;
#endif

#define	RBUFSIZ		28815	/* �ǂݍ��݃o�b�t�@�T�C�Y */
#define	WBUFSIZ		4096	/* �������݃o�b�t�@�T�C�Y */

typedef int     FD;

static uchar   *tbl= "\
End\0\
For\0\
Next\0\
Data\0\
Input\0\
Dim\0\
Read\0\
Let\0\
GoTo\0\
Run\0\
If\0\
Restore\0\
GoSub\0\
Return\0\
Rem\0\
Stop\0\
Print\0\
Clear\0\
List\0\
New\0\
On\0\
Wait\0\
Def\0\
Poke\0\
Cont\0\
CSave\0\
CLoad\0\
Out\0\
Lprint\0\
Llist\0\
Cls\0\
Width\0\
Else\0\
TrOn\0\
TrOff\0\
Swap\0\
Erase\0\
Error\0\
Resume\0\
Delete\0\
Auto\0\
ReNum\0\
DefStr\0\
DefInt\0\
DefSng\0\
DefDbl\0\
Line\0\
Open\0\
Field\0\
Get\0\
Put\0\
Close\0\
Load\0\
Merge\0\
Files\0\
LSet\0\
RSet\0\
Save\0\
LFiles\0\
Circle\0\
Color\0\
Draw\0\
Paint\0\
Beep\0\
Play\0\
PSet\0\
PReset\0\
Sound\0\
Screen\0\
VPoke\0\
Sprite\0\
VDP\0\
Base\0\
Call\0\
Time\0\
Key\0\
Max\0\
Motor\0\
BLoad\0\
BSave\0\
Dsko$\0\
Set\0\
Name\0\
Kill\0\
Ipl\0\
Copy\0\
Cmd\0\
Locate\0\
To\0\
Then\0\
Tab(\0\
Step\0\
Usr\0\
Fn\0\
Spc\0\
Not\0\
Erl\0\
Err\0\
String$\0\
Using\0\
Instr\0\
'\0\
Varptr\0\
Csrlin\0\
Attr\0\
Dski$\0\
Off\0\
InKey$\0\
Point\0\
>\0\
=\0\
<\0\
+\0\
-\0\
*\0\
/\0\
^\0\
And\0\
Or\0\
Xor\0\
Eqv\0\
Imp\0\
Mod\0\
\\\0\
";

static uchar   *tbl_ff = "\
Left$\0\
Right$\0\
Mid$\0\
Sgn\0\
Int\0\
Abs\0\
Sqr\0\
Rnd\0\
Sin\0\
Log\0\
Exp\0\
Cos\0\
Tan\0\
Atn\0\
Fre\0\
Inp\0\
Pos\0\
Len\0\
Srt$\0\
Val\0\
Asc\0\
Chr$\0\
Peek\0\
VPeek\0\
Space$\0\
Oct$\0\
Hex$\0\
LPos\0\
Bin$\0\
CInt\0\
CSng\0\
CDbl\0\
Fix\0\
Stick\0\
Strig\0\
Pdl\0\
Pad\0\
DskF\0\
FPos\0\
CvI\0\
CvS\0\
CvD\0\
EoF\0\
LoC\0\
LoF\0\
MkI$\0\
MkS$\0\
MkD$\0\
\0";

static uchar    dc_flag;	/* �h�h�t���O		 */
static uchar    rem_flag;	/* �q�d�l�t���O		 */
static uchar    mjflag;		/* Print Mode	 */
static uchar    readf;		/* ���ǑΏ�		 */
static uchar	files;		/* Files */

static uchar   *buffer;		/* �ǂݍ��݃o�b�t�@ */
static uchar   *save;
static uchar   *wbuf;		/* �������݃o�b�t�@ */
static uchar   *wsave;

static FD       fd;
static FD       out;

void            fileopen(uchar *);
uchar          *memalloc(size_t);
uchar           flags(uchar *);
uchar           conv(void);
void            sngdbl(uchar);
void            command(uchar);
void            com_print(uchar, register uchar *);
void            koron(void);
void            err(uchar *, uchar *);
uchar           intprt(uint, uchar);
void            putb(uchar);
void            put(uchar *);
uint            getword(void);
uchar           getbyte(void);
void            write2(uchar *, int);

void main(uint argc, uchar * argv[])
{
    uint            i;
    uchar           j;

    buffer = memalloc(RBUFSIZ);
    wbuf = memalloc(WBUFSIZ);

    save = buffer;
    wsave = wbuf;

    if (argc <= 1) {
	out = 2;
	put("\
MSXLIST version 1.07 by ITOCHI / (YMD)A\n\
\n\
MSX-BASIC �^���̃t�@�C����W���o�͂���v���O�����ł��B\n\
Usage : MSXLIST [-Mn] filename\n\
");
	exit(1);
    }

    mjflag = (uchar) 1;
    files = (uchar)argc;
    files--;
    for (i = 1; i < argc; i++) {	/* �I�v�V�����T�[�` */
	mjflag += flags(argv[i]) - (mjflag * (flags(argv[i]) != (uchar)0) );
	if ((flags(argv[i]))) files--;
    }
    for (i = 1; i < argc; i++) {
	if ((flags(argv[i])) != (uchar)0 ) continue;
	readf = (uchar) 0;
	fileopen(argv[i]);	/* �e��t�@�C�����J�� */
	while (getword()) {
	    dc_flag = (uchar) 0;		/* �h�h�t���O���N���A 	 */
	    rem_flag = (uchar) 0;		/* �q�d�l�t���O���N���A	 */
	    intprt(getword(), (uchar) 10);	/* �s�ԍ���\��		 */
	    putb(' ');

	    while (!conv());	/* ��͕����Ăяo�� */
	    putb((uchar) 10);
	}
	close(fd);		/* �L���b�V���t���b�V�� */
	write2(NULL, -1);

//	close(out);
    }
}

uchar          *memalloc(size_t byte)		/* �������m�� */
{
    uchar          *buf;

    if (((buf = (uchar *) malloc(byte))) == NULL) {
	err("Out of memory", "");
    }
    *buf = '\0';
    return (buf);
}

uchar		mjchkc(uchar c)
{
	switch(mjflag) {
		case	(uchar)1: return toupper(c);
		case	(uchar)2: return tolower(c);
	}
	return (c);
}

uchar		*mjchk(uchar *s)
{
	uchar	*ss;

	ss = s;
	do {
		*s = mjchkc(*s);
	} while(*s++ != '\0');
	return (ss);
}
	
void fileopen(uchar * argv)
{
    buffer = save;

    if ((fd = open(argv, O_RDONLY | O_BINARY)) == -1) {
    				/* �\�[�X�t�@�C�����J�� */
	err("�t�@�C���I�[�v���G���[:", argv);
    }
    if (getbyte() != (uchar) 0xff) {
	err("MSX-BASIC �^���̃t�@�C���ł͂���܂���:", argv);
    }
    out = 1;
    if (files != (uchar)1) {
	put("\n\33[7m< ");
	put(strupr(argv));
	put(" >\33[m\n\n");
    }
}

uchar flags(uchar * argument)
{
    if ((*argument == '/' || *argument == '-')) {
	if (toupper(*(argument+1)) != 'M')
		err("Option Error:", argument);
	switch(*(argument+2)) {
		case	'0':	return (uchar)1;
		case	'1':	return (uchar)2;
		case	'2':	return (uchar)3;
		case	'\0':	return (uchar)2;
	}
    }
    return (uchar)0;
}

uchar conv()
{
    uchar           state;	/* BASIC �X�e�[�g�����g	 */

    if ((state = getbyte()) == (uchar) 0)
	return ((uchar) 1);
    if ((rem_flag || dc_flag) != (uchar) 0) {
	if (state == (uchar) 0x22)
	    dc_flag = (uchar) 0;
	putb(state);
    } else
	switch (state) {
	case 0x0b: {		/* �W�i��	*/
	    put(mjchk("&O"));
	    intprt(getword(), (uchar) 8);
	    break;
	}
	case 0x0c: {		/* 16�i��	*/
	    put(mjchk("&H"));
	    intprt(getword(), (uchar) 16);
	    break;
	}
	case 0x0e:		/* �s�ԍ�/����	*/
	case 0x1c: {
	    intprt(getword(), (uchar) 10);
	    break;
	}
	case 0x0f: {		/* 10-255�̐���	*/
	    intprt(getbyte(), (uchar) 10);
	    break;
	}
	case 0x1d: {		/* �P���x����	*/
	    sngdbl((uchar) 4);
	    break;
	}
	case 0x1f: {		/* �{���x����	*/
	    sngdbl((uchar) 8);
	    break;
	}
	case 0x3a: {		/* :		*/
	    koron();
	    break;
	}
	case 0xff: {		/* �֐�		*/
	    com_print(getbyte(), tbl_ff);
	    break;
	}
	default:
	    if (state < (uchar) 0x1b)
		putb((int) (state + (uchar) 0x1f));	/* 0-9 �̐���	*/
	    else if (state < (uchar) 0x81) {
		putb(state);				/* ����		*/
		if (state == (uchar) 0x22)
		    dc_flag = (uchar) 1;
	    } else {					/* ��ʃX�e�[�g	*/
		if (state == (uchar) 0x84)
		    rem_flag = (uchar) 255;
		com_print(state, tbl);
	    }
	}
    return ((uchar) 0);
}

void sngdbl(uchar len)
{
    uchar           buf, doubles[14];
    schar           i;
    schar           valpow;

    valpow = (schar) getbyte();	/* �w�����ǂݏo�� */

    for (i = (uchar) 1; i < len; i++) {	/* �������ďo */
	doubles[i * 2 - 2] = (((buf = getbyte()) >> 4) &
			      (uchar) 0x0f) + (uchar) 0x30;
	doubles[i * 2 - 1] = (buf & (uchar) 0x0f) + (uchar) 0x30;
    }
    doubles[len * 2 - 2] = '\0';/* �O�k�� */
    for (i = len * (uchar) 2 - (uchar) 3; i >= (uchar) 0; i--) {
	if (doubles[i] == '0')
	    doubles[i] = '\0';
	else
	    break;
    }

    valpow -= (schar) 0x40;	/* �w�����ɂ���ď�����U�蕪 */

    if (valpow > (schar) - 2 && valpow < (schar) 15) {

	if (valpow < (schar) 1) {
	    putb('.');
	    for (i = (schar) 0; i > valpow; i--)
		putb('0');

	    i = (schar) 0;
	    while (doubles[i++] != '\0')
		putb(doubles[i - 1]);

	} else {
	    i = 0;

	    while (doubles[i++] != '\0') {
		putb(doubles[i - 1]);
		if (--valpow == (schar) 0 && doubles[i] != '\0')
		    putb('.');
	    }
	    while (valpow-- > (schar) 0)
		putb('0');
	}
    } else {
	i = 0;
	while (doubles[i++] != '\0') {
	    putb(doubles[i - 1]);
	    if (i == (schar) 1 && doubles[i] != '\0')
		putb('.');
	}
	putb(mjchkc('E'));
	if (--valpow < (schar) 0) {
	    putb('-');
	    valpow = -valpow;
	} else
	    putb('+');

	putb(valpow / (schar) 10 + (schar) 0x30);
	putb(valpow % (schar) 10 + (schar) 0x30);
    }
}

void koron()
{
    switch (getbyte()) {
	case 0x8f:{
	    rem_flag = (uchar) 255;	/* REM */
	    if (getbyte() == (uchar) 0xe6)
		com_print((uchar) 0xe6, tbl);
	    else {
		buffer--;
		com_print((uchar) 0x8f, tbl);
	    } break;
	}
    case 0xa1:{
	    com_print((uchar) 0xa1, tbl);
	    break;
	}
    default:{
	    buffer--;
	    putb(':');
	}
    }
}

void com_print(uchar func, register uchar * table)
{
    func -= (uchar) 0x81;
    while (func) {
	while (*(table++) != '\0');
	func--;
	continue;
    }
    put(mjchk(table));
}

void err(uchar * message, uchar * filename)
{
    out = 2;
    put("msxlist>");
    put(message);
    put(strupr(filename));
    putb((uchar) 10);
    exit(1);
}

uchar intprt(uint n, uchar radix)
{
    uchar           len, r;

    len = (n >= radix) ? intprt(n / radix, radix) : 0;
    r = (uchar) (n % radix);
    putb(mjchkc(
    	(uchar) ((r < (uchar) 10) ? r + (uchar) 0x30 : r + (uchar) 0x37)));
    return (len + 1);
}

uchar getbyte()
{
    if ((buffer >= save + RBUFSIZ) || !readf) {
	readf = (uchar) 1;
	buffer = save;
	_read(fd, buffer, RBUFSIZ);
    }
    return (*(buffer++));
}

uint getword()
{
    return (getbyte() + getbyte() * 256);
}

void putb(uchar str)
{
//    if (str == (uchar) 10)
//	putb((uchar) 13);
    write2((uchar *) & str, (uint) 1);
}

void put(uchar * str)
{
    write2(str, strlen(str));
}

void write2(uchar * str, int len)
{
    _write(out, str, len);
//    if (out == 2)
//	_write(out, str, len);
//    else {
//	if ((wsave + WBUFSIZ < wbuf + 1024) || (len == -1)) {
//	    _write(out, wsave, (uint)wbuf - (uint)wsave);
//	    wbuf = wsave;
//	}
//	if (str != NULL) {
//	    memcpy(wbuf, str, len);
//	    wbuf = wbuf + len;
//	}
//    }
}