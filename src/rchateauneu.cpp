#include "itoa_ljust.h"
#include "test.h"

////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
Copyright 2007 Remi Chateauneu.
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Except the routine:
char * uint_to_ascii_nlz( char *sptr, unsigned int x )
... which comes from the AMD web site:
http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/25112.PDF
... and whose status is not clear.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

itoa() is extremely useful for me. It is not in the C++ standard.
There are several problems:
- It is stuck with radix 10: Used 99% of the time.
- Possible buffer overflow: Just allocate 12 - twelve - chars.
- It can be emulated with std::stringstream.str() : Hundred times slower.
- It needs 40kbytes of static buffer: Cost=0.001 pence of RAM, or 0.00001 of HD.

All this library assumes:
- There is only one way to display numbers. No locale, base 10 only.
- Buffers are big enough.

******************************************************************************/

#include <stdlib.h>
#include <memory.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>

/* Size of the table of precomputed values. */
#define ATOI_TAB_SZ 10000UL

/* Number of digits of precomputed values. */
#define ATOI_TAB_SZ_LOG 4UL

/*
This coding is endian-neutral. This table is used for converting from
a short int, an int or a long long.
Preprocessor generation thanks to:
http://stackoverflow.com/questions/319328/writing-a-while-loop-in-the-c-preprocessor
The result looks like:
char cstItoa[40001] = "0000" "0001" "0002" ... "9999"
*/
static const char cstItoa[ATOI_TAB_SZ * ATOI_TAB_SZ_LOG + 1] =

#define n0(p,x)         //x(p##0)
#define n1(p,x) n0(p,x) x(p##1)
#define n2(p,x) n1(p,x) x(p##2)
#define n3(p,x) n2(p,x) x(p##3)
#define n4(p,x) n3(p,x) x(p##4)
#define n5(p,x) n4(p,x) x(p##5)
#define n6(p,x) n5(p,x) x(p##6)
#define n7(p,x) n6(p,x) x(p##7)
#define n8(p,x) n7(p,x) x(p##8)
#define n9(p,x) n8(p,x) x(p##9)

#define n00(f,p,a,x)                       n##a(p##0,x)
#define n10(f,p,a,x) n00(f,p,9,x) x(p##10) n##a(p##1,x)
#define n20(f,p,a,x) n10(f,p,9,x) x(p##20) n##a(p##2,x)
#define n30(f,p,a,x) n20(f,p,9,x) x(p##30) n##a(p##3,x)
#define n40(f,p,a,x) n30(f,p,9,x) x(p##40) n##a(p##4,x)
#define n50(f,p,a,x) n40(f,p,9,x) x(p##50) n##a(p##5,x)
#define n60(f,p,a,x) n50(f,p,9,x) x(p##60) n##a(p##6,x)
#define n70(f,p,a,x) n60(f,p,9,x) x(p##70) n##a(p##7,x)
#define n80(f,p,a,x) n70(f,p,9,x) x(p##80) n##a(p##8,x)
#define n90(f,p,a,x) n80(f,p,9,x) x(p##90) n##a(p##9,x)

#define n000(f,p,a,b,x)                           n##a##0(1,p##0,b,x)
#define n100(f,p,a,b,x) n000(f,p,9,9,x) x(p##100) n##a##0(1,p##1,b,x)
#define n200(f,p,a,b,x) n100(f,p,9,9,x) x(p##200) n##a##0(1,p##2,b,x)
#define n300(f,p,a,b,x) n200(f,p,9,9,x) x(p##300) n##a##0(1,p##3,b,x)
#define n400(f,p,a,b,x) n300(f,p,9,9,x) x(p##400) n##a##0(1,p##4,b,x)
#define n500(f,p,a,b,x) n400(f,p,9,9,x) x(p##500) n##a##0(1,p##5,b,x)
#define n600(f,p,a,b,x) n500(f,p,9,9,x) x(p##600) n##a##0(1,p##6,b,x)
#define n700(f,p,a,b,x) n600(f,p,9,9,x) x(p##700) n##a##0(1,p##7,b,x)
#define n800(f,p,a,b,x) n700(f,p,9,9,x) x(p##800) n##a##0(1,p##8,b,x)
#define n900(f,p,a,b,x) n800(f,p,9,9,x) x(p##900) n##a##0(1,p##9,b,x)

