/*
 *
 * Chinese Academy of Sciences
 * State Key Laboratory of Information Security
 * Institute of Information Engineering
 *
 * Copyright (C) 2016 Chinese Academy of Sciences
 *
 * LuoPeng, luopeng@iie.ac.cn
 * Updated in May 2016
 *
 */
#ifndef AES_128_H
#define AES_128_H

#define AES_BLOCK_SIZE      16

/**
 * @purpose:            Key schedule for AES-128
 * @par[in]key:         16 bytes of master keys
 */
void aes128_set_key(const unsigned char *key);

/**
 * @purpose:            Encryption. The length of plain and cipher should be one block (16 bytes).
 *                      The plaintext and ciphertext may point to the same memory
 * @par[in]plaintext:   plain text
 * @par[out]ciphertext: cipher text
 */
void aes128_encrypt(unsigned char *ciphertext, const unsigned char *plaintext);

/**
 * @purpose:            Decryption. The length of plain and cipher should be one block (16 bytes).
 *                      The ciphertext and plaintext may point to the same memory
 * @par[in]ciphertext:  cipher text
 * @par[out]plaintext:  plain text
 */
void aes128_decrypt(unsigned char *plaintext, const unsigned char *ciphertext);

#endif
