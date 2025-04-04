
//
//
//		MXファイル構造
//
//			[NXHED]
//			[MDINF] x SIZE_MDINF
//			[NDINF] x SIZE_NDINF
//			[MATINF] x SIZE_MATINF
//
//

typedef struct {
  float x, y, z, w;
} MX_VECTOR;

// 整数AXIS型
typedef struct {
float u,v;
} MX_UVAXIS;

typedef struct
{
	MX_VECTOR v[4];
	MX_VECTOR n[4];
	MX_UVAXIS uv[4];
	int color;
	short tex;
	short mat;
} NDINF;

typedef struct
{
	MX_VECTOR pos;
	MX_VECTOR ang;
	MX_VECTOR scale;
	short	flag;			// enable flag ( master model )
	short	num_ndinf;		// num of ndinf
	int		ndinf;			// ndinf index
	short	child;			// child index
	short	sibling;		// sibling index
} MDINF;

typedef struct
{
	char name[16];
} MATINF;


typedef struct
{
	//	Extra Material Info
	//
	short	flag;			// Mode Flags
	char	shade;			// Shade Mode (0=no light/1=flat/2=gouraud)
	char	priority;		// Transparent priority
	int		opt1;			// (option)
	int		opt2;			// (option)
	float	alpha;			// 0.0～1.0 透明 / 1.1～ 加算 / -0.1～ 減算

	float	diffuse;		// diffuse rate値
	float	fog;			// fog rate値
	float	specular;		// スペキュラーrate値
	float	envmap;			// 環境マップrate値

	float	pad[8];			// (reserved)
} EMTINF;

typedef struct
{
	//		MX file header structure
	//
	char	h1;					// magic code1
	char	h2;					// magic code2
	char	h3;					// magic code3
	char	h4;					// magic code4
	int		version;			// version number info

	int		size_mdinf;			// size of Model section
	int		pt_mdinf;			// ptr to Model Segment
	int		size_ndinf;			// size of Node section
	int		pt_ndinf;			// ptr to Node Segment
	int		size_matinf;		// size of Matinf section
	int		pt_matinf;			// ptr to Matinf Segment
} MXHED;

#define MX_FLAG_GLOBALTEX 1		// グローバルのTextureIDを使用する
#define MX_FLAG_UV2	2			// UV値を0.0～1.0で格納する
#define MX_FLAG_PCOLOR 4		// 法線の替わりにRGB値を格納する
#define MX_FLAG_WEIGHT 8		// ndinfのvector.wにボーンウェイト情報を格納する
#define MX_FLAG_BONE 16			// ndinfのnormal.wにブレンド先ボーン情報を格納する
#define MX_FLAG_BOUND 32		// mdinfのscale.wにバウンディング半径情報を格納する


typedef struct
{
	//		MX file header structure (ver2)
	//
	char	h1;					// magic code1
	char	h2;					// magic code2
	char	h3;					// magic code3
	char	h4;					// magic code4
	int		version;			// version number info

	int		size_mdinf;			// size of Model section
	int		pt_mdinf;			// ptr to Model Segment
	int		size_ndinf;			// size of Node section
	int		pt_ndinf;			// ptr to Node Segment
	int		size_matinf;		// size of Matinf section
	int		pt_matinf;			// ptr to Matinf Segment

	//		Additional information
	int		flags;				// Extra Mode Flags
	short	texoffset;			// global Texture ID offset
	short	matoffset;			// global Material ID offset
	
	int		size_emtinf;		// size of ExMatinf section
	int		pt_emtinf;			// ptr to ExMatinf Segment
} MXHED2;