#define n0000(f,p,a,b,c,x)                               n##a##00(f,p##0,b,c,x)
#define n1000(f,p,a,b,c,x) n0000(f,p,9,9,9,x) x(p##1000) n##a##00(1,p##1,b,c,x)
#define n2000(f,p,a,b,c,x) n1000(f,p,9,9,9,x) x(p##2000) n##a##00(1,p##2,b,c,x)
#define n3000(f,p,a,b,c,x) n2000(f,p,9,9,9,x) x(p##3000) n##a##00(1,p##3,b,c,x)
#define n4000(f,p,a,b,c,x) n3000(f,p,9,9,9,x) x(p##4000) n##a##00(1,p##4,b,c,x)
#define n5000(f,p,a,b,c,x) n4000(f,p,9,9,9,x) x(p##5000) n##a##00(1,p##5,b,c,x)
#define n6000(f,p,a,b,c,x) n5000(f,p,9,9,9,x) x(p##6000) n##a##00(1,p##6,b,c,x)
#define n7000(f,p,a,b,c,x) n6000(f,p,9,9,9,x) x(p##7000) n##a##00(1,p##7,b,c,x)
#define n8000(f,p,a,b,c,x) n7000(f,p,9,9,9,x) x(p##8000) n##a##00(1,p##8,b,c,x)
#define n9000(f,p,a,b,c,x) n8000(f,p,9,9,9,x) x(p##9000) n##a##00(1,p##9,b,c,x)

#define cycle4(c1,c2,c3,c4,x) n##c1##000(0,,c2,c3,c4,x)
#define cycle3(c1,c2,c3,x) n##c1##00(0,,c2,c3,x)
#define cycle2(c1,c2,x) n##c1##0(0,,c2,x)
#define cycle1(c1,x) n##c1(,x)

#define STRINGIZE(S) #S

"0000" cycle4(9, 9, 9, 9, STRINGIZE)

;

/* Number of digits of an unsigned int (Except 0).
Inspired from https://graphics.stanford.edu/~seander/bithacks.html
*/
unsigned int ilog_10(unsigned int i)
{
	return
		(i >= 1000000000)
		? 10
		: (i >= 100000000)
		? 9
		: (i >= 10000000)
		? 8
		: (i >= 1000000)
		? 7
		: (i >= 100000)
		? 6
		: (i >= 10000)
		? 5
		: (i >= 1000)
		? 4
		: (i >= 100)
		? 3
		: (i >= 10)
		? 2
		: i
		? 1
		: 0;
};

#define LONG_DIGITS 10

/* Number of digits of an unsigned long long (Except 0).
Inspired from https://graphics.stanford.edu/~seander/bithacks.html
*/
unsigned int lllog_10(unsigned long long i)
{
	// Better balance. Small values come first.
	if (i < 10000000)     goto lab07;
	if (i < 100000000000) goto lab11;

	if (i >= 10000000000000000000U) return 20;
	if (i >= 1000000000000000000U) return 19;
	if (i >= 100000000000000000U) return 18;
	if (i >= 10000000000000000U) return 17;
	if (i >= 1000000000000000U) return 16;
	if (i >= 100000000000000U) return 15;
	if (i >= 10000000000000U) return 14;
	if (i >= 1000000000000U) return 13;
	if (i >= 100000000000U) return 12;
lab11:	if (i >= 10000000000U) return 11;
	if (i >= 1000000000U) return 10;
	if (i >= 100000000U) return 9;
	if (i >= 10000000U) return 8;
lab07:	if (i >= 1000000U) return 7;
	if (i >= 100000U) return 6;
	if (i >= 10000U) return 5;
	if (i >= 1000U) return 4;
	if (i >= 100U) return 3;
	if (i >= 10U) return 2;
	if (i >= 1U) return 1;
	return 0;
};

#define LLONG_DIGITS 20

/* Transforms an unsigned short into a string.
* Returns a pointer just after the last digit.
* It would be faster with a specific table of 65536.
* */
char * ustoa(char *aBuf, unsigned short aShort)
{
	unsigned short tmpShort = (aShort > 9999) ? (aShort / 10) : aShort;

	const char * src = cstItoa + ATOI_TAB_SZ_LOG * tmpShort;
	if (tmpShort >= 1000) *aBuf++ = src[0];
	if (tmpShort >= 100) *aBuf++ = src[1];
	if (tmpShort >= 10) *aBuf++ = src[2];
	*aBuf++ = src[3];

	if (tmpShort != aShort)
	{
		char tmpRem = aShort % 10;
		*aBuf++ = tmpRem + '0';
	}
	*aBuf = '\0';
	return aBuf;
};

/* Returns a pointer just after the last digit. */
char * stoa(char *aBuf, short aShort)
{
	if (aShort < 0) {
		*aBuf++ = '-';
		aShort = -aShort;
	};
	return ustoa(aBuf, aShort);
};

