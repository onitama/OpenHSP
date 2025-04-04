/***********************************************************************
 *	PACKET ä«óù
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "packet.h"

static Packet *mem_ot;
static char *mem_pk;
static char *cur;
static char *pkstart;
static int pkIndex;
static int pkSize;
static int pkLimit;

static int pkOTRange;
static char *pkBuf[2];
static Packet *table;

static void PkClearTable( int count )
{
	int i;

	for ( i=0; i<count; i++ ) {
		table[i].adr = NULL;
		table[i].next = (void *)&table[i+1];
	}
	table[count - 1].next = NULL;

}


void *pkAllocBuffer( int size )
{
	char *p = cur;
	cur += size;
	return p;
}


void pkRestartBuffer( void )
{
	cur = pkBuf[pkIndex];
	table = (Packet *)pkAllocBuffer( sizeof( Packet ) * pkOTRange );
	PkClearTable( pkOTRange );
}


void pkBeginBuffer( void )
{
	pkRestartBuffer();
	pkIndex ^= 1;
}


void *pkGetBuffer( void )
{
	return cur;
}


void pkAddBuffer( int size )
{
	cur += size;
}


Packet *pkGetTable( void )
{
	return table;
}


void pkInsertPacket( int ot, void *ptr )
{
	Packet *p;
	Packet *n;
	void *next;

	n = &table[ot];
	next = n->next;
	n->next = cur;

	p = (Packet *)cur;
	cur += sizeof(Packet);
	p->adr = ptr;
	p->next = next;
}


void *pkOpen( void )
{
	pkstart = cur;
	return cur;
}


void pkClose( int ot )
{
	pkInsertPacket( ot, pkstart );
}


/*---------------------------------------------------------------------------*/

void pkInit( int size, int otrange )
{
	pkSize = size;
	pkLimit = size/2;
	pkIndex = 0;
	mem_pk = (char *)malloc( size );
	pkBuf[0] = mem_pk;
	pkBuf[1] = mem_pk + pkLimit;

	pkOTRange = otrange;
	mem_ot = (Packet *)malloc( sizeof(Packet)*otrange );
}

void pkTerm( void )
{
	free( mem_ot );
	free( mem_pk );
}

