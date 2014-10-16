/*
 * MSXLIST.COM for MS-DOS Version 1.08
 * Copyright 1989-1990 by [ITOCHI]
 * Reright 1992-1996 & 2012 by (YMD)A @nanakochi123456
 * This code is UTF-8
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#define		_write(fd,buf,len)		write(fd,buf,len)
#define		_read(fd,buf,len)		read(fd,buf,len)

typedef unsigned short unsignedint;
typedef unsigned char unsignedchar;
typedef signed char signedchar;

#define		RBUFSIZ				65536		/* 読み込みバッファサイズ */
#define		WBUFSIZ				65536		/* 書き込みバッファサイズ */
#define		REPLACEBUF			4096		/* 書き換え用バッファサイズ */

#ifndef		O_BINARY
#define		O_BINARY		0				/* ダミー */
#endif

typedef int	 FD;

unsignedchar   *tbl= "\
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

unsignedchar   *tbl_ff = "\
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

#define	CONVCHARS	62+31
unsignedchar charconv[CONVCHARS][3] = {
	 {0x80,0x81,0xA5} /* スペード */
	,{0x81,0x81,0xA4} /* ハート */
	,{0x82,0x81,0xA3} /* クラブ */
	,{0x83,0x81,0x9E} /* ダイア */
	,{0x84,0x81,0x9B} /* ○ */
	,{0x85,0x81,0x9C} /* ● */
	,{0x86,0x82,0xF0} /* を */
	,{0x87,0x82,0x9F} /* ぁ */
	,{0x88,0x82,0xA1} /* ぃ */
	,{0x89,0x82,0xA3} /* ぅ */
	,{0x8A,0x82,0xA5} /* ぇ */
	,{0x8B,0x82,0xA7} /* ぉ */
	,{0x8C,0x82,0xE1} /* ゃ */
	,{0x8D,0x82,0xE3} /* ゅ */
	,{0x8E,0x82,0xE5} /* ょ */
	,{0x8F,0x82,0xC1} /* っ */
	,{0x90,0x00,0x00} /* ? */
	,{0x91,0x82,0xA0} /* あ */
	,{0x92,0x82,0xA2} /* い */
	,{0x93,0x82,0xA4} /* う */
	,{0x94,0x82,0xA6} /* え */
	,{0x95,0x82,0xA8} /* お */
	,{0x96,0x82,0xA9} /* か */
	,{0x97,0x82,0xAB} /* き */
	,{0x98,0x82,0xAD} /* く */
	,{0x99,0x82,0xAF} /* け */
	,{0x9A,0x82,0xB1} /* こ */
	,{0x9B,0x82,0xB3} /* さ */
	,{0x9C,0x82,0xB5} /* し */
	,{0x9D,0x82,0xB7} /* す */
	,{0x9E,0x82,0xB9} /* せ */
	,{0x9F,0x82,0xBB} /* そ */
	,{0xE0,0x82,0xBD} /* た */
	,{0xE1,0x82,0xBF} /* ち */
	,{0xE2,0x82,0xC2} /* つ */
	,{0xE3,0x82,0xC4} /* て */
	,{0xE4,0x82,0xC6} /* と */
	,{0xE5,0x82,0xC8} /* な */
	,{0xE6,0x82,0xC9} /* に */
	,{0xE7,0x82,0xCA} /* ぬ */
	,{0xE8,0x82,0xCB} /* ね */
	,{0xE9,0x82,0xCC} /* の */
	,{0xEA,0x82,0xCD} /* は */
	,{0xEB,0x82,0xD0} /* ひ */
	,{0xEC,0x82,0xD3} /* ふ */
	,{0xED,0x82,0xD6} /* へ */
	,{0xEE,0x82,0xD9} /* ほ */
	,{0xEF,0x82,0xDC} /* ま */
	,{0xF0,0x82,0xDD} /* み */
	,{0xF1,0x82,0xDE} /* む */
	,{0xF2,0x82,0xDF} /* め */
	,{0xF3,0x82,0xE0} /* も */
	,{0xF4,0x82,0xE2} /* や */
	,{0xF5,0x82,0xE4} /* ゆ */
	,{0xF6,0x82,0xE6} /* よ */
	,{0xF7,0x82,0xE7} /* ら */
	,{0xF8,0x82,0xE8} /* り */
	,{0xF9,0x82,0xE9} /* る */
	,{0xFA,0x82,0xEA} /* れ */
	,{0xFB,0x82,0xEB} /* ろ */
	,{0xFC,0x82,0xED} /* わ */
	,{0xFD,0x82,0xF1} /* ん */

	,{0x41,0x8C,0x8E} /* 月 */
	,{0x42,0x89,0xCE} /* 火 */
	,{0x43,0x90,0x85} /* 水 */
	,{0x44,0x96,0x8D} /* 木 */
	,{0x45,0x8B,0xE0} /* 金 */
	,{0x46,0x93,0x79} /* 土 */
	,{0x47,0x93,0xFA} /* 日 */
	,{0x48,0x94,0x4E} /* 年 */
	,{0x49,0x89,0x7E} /* 円 */
	,{0x4A,0x8E,0x9E} /* 時 */
	,{0x4B,0x95,0xAA} /* 分 */
	,{0x4C,0x95,0x62} /* 秒 */
	,{0x4D,0x95,0x53} /* 百 */
	,{0x4E,0x90,0xE7} /* 千 */
	,{0x4F,0x96,0x9C} /* 万 */
	,{0x50,0x83,0xCE} /* π */
	,{0x51,0x84,0xA8} /* ┴ */
	,{0x52,0x84,0xA6} /* ┬ */
	,{0x53,0x84,0xA7} /* ┤ */
	,{0x54,0x84,0xA5} /* ├ */
	,{0x55,0x84,0xA9} /* ┼ */
	,{0x56,0x84,0xA0} /* │ */
	,{0x57,0x84,0x9F} /* ─ */
	,{0x58,0x84,0xA1} /* ┌ */
	,{0x59,0x84,0xA2} /* ┐ */
	,{0x5A,0x84,0xA4} /* └ */
	,{0x5B,0x84,0xA3} /* ┘ */
	,{0x5C,0x81,0x7E} /* × */
	,{0x5D,0x91,0xE5} /* 大 */
	,{0x5E,0x92,0x86} /* 中 */
	,{0x5F,0x8F,0xAC} /* 小 */
};