/* Used by 'unsigned int' and 'unsigned long long'. */
static void uitoa_core(char * targetPtr, unsigned int anInt)
{
	while (anInt >= ATOI_TAB_SZ)
	{
		unsigned int myDiv10000 = anInt / ATOI_TAB_SZ;
		targetPtr -= 4;
		unsigned int myModu10000 = anInt - myDiv10000 * ATOI_TAB_SZ;
		memcpy(targetPtr, cstItoa + ATOI_TAB_SZ_LOG * (myModu10000), 4);
		anInt = myDiv10000;
	}

	const char * myTabPtr = cstItoa + (ATOI_TAB_SZ_LOG - 1) + ATOI_TAB_SZ_LOG * anInt;
	if (anInt >= 1000)
		*(--targetPtr) = *myTabPtr--;
	if (anInt >= 100)
		*(--targetPtr) = *myTabPtr--;
	if (anInt >= 10)
		*(--targetPtr) = *myTabPtr--;
	*(--targetPtr) = *myTabPtr--;
} /* uitoa_core */

/* The numbers are processed by groups of four digits. */
static char * uitoa_general(char *aBuf, unsigned int anInt)
{
	/* Points after the last digit of the number. */
	aBuf += ilog_10(anInt);

	uitoa_core(aBuf, anInt);

	*aBuf = '\0';
	return aBuf;
} /* uitoa_general */

char * uitoa(char *aBuf, unsigned int anInt)
{
	if (anInt < 10000)
	{
		const char * src = cstItoa + ATOI_TAB_SZ_LOG * anInt;
		if (anInt >= 1000) *aBuf++ = src[0];
		if (anInt >= 100) *aBuf++ = src[1];
		if (anInt >= 10) *aBuf++ = src[2];
		*aBuf++ = src[3];
		*aBuf = '\0';
		return aBuf;
	}
	return uitoa_general(aBuf, anInt);
};


char * good_old_itoa(char *aBuf, int anInt)
{
	if (anInt < 0) {
		*aBuf++ = '-';
		anInt = -anInt;
	};
	return uitoa(aBuf, anInt);
};


/* The numbers are processed by groups of four digits. */
static char * ulltoa_general(char *aBuf, unsigned long long aLL)
{
	/* Points after the last digit of the number. */
	aBuf += lllog_10(aLL);
	char * myPtr = aBuf;

	/* First div64 bits, then later 32 bits only. */
	while (aLL >= ULONG_MAX)
	{
		/* See div64_32 in Linux internals, for fast division. */
		unsigned long long myDiv10000 = aLL / ATOI_TAB_SZ;
		myPtr -= 4;
		unsigned int myModu10000 = unsigned int(aLL - myDiv10000 * ATOI_TAB_SZ);
		memcpy(myPtr, cstItoa + ATOI_TAB_SZ_LOG * myModu10000, 4);
		aLL = myDiv10000;
	};

	/* 32 bits only. */
	uitoa_core(myPtr, (unsigned int)aLL);

	*aBuf = '\0';
	return aBuf;
} /* ulltoa_general */

char * ulltoa(char *aBuf, unsigned long long aLL)
{
	const char * src = cstItoa + ATOI_TAB_SZ_LOG * aLL;
	if (aLL < 10000)
	{
		unsigned int anInt = (unsigned int)aLL;
		if (anInt >= 1000) *aBuf++ = src[0];
		if (anInt >= 100) *aBuf++ = src[1];
		if (anInt >= 10) *aBuf++ = src[2];
		*aBuf++ = src[3];
		*aBuf = '\0';
		return aBuf;
	};
	return ulltoa_general(aBuf, aLL);
};



/* A usual for this family of functions, there is no buffer overrun control.
* Any problem can be easily avoiedd because the buffer size does not be bigger
* than about twenty-one chars, plus neg sign. */
char * lltoa(char *aBuf, long long aLL)
{
	if (aLL < 0) {
		*aBuf++ = '-';
		aLL = -aLL;
	};
	return ulltoa(aBuf, aLL);
};





////////////////////////////////////////////////////////////////////////////////

void u32toa_rchateauneu(uint32_t v, char* p) { uitoa(p, v); }
void i32toa_rchateauneu(int32_t v, char* p) { good_old_itoa( p,v); }
void u64toa_rchateauneu(uint64_t v, char* p) { ulltoa(p, v); }
void i64toa_rchateauneu(int64_t v, char* p) { lltoa(p, v); }

REGISTER_TEST(rchateauneu);
