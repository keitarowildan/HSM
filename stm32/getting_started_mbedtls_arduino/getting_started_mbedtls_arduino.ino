/* @file getting_started_mbedtls_arduino.ino
 * by Johannes Kinzig -- https://johanneskinzig.de
 *  
 *  Getting satrted with mbedtls on ARM based Arduinos:
 *  - AES256 encryption/decryption
 *  - HMAC-SHA256
 *  
 *  https://johanneskinzig.de/index.php/systems-engineering/14-getting-started-with-mbedtls-on-arm-based-arduinos
 */


#include "src/lib_mbedtls/md.h"
#include "src/lib_mbedtls/aes.h"

unsigned char         key[32];
unsigned char     iv_orig[16];// iv is changed during encryption, so we need to keep a copy of the original
unsigned char          iv[16];
unsigned char       plain[128];
unsigned char plain_again[128];
unsigned char      cipher[128];

int i;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  Serial.println("Here we go....mbedtls on Arduino Primo....");
}

void loop() {

  Serial.write("----------------------------AES Encryption/Decryption--------------------\n");
  // prepare data, generate according to AES algorithm
  
 // Generate key -- bad way for production, just for testing
    Serial.write("Generate key, 256 Bit, 32 Byte\n");
    
    for(i=0; i<sizeof(key); i=i+1) {
        key[i]=(char)(90 - random(25));
        Serial.write((char)key[i]);
    }
    Serial.write("\nsize: ");
    Serial.print(sizeof(key), DEC);
    Serial.write("\n\n");
    
 // Generate IV
    Serial.write("Generate IV, 128 Bit, 16 Byte\n");
    for(i=0; i<sizeof(iv); i=i+1) {
        iv_orig[i]=key[i]=(char)(122 - random(25));
        iv[i]=iv_orig[i];
        Serial.write((char)iv_orig[i]);
    }
    Serial.write("\nsize: ");
    Serial.print(sizeof(iv), DEC);
    Serial.write("\n\n");
    
 // Generate data
    Serial.write("Random data, 1024 Bit, 128 Byte - (ascii)\n");
    
    for(i=0; i<sizeof(plain); i=i+1) {
        plain[i]=(char)(122 - random(74));
        Serial.write((char)plain[i]);
    }
    Serial.write("\nsize: ");
    Serial.print(sizeof(plain), DEC);
    Serial.write("\n\n");
    
    // begin encryption
    mbedtls_aes_context aes_encrypt, aes_decrypt;
    // size of key must be given as 128, 192 or 256 bit
    mbedtls_aes_setkey_enc( &aes_encrypt, key, sizeof(key)*8 );
    // size of plaintext is given in bytes
    mbedtls_aes_crypt_cbc(&aes_encrypt, MBEDTLS_AES_ENCRYPT, sizeof(plain), iv, plain, cipher);
    
    Serial.write("Ciphertext (hex)\n");
    for (i=0; i<sizeof(cipher);i=i+1) {
        Serial.print((char)cipher[i], HEX);
    }
    Serial.write("\nsize: ");
    Serial.print(sizeof(cipher), DEC);
    Serial.write("\n\n");
    
    // begin decryption - size of key must be given as 128, 192 or 256 bit
    mbedtls_aes_setkey_dec(&aes_decrypt, key, sizeof(key)*8);
    // size of ciphertext is given in bytes
    mbedtls_aes_crypt_cbc(&aes_decrypt, MBEDTLS_AES_DECRYPT, sizeof(cipher), iv_orig, cipher, plain_again);
    
    Serial.write("Plaintext (again)\n");
    for (i=0; i<sizeof(plain_again);i=i+1) {
        Serial.write((char)plain_again[i]);
    }
    Serial.write("\nsize: ");
    Serial.print(sizeof(plain_again), DEC);
    Serial.write("\n\n");

    Serial.write("------------------------------------------------------------\n");


    Serial.write("-------------------------HMAC sha256------------------------\n");

    int             hmac_1_sha256_success;
    int             hmac_2_sha256_success;
    unsigned char   hmac_sha256_output1[32] = "";   // 256/8 = 32
    unsigned char   hmac_sha256_output2[32] = "";   // 256/8 = 32
    unsigned char   data_block1[64] = "";
    memcpy(data_block1, "WOHVMMXRXDMMVDMRWXNOCYMVBEGBYCILWOHVMMXRXDMMVDMRWXNOCYMVBEGBYCI5", sizeof(data_block1));  // 512/8 = 64
    unsigned char   data_block2[64] = "";
    memcpy(data_block2, "WOHVMMXRXDMMVDMRWXNOCYMVBEGBYCILWOHVMMXRXDMMVDMRWXNOCYMVBEGBYCI5", sizeof(data_block2));  // 512/8 = 64
    unsigned char   signing_key[32] = "";
    memcpy(signing_key, "amdn395h192mfk47flak2932gh46271j", sizeof(signing_key)); // 256/8 = 32
    // prepare hmac context
    const mbedtls_md_info_t *md_info_sha256_1, *md_info_sha256_2;
    md_info_sha256_1 = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    md_info_sha256_2 = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

    Serial.print("Data-Block-1 - (ascii)\n");
    for(i=0;i<sizeof(data_block1);i=i+1){
        Serial.print((char)data_block1[i]);
    }
    Serial.print("\n");
    
    Serial.print("Data-Block-2 - (ascii)\n");
    for(i=0;i<sizeof(data_block2);i=i+1){
        Serial.print((char)data_block2[i]);
    }
    Serial.print("\n");
    
    // sign both messages
    hmac_1_sha256_success = mbedtls_md_hmac(md_info_sha256_1, signing_key, sizeof(signing_key), data_block1, sizeof(data_block1), hmac_sha256_output1);
    hmac_2_sha256_success = mbedtls_md_hmac(md_info_sha256_2, signing_key, sizeof(signing_key), data_block2, sizeof(data_block2), hmac_sha256_output2);

    Serial.print("Computing signatures:\n");
    Serial.print("Status 1: ");
    Serial.print((char)hmac_1_sha256_success, DEC);
    Serial.print("\n");
    Serial.print("Status 2: ");
    Serial.print(hmac_2_sha256_success, DEC);
    Serial.print("\n");
    
    Serial.print("Signature-1 (hex):\n");
    for(i=0;i<sizeof(hmac_sha256_output1);i=i+1){
        Serial.print((char)hmac_sha256_output1[i], HEX); //printf("%x", hmac_sha256_output1[i]);
    }
    Serial.print("\n");
    
    Serial.print("Signature-2 (hex):\n");
    for(i=0;i<sizeof(hmac_sha256_output2);i=i+1){
        Serial.print((char)hmac_sha256_output2[i], HEX); //printf("%x", hmac_sha256_output2[i]);
    }
    Serial.print("\n");

    
    // compare both signatures - http://pubs.opengroup.org/onlinepubs/009695399/functions/memcmp.html
    if(memcmp(hmac_sha256_output1, hmac_sha256_output2, sizeof(hmac_sha256_output1)) == 0) {
        Serial.print("Signatures are identical!\n");
    }
    else {
        Serial.print("Signatures are NOT identical!\n");
    }

    Serial.print("---------------------------------------------------------\n");
    
}