char *str_files="============================";
char *str_lf="\n";

static unsignedchar	dc_flag;		/* ””フラグ				 */
static unsignedchar	rem_flag;		/* ＲＥＭフラグ				 */
static unsignedchar	mjflag;				/* Print Mode		 */
static unsignedchar	readf;				/* 初読対処				 */
static unsignedchar		files;				/* Files */

static unsignedchar   *buffer;				/* 読み込みバッファ */
static unsignedchar   *save;
static unsignedchar   *wbuf;				/* 書き込みバッファ */
static unsignedchar   *wsave;

static unsignedchar   *mjbuf;				/* 文字書き換えバッファ */

static int	   fd;
static int	   out;
FILE	*fp;

int	outconv;
int	spc;
int help;

void			fileopen(unsignedchar *);
unsignedchar		  *memalloc(size_t);
unsignedchar		   flags(unsignedchar *);
unsignedchar		   conv(void);
void			sngdbl(unsignedchar);
void			command(unsignedchar);
void			com_print(unsignedchar, register unsignedchar *);
void			koron(void);
void			err(unsignedchar *, unsignedchar *);
unsignedchar		   intprt(unsignedint, unsignedchar);
void			putb(unsignedchar);
void			put(unsignedchar *);
unsignedint			getword(void);
unsignedchar		   getbyte(void);
void			write2(unsignedchar *, int);
void			write3(unsignedchar);
void			insspc(void);

char *usage="\
MSXLIST version 1.08 by ITOCHI / (YMD)A\n\
\n\
Output MSX-BASIC Binary to stdout\n\
Usage : MSXLIST [-Mn][-K] [-S] [-H] filename\n\
";

