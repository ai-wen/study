#ifndef __DES_H__
#define __DES_H__

	#define DES_ENCRYPT     1
	#define DES_DECRYPT     0

	#define DES_KEY_SIZE    8

	/**
	 * \brief          DES context structure
	 */
	typedef struct
	{
		int mode;                   /*!<  encrypt/decrypt   */
		unsigned long sk[32];       /*!<  DES subkeys       */
	}
	des_context;

	/**
	 * \brief          Triple-DES context structure
	 */
	typedef struct
	{
		int mode;                   /*!<  encrypt/decrypt   */
		unsigned long sk[96];       /*!<  3DES subkeys      */
	}
	des3_context;

	extern int des_setkey_enc( des_context *ctx, const unsigned char key[DES_KEY_SIZE] );

	extern int des_setkey_dec( des_context *ctx, const unsigned char key[DES_KEY_SIZE] );

	extern int des3_set3key_enc( des3_context *ctx, const unsigned char key[DES_KEY_SIZE * 3] );

	extern int des3_set3key_dec( des3_context *ctx, const unsigned char key[DES_KEY_SIZE * 3] );

	extern int des_crypt_ecb( des_context *ctx,
		int mode,
		size_t length,
		unsigned char *input,
		unsigned char *output );

	extern int des_crypt_cbc( des_context *ctx,
		int mode,
		size_t length,
		unsigned char iv[8],
		unsigned char *input,
		unsigned char *output );

	extern int des3_crypt_ecb( des3_context *ctx,
		int mode,
		size_t length,
		unsigned char *input,
		unsigned char *output );

	extern int des3_crypt_cbc( des3_context *ctx,
		int mode,
		size_t length,
		unsigned char iv[8],
		unsigned char *input,
		unsigned char *output );

#endif