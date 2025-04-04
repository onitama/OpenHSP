//
//	cz crypt header
//
#ifndef __czcrypt_h
#define __czcrypt_h

typedef struct {
	short	year;		// 年
	char	month;		// 月
	char	day;		// 日

	char	hour;		// 時
	char	min;		// 分
	char	sec;		// 秒
	char	msec;		// ミリ秒

	char	rnd[8];		// 乱数値
} OURGUID;


class CzCrypt {
public:
	CzCrypt();
	~CzCrypt();

	int				DataSet( char *ptr, int size );			// 作業バッファを設定する
	int				DataLoad( char *file );					// ファイルの内容をロードしてバッファとする
	int				DataSave( char *file );					// バッファの内容をファイルに書き出す
	int				Encrypt( void );						// バッファを暗号化
	int				Decrypt( void );						// バッファを復号化
	int				GetSize( void );						// バッファサイズを取得
	char*			GetData( void );						// バッファへのポインタを取得
	void			SetGUID( OURGUID *guid );				// 暗号/復号のためのコードをGUIDから設定する
	void			SetSeed( int seed1, int seed2 );		// 暗号/復号のためのコードをSEEDから設定する
	void			MakeGUID( OURGUID *guid );				// GUIDを新規に作成する

	int				GetCRC32( void );						// CRC32値を取得
	void			GetMD5( char *res );					// MD5値を取得
	void			GetMD5ext( char *res, char *buf, int size );	// MD5値を取得
	void			GetSHA256(char* res);						// SHA256値を取得
	void			GetSHA256ext(char* res, char* buf, int size);	// SHA256値を取得

	int				EncodeBASE64( char *dstptr, char *srcptr, int size );
	int				DecodeBASE64( char *dstptr, char *srcptr, int size );
	int				GetBASE64Size( int size );
	int				EncodeRC4( char *ptr, char *key, int size );
	int				DecodeRC4( char *ptr, char *key, int size );


private:
	//		private data
	//
	char*			buffer;									// 操作対象データ
	int				size;									// 操作対象データサイズ

	char			randtable[128];							// 暗号/復元用乱数テーブル


	unsigned int	r_s1,r_s2,r_s3;
	double			rnd_d1,rnd_d2,rnd_d3;

	//		private function
	//
	void			initrnd( int seed );
	double			makernd( void );
	int				makerndi( void );
};


#endif