int main(unsignedint argc, unsignedchar * argv[])
{
	unsignedint			i;
	int	flg;

	buffer = memalloc(RBUFSIZ);
	wbuf = memalloc(WBUFSIZ);
	mjbuf = memalloc(REPLACEBUF);

	save = buffer;
	wsave = wbuf;
	outconv=0;
	spc=0;
	help=0;

	if (argc <= 1) {
		out = 2;
		put(usage);
		exit(1);
	}

	mjflag = (unsignedchar) 1;
	files = (unsignedchar)argc;
	files--;
	for (i = 1; i < argc; i++) {		/* オプションサーチ */
		flg=flags(argv[i]);
		if(flg) {
			if(flg < 128) {
				mjflag = flg;
			}
			files--;
		}
	}
	if(help) {
		out = 2;
		put(usage);
		put("\
\n\
Options:\n\
-M0 : Display in upper case letters.\n\
-M1 : Display in lower case letters\n\
-M2 : Capitalizes the first letter only.\n\
-K  : Convert MSX code to Shift-JIS code.\n\
-S  : Easy read of MSX-BASIC compressed space code.\n\
");
		exit(1);
	}
	for (i = 1; i < argc; i++) {
		if ((flags(argv[i])) != (unsignedchar)0 ) continue;
		readf = (unsignedchar) 0;
		fileopen(argv[i]);		/* 各種ファイルを開く */
		while (getword()) {
			dc_flag = (unsignedchar) 0;				/* ””フラグをクリア 		 */
			rem_flag = (unsignedchar) 0;				/* ＲＥＭフラグをクリア		 */
			intprt(getword(), (unsignedchar) 10);		/* 行番号を表示				 */
			putb(' ');

			while (!conv());		/* 解析部を呼び出す */
			putb((unsignedchar) 10);
		}
		fclose(fp);
	}
	exit(0);
}

unsignedchar		  *memalloc(size_t byte)				/* メモリ確保 */
{
	unsignedchar		  *buf;

	if (((buf = (unsignedchar *) malloc(byte))) == NULL) {
		err("Out of memory", "");
	}
	*buf = '\0';
	return (buf);
}

unsignedchar				mjchkc(unsignedchar c)
{
		switch(mjflag) {
				case		(unsignedchar)1: return toupper(c);
				case		(unsignedchar)2: return tolower(c);
		}
		return (c);
}

unsignedchar				*cp(unsignedchar *dst, unsignedchar *src) {
	char c;
	while((c = *src++) != '\0') {
		*dst++ = c;
	}
	*dst='\0';
	return dst;
}

unsignedchar				*mjchk(unsignedchar *s)
{
		unsignedchar		*ss;
		ss=mjbuf;
		cp(ss, s);
		do {
			*ss = mjchkc(*ss);
		} while(*ss++ != '\0');
		return (mjbuf);
}
		
void fileopen(unsignedchar * argv)
{
	buffer = save;

	if ((fp = fopen(argv, "rb")) == NULL) {
		err("File not found  :", argv);
	}
	if (getbyte() != (unsignedchar) 0xff) {
		err("Not MSX-BASIC File : ", argv);
	}
	out = 1;
	if (files != (unsignedchar)1) {
		put(str_files);
		put(str_lf);
		put(argv);
		put(str_lf);
		put(str_files);
		put(str_lf);
	}
}

unsignedchar flags(unsignedchar * argument)
{
	unsignedchar argchar;

	if ((*argument == '/' || *argument == '-')) {
		argchar=toupper(*(argument + 1));
		if (argchar == 'M') {
			switch(*(argument+2)) {
					case		'0':		return (unsignedchar)1;
					case		'1':		return (unsignedchar)2;
					case		'2':		return (unsignedchar)3;
					case		'\0':		return (unsignedchar)2;
			}
		}
		if (argchar == 'K') {
			outconv=1;
			return (unsignedchar)128;
		}
		if (argchar == 'S') {
			spc=1;
			return (unsignedchar)128;
		}
		if (argchar == 'H' || argchar == '?') {
			help=1;
			return (unsignedchar)128;
		}
		err("Option Error:", argument);
	}
	return (unsignedchar)0;
}

