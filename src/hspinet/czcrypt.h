//
//	CzCrypt.cpp header
//
#ifndef __czcrypt_h
#define __czcrypt_h

//	CzCrypt class
//
class CzCrypt {
public:
	//	Functions
	//
    CzCrypt();
    ~CzCrypt();

    // Load data from a file
    int DataLoad(const char* fname);

    // Initialize the seed values
    void InitializeSeed(int seed_1, int seed_2);

    // Decrypt the loaded data
    void DecryptData();

    // Get the size of the decrypted data
    unsigned int GetDataSize();

    // Get a pointer to the decrypted data
    unsigned char* GetDecryptedData();

	//	Data
	//
private:
    unsigned char* mData;   // Pointer to encrypted/decrypted data
    size_t mDataSize;          // Size of the data
    unsigned int mstate1, mstate2, mstate3; // PRNG state values
    unsigned char seedBuffer[128]; // Seed buffer for decryption

    // Helper function to generate a seed value
    double  GenerateSeedValue();

};

#endif // CZCRYPT_H
