#ifndef		__PACKET_H__
#define		__PACKET_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

// パケット構造体

typedef struct Packet {
	void *adr;			// 実体のアドレス
	void *next;			// 次のパケットアドレス
} Packet;

void pkInit( int size, int otrange );
void pkTerm( void );
void *pkAllocBuffer( int size );
void pkBeginBuffer( void );
void pkRestartBuffer( void );
void *pkGetBuffer( void );
void pkAddBuffer( int size );
void pkInsertPacket( int ot, void *ptr );
void *pkOpen( void );
void pkClose( int ot );
Packet *pkGetTable( void );

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif
/*---------------------------------------------------------------------------*/
#endif