unsignedchar conv()
{
	unsignedchar		   state;		/* BASIC ステートメント		 */

	if ((state = getbyte()) == (unsignedchar) 0)
		return ((unsignedchar) 1);
	if ((rem_flag || dc_flag) != (unsignedchar) 0) {
		if (state == (unsignedchar) 0x22)
			dc_flag = (unsignedchar) 0;
		putb(state);
	} else
		switch (state) {
		case 0x0b: {				/* ８進数		*/
			put(mjchk("&O"));
			intprt(getword(), (unsignedchar) 8);
			insspc();
			break;
		}
		case 0x0c: {				/* 16進数		*/
			put(mjchk("&H"));
			intprt(getword(), (unsignedchar) 16);
			insspc();
			break;
		}
		case 0x0e:				/* 行番号/整数		*/
		case 0x1c: {
			intprt(getword(), (unsignedchar) 10);
			insspc();
			break;
		}
		case 0x0f: {				/* 10-255の整数		*/
			intprt(getbyte(), (unsignedchar) 10);
			insspc();
			break;
		}
		case 0x1d: {				/* 単精度実数		*/
			sngdbl((unsignedchar) 4);
			insspc();
			break;
		}
		case 0x1f: {				/* 倍精度実数		*/
			sngdbl((unsignedchar) 8);
			insspc();
			break;
		}
		case 0x3a: {				/* :				*/
			koron();
			break;
		}
		case 0xff: {				/* 関数				*/
			com_print(getbyte(), tbl_ff);
			insspc();
			break;
		}
		default:
			if (state < (unsignedchar) 0x1b) {
				putb((int) (state + (unsignedchar) 0x1f));		/* 0-9 の整数		*/
				insspc();
			} else if (state < (unsignedchar) 0x81) {
				putb(state);								/* 文字				*/
				if (state == (unsignedchar) 0x22)
					dc_flag = (unsignedchar) 1;
			} else {										/* 一般ステート		*/
				if (state == (unsignedchar) 0x84)
					rem_flag = (unsignedchar) 255;
				com_print(state, tbl);
				insspc();
			}
		}
	return ((unsignedchar) 0);
}

void sngdbl(unsignedchar len)
{
	unsignedchar		   buf, doubles[16];
	signedchar		   i;
	signedchar		   valpow;

	valpow = (signedchar) getbyte();		/* 指数部読み出し */

	for (i = (unsignedchar) 1; i < len; i++) {		/* 仮数部呼出 */
		doubles[i * 2 - 2] = (((buf = getbyte()) >> 4) &
							  (unsignedchar) 0x0f) + (unsignedchar) 0x30;
		doubles[i * 2 - 1] = (buf & (unsignedchar) 0x0f) + (unsignedchar) 0x30;
	}
	doubles[len * 2 - 2] = '\0';/* ０縮小 */
	for (i = len * (unsignedchar) 2 - (unsignedchar) 3; i >= (unsignedchar) 0; i--) {
		if (doubles[i] == '0')
			doubles[i] = '\0';
		else
			break;
	}

	valpow -= (signedchar) 0x40;		/* 指数部によって処理を振り分 */

	if (valpow > (signedchar) - 2 && valpow < (signedchar) 15) {

		if (valpow < (signedchar) 1) {
			putb('.');
			for (i = (signedchar) 0; i > valpow; i--)
				putb('0');

			i = (signedchar) 0;
			while (doubles[i++] != '\0')
				putb(doubles[i - 1]);

		} else {
			i = 0;

			while (doubles[i++] != '\0') {
				putb(doubles[i - 1]);
				if (--valpow == (signedchar) 0 && doubles[i] != '\0')
					putb('.');
			}
			while (valpow-- > (signedchar) 0)
				putb('0');
		}
	} else {
		i = 0;
		while (doubles[i++] != '\0') {
			putb(doubles[i - 1]);
			if (i == (signedchar) 1 && doubles[i] != '\0')
				putb('.');
		}
		putb(mjchkc('E'));
		if (--valpow < (signedchar) 0) {
			putb('-');
			valpow = -valpow;
		} else
			putb('+');

		putb(valpow / (signedchar) 10 + (signedchar) 0x30);
		putb(valpow % (signedchar) 10 + (signedchar) 0x30);
	}
}

