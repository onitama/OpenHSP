// https://en.wikipedia.org/wiki/Wichmann%E2%80%93Hill
// https://www.vbforums.com/showthread.php?499661-Wichmann-Hill-Pseudo-Random-Number-Generator-an-alternative-for-VB-Rnd()-function

#include "czcrypt.h"
#include <stdio.h>
#include <stdlib.h>

#include <cassert>


CzCrypt::CzCrypt() {
    mData = nullptr;
	mDataSize = 0;
}

CzCrypt::~CzCrypt() {
    if (mData) {
        free(mData);
    }
}

// Load data from a file
int CzCrypt::DataLoad(const char* fname) {
    FILE *fp = fopen(fname, "rb");
    if (!fp) {
		// Improvement
		// Outputs an error message to stderr based on the current errno value (e.g., file open errors).
		// Example: "Failed to open file: No such file or directory"
		perror("Failed to open file");
		return 1;
	}

    //Get file size
	fseek(fp, 0, SEEK_END);
    mDataSize = ftell(fp);
    rewind(fp);

    if (mData) {
        free(mData);
        mData = nullptr;
    }

    mData = (unsigned char*) malloc(mDataSize);
    if (!mData) {
        fclose(fp);
		// Improvement
		fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
	
	// improvement to original: Handle read error
	int i = 0;
	while (i < mDataSize) {
		i += 
		size_t bytesRead = fread( mData + i, 1, mDataSize - i, fp)
		if (bytesRead == 0) break; // Handle read error or EOF
		i += bytesRead;
	}
	
    fclose(fp);

    return 0;
}
// Generate the next seed value
// linear congruential generator (LCG)
double CzCrypt::GenerateSeedValue() {

	// Seed values (are all prime numbers)
	// https://de.wikibooks.org/wiki/Primzahlen:_Tabelle_der_Primzahlen_(2_-_100.000)
	// ...
	// 30269, 30271, 30293, 
	// 30307, 30313, 30319, 
	// 30323, 30341, 30347, ... 

	// standard Wichmann-Hill PRNG implementation 
    mstate1 = ( mstate1 * 171 ) % 30269;
    mstate2 = ( mstate2 * 172 ) % 30307;
	mstate3 = ( mstate3 * 170 ) % 30323;

    double r = 
		mstate1 / 30269 + 
		mstate2 / 30307 + 
		mstate3 / 30323;
		
return (r % 1);
}


// Initialize the seed values
void CzCrypt::InitializeSeed(int seed_1, int seed_2) {

	#define MASK_TO_15BITS(value) ((value) & 0x7FFF)

	// Macro to convert a scaled double to a single byte
	#define DOUBLE_TO_BYTE(value) ((unsigned char)((value) * 32768.0) & 0xFF)

	// Initialize mstate based on seed_1 ( for ex. hsp_sum )
	mstate1 = MASK_TO_15BITS( seed_1			); // Equivalent to seed_1 & 0x7FFF
	mstate2 = MASK_TO_15BITS( mstate1 * mstate1	); // = seed_1²
	mstate3 = MASK_TO_15BITS( mstate1 * mstate2	); // = seed_1³

    mstate1 =  seed_1 				& 0x7FFF; // = seed_1
    mstate2 = (mstate1 * mstate1)	& 0x7FFF; // = seed_1²
    mstate3 = (mstate1 * mstate2)	& 0x7FFF; // = seed_1³

    // Generate the first 64 seedBuffer values
    for ( int i = 0; i < 64; ++i ) {
        seedBuffer[ i ] = DOUBLE_TO_BYTE( GenerateSeedValue() );
    }


    // Initialize mstate based on seed_2 ( for ex. hsp_dec )
    mstate1 =  seed_2 				& 0x7FFF; // = seed_2
    mstate2 = (mstate1 * mstate1)	& 0x7FFF; // = seed_2²
    mstate3 = (mstate1 * mstate2)	& 0x7FFF; // = seed_2³

    // Generate the remaining 64 seedBuffer values
    for (     j = 64; j < 128; ++j ) {
        seedBuffer[ j ] = DOUBLE_TO_BYTE( GenerateSeedValue() );
    }
}

// Decrypt the data using the seed values
void CzCrypt::DecryptData() {
	//Improvement:
    assert(mData != nullptr && "Data must be loaded before decryption");
    assert(mDataSize > 0 && "Data size must be positive");
	
	
    int dataIndex =  mDataSize         - 1;
    int seedIndex = (mDataSize & 0x7F) - 1;

    // backwards loop though mData
	while (dataIndex >= 0) {
        
		// Keep seedIndex in range
		seedIndex &= 0x7F;   //if (seedIndex < 0) seedIndex = 127;
		
		// XOR data with seed and forward (?!- Yes since index runs backwards.) seedIndex
        mData[dataIndex] ^= seedBuffer[ seedIndex-- ];
		
        if (dataIndex >= 1) //expect for the last byte
		// ADD next (still encrypted) byte to current and forward dataIndex  
		mData[dataIndex--] += mData[ dataIndex - 1];
    }
}

// Get the size of the decrypted data
size_t CzCrypt::GetDataSize() {
    return mDataSize;
}

// Get the pointer to the decrypted data
unsigned char* CzCrypt::GetDecryptedData() {
    return mData;
}
