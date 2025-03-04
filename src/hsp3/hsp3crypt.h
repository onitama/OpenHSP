//
//	HSP3 crypt header
//
#ifndef __hsp3crypt_h
#define __hsp3crypt_h

#define HSP3CRYPT_TABLESIZE 128

class HSP3Crypt {
public:
	HSP3Crypt();
	~HSP3Crypt();
	void Reset(void);

	void			SetBasePath(char* path);				// 対象ファイルのパスを設定する
	char *			GetBasePath(void);						// 対象ファイルのパスを取得する

	void			SetPassword(char *pass);				// 暗号/復号のためのパスワードを設定する
	int				DataSet( char *ptr, int size, int crypt=0 );		// 作業バッファを設定する
	void			SetOffset(int offset);					// バッファのオフセット指定
	unsigned char	Encrypt(unsigned char data);			// データを暗号化
	unsigned char	Decrypt(unsigned char data);			// データを復号化
	int				EncryptBuffer( void );					// バッファを暗号化
	int				DecryptBuffer( void );					// バッファを復号化
	int				GetSize( void );						// バッファサイズを取得
	char*			GetData( void );						// バッファへのポインタを取得
	int				GetSalt(int value);						// valueを元にしたsaltを得る

	int				GetCRC32( char *pbuffer, int psize );		// CRC32値を取得

private:
	//		private data
	//
	char*			basepath;								// dpm/exeファイルのフルパス
	char*			buffer;									// 操作対象データ
	int				size;									// 操作対象データサイズ
	int				cur;									// 現在のポインタ
	int				maincrypt;								// グローバルの暗号キー
	int				bufcrypt;								// バッファ固有の暗号キー
	int				lastcode;								// ワーク用キー


};


#endif