void koron()
{
	switch (getbyte()) {
		case 0x8f:{
			rem_flag = (unsignedchar) 255;		/* REM */
			if (getbyte() == (unsignedchar) 0xe6) {
				com_print((unsignedchar) 0xe6, tbl);
				insspc();
			}else {
				buffer--;
				com_print((unsignedchar) 0x8f, tbl);
				insspc();
			} break;
		}
	case 0xa1:{
			com_print((unsignedchar) 0xa1, tbl);
			insspc();
			break;
		}
	default:{
			buffer--;
			putb(':');
		}
	}
}

void com_print(unsignedchar func, register unsignedchar * table)
{
	func -= (unsignedchar) 0x81;
	while (func) {
		while (*(table++) != '\0');
		func--;
		continue;
	}
	put(mjchk(table));
}

void err(unsignedchar * message, unsignedchar * filename)
{
	out = 2;
	put("msxlist>");
	put(message);
	put(filename);
	putb((unsignedchar) 10);
	exit(1);
}

unsignedchar intprt(unsignedint n, unsignedchar radix)
{
	unsignedchar		   len, r;

	len = (n >= radix) ? intprt(n / radix, radix) : 0;
	r = (unsignedchar) (n % radix);
	putb(mjchkc(
			(unsignedchar) ((r < (unsignedchar) 10) ? r + (unsignedchar) 0x30 : r + (unsignedchar) 0x37)));
	return (len + 1);
}

unsignedchar getbyte()
{
	if ((buffer >= save + RBUFSIZ) || !readf) {
		readf = (unsignedchar) 1;
		buffer = save;
		fread(buffer, RBUFSIZ, 1, fp);
	}
	return (*(buffer++));
}

unsignedint getword()
{
	unsignedint a, b;
	a=(unsignedint)getbyte();
	b=(unsignedint)getbyte();
	return a + (b * 256);
}

void putb(unsignedchar str)
{
/*
	if (str == (unsignedchar) 10)
		putb((unsignedchar) 13);
*/
	write2((unsignedchar *) & str, (unsignedint) 1);
}

void put(unsignedchar * str)
{
	write2(str, strlen(str));
}

void write2(unsignedchar * str, int len)
{
	int	i, kflag, j;
	unsignedchar c;
	int oflag;

	kflag=0;

	if(len == -1) {
		_write(out, str, len);
	} else {
		for(i = 0; i < len; i++) {
			oflag=0;
			c=*str++;
			if(outconv && kflag) {
				for(j=0; j<CONVCHARS; j++) {
					if(charconv[j][0] == c && c < (unsignedchar)0x60) {
						write3(charconv[j][1]);
						write3(charconv[j][2]);
						oflag=1;
					}
				}
				kflag=0;
			} else if(outconv) {
				if(c == (unsignedchar)1) {
					kflag=1;
					oflag=1;
				} else {
					for(j=0; j<CONVCHARS; j++) {
						if(charconv[j][0] == c && c > (unsignedchar)0x7f) {
							write3(charconv[j][1]);
							write3(charconv[j][2]);
							oflag=1;
						}
					}
				}
			}
			if(!oflag) {
				write3(c);
			}
		}
	}
}

void write3(unsignedchar c) {
	char str[2];

	str[0]=c;
	_write(out, str, 1);
}

void insspc(void) {
	if(spc) {
		write3(' ');
	}
}

/*
 * かなりスパゲティーソース閲覧ありがとうございます
 */
