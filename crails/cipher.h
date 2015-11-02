// ================================================================
// $Id: cipher.h,v 1.1 2012/04/10 01:37:54 jlinoff Exp jlinoff $
//
// Description: Cipher class.
// Copyright:   Copyright (c) 2012 by Joe Linoff
// Version:     1.1
// Author:      Joe Linoff
//
// LICENSE
//   The cipher package is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of the
//   License, or (at your option) any later version.
//       
//   The cipher package is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   General Public License for more details. You should have received
//   a copy of the GNU General Public License along with the change
//   tool; if not, write to the Free Software Foundation, Inc., 59
//   Temple Place, Suite 330, Boston, MA 02111-1307 USA.
// ================================================================
#ifndef cipher_h
#define cipher_h

#include <string>
#include <vector>
#include <utility> // pair

#define CIPHER_DEFAULT_CIPHER "aes-256-cbc"
#define CIPHER_DEFAULT_DIGEST "sha1"
#define CIPHER_DEFAULT_COUNT  1

/**
 * The cipher object encrypts plaintext data or decrypts ciphertext
 * data. All data is in ASCII because it is MIME encoded.
 *
 * The default cipher used is AES-256-CBC from the openssl library
 * but there are many others available. The default digest used is
 * SHA1 which is stronger than MD5.
 *
 * The algorithms mimic openssl so files created with this object
 * and with the openssl tool are interchangeable.
 *
 * Here is how you would use it to encrypt and decrypt plaintext
 * data in memory.
 * @code
 *   #include "cipher.h"
 *   #include <string>
 *   using namepsace std;
 *
 *   // Example the encrypts and decrypts some plaintext.
 *   // Use encrypt_file or decrypt_file to deal with files.
 *   void cipher_test()
 *   {
 *      string pass = "testTEST!23_";
 *      string salt = "12345678"; // must be 8 chars
 *      string plaintext = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
 *
 *      // Encrypt and decrypt.
 *      Cipher c;
 *      string ciphertext = c.encrypt(plaintext ,pass,salt);
 *      string decoded    = c.decrypt(ciphertext,pass,salt);
 *
 *      // Test the results
 *      if (plaintext == decoded) {
 *         cout << "passed" << endl;
 *      }
 *      else {
 *         cout << "failed" << endl;
 *      }
 *   }
 * @endcode
 * @author Joe Linoff
 */
class Cipher
{
public:
  typedef unsigned int uint;
  typedef unsigned char uchar;
  typedef uchar aes_key_t[32];
  typedef uchar aes_iv_t[32];
  typedef uchar aes_salt_t[8];
  typedef std::pair<uchar*,uint> kv1_t;
public:
  /**
   * Constructor.
   */
  Cipher();
  
  /**
   * Constructor.
   * @param cipher The cipher algorithm to use (def. aes-256-cbc).
   * @param digest The digest to use (def. sha1).
   * @param count  The number of iterations (def. 1).
   * @param embed  Embed the salt. If this is false, the output will 
   *               not be compatible with openssl.
   */
  Cipher(const std::string& cipher,
         const std::string& digest,
         uint count,
         bool embed=true);
  
  /**
   * Destructor.
   */
  ~Cipher();

  static void initialize();
public:
  /**
   * Encrypt buffer using AES 256 CBC (SHA1).
   * @param ifn   The plaintext buffer.
   * @param pass  The passphrase.
   * @param salt  The optional salt.
   * @returns The ciphertext: encrypted, MIME encoded data.
   */
  std::string encrypt(const std::string& plaintext,
                      const std::string& pass="",
                      const std::string& salt="");
  
  /**
   * Encrypt a file.
   *
   * Here is a usage example.
   * @code
   *   #include "cipher.h"
   *   #include <iostream>
   *   #include <string>
   *   using namepsace std;
   *
   *   void example()
   *   {
   *      string pass = "password";
   *      string salt = "12345678";    // must be 8
   *      string ifn  = "license.txt"; // plaintext
   *      string ofn  = "license.dat"; // ciphertext
   *
   *      // Encrypt file.
   *      // It is the same as running this command:
   *      // % openssl enc -e -a -md sha1 -aes-256-cbc -salt -p \
   *      //       -S 49505152535455676758                      \
   *      //       -pass pass:password                          \
   *      //       -in license.txt -out license.dat
   *      try {
   *         Cipher c;
   *         c.encrypt_file(ifn,ofn,pass,salt);
   *      }
   *      catch (exception& e) {
   *         cerr << "ERROR: " << e.what() << endl;
   *      }
   *   }
   * @endcode
   *
   * @param ifn   The plaintext file.
   * @param ofn   The encrypted file.
   * @param pass  The passphrase.
   * @param salt  The optional salt.
   * @throws runtime_error If a problem occurs.
   */
  void encrypt_file(const std::string& ifn,
                    const std::string& ofn,
                    const std::string& pass="",
                    const std::string& salt="");
public:
  /**
   * Decrypt a buffer using AES 256 CBC (SHA1).
   * @param ifn   The input file name.
   * @param pass  The passphrase.
   * @param salt  The optional salt.
   * @returns The plaintext: decrypted, MIME decoded data.
   */
  std::string decrypt(const std::string& ciphertext,
                      const std::string& pass="",
                      const std::string& salt="");
  
  /**
   * Decrypt a file.
   *
   * Here is a usage example.
   * @code
   *   #include "cipher.h"
   *   #include <iostream>
   *   #include <string>
   *   using namepsace std;
   *
   *   void example()
   *   {
   *      string pass = "password";
   *      string salt = "12345678";    // must be 8
   *      string ifn  = "license.dat"; // ciphertext
   *      string ofn  = "license.txt"; // plaintext
   *
   *      // Decrypt file.
   *      // It is the same as running this command:
   *      // % openssl enc -d -a -md sha1 -aes-256-cbc -salt -p \
   *      //       -pass pass:password                          \
   *      //       -in license.dat -out license.txt
   *      // NOTE: it is not necessary to specify the salt for decryption.
   *      try {
   *         Cipher c;
   *         c.decrypt_file(ifn,ofn,pass,salt);
   *      }
   *      catch (exception& e) {
   *         cerr << "ERROR: " << e.what() << endl;
   *      }
   *   }
   * @endcode
   * @param ifn   The encrypted file.
   * @param ofn   The plaintext file.
   * @param pass  The passphrase.
   * @param salt  The optional salt.
   */
  void decrypt_file(const std::string& ifn,
                    const std::string& ofn,
                    const std::string& pass="",
                    const std::string& salt="");
public:
  /**
   * Base64 encode.
   * @param ciphertext  Binary cipher text.
   * @param ciphertext_len  Length of cipher buffer.
   * @returns The encoded ASCII MIME string.
   */
  std::string encode_base64(uchar* ciphertext,
                            uint   ciphertext_len) const;
  
  /**
   * Cipher encode.
   * @param plaintext  ASCII data to encode.
   * @returns Binary data.
   */
  kv1_t encode_cipher(const std::string& plaintext) const;
  
  /**
   * Base64 decode.
   * @param mimetext  ASCII MIME text.
   * @returns Binary data.
   */
  kv1_t decode_base64(const std::string& mimetext) const;
  
  /**
   * Cipher decode.
   * @param ciphertext      Binary cipher text.
   * @param ciphertext_len  Length of cipher buffer.
   * @returns The decoded ASCII string.
   */
  std::string decode_cipher(uchar* ciphertext,
                            uint   ciphertext_len) const;
public:
  /**
   * Read a file into a buffer.
   * @param fn The file name.
   * @returns The file contents. 
   * @throws runtime_error if the file doesn't exist.
   */
  std::string file_read(const std::string& fn) const;
  /**
   * Write ASCII data to a file.
   * @param fn    The file name.
   * @param data  The data to write.
   * @throws runtime_error if the file cannot be written.
   */
  void file_write(const std::string& fn,
                  const std::string& data,
                  bool nl=false) const;
public:
  /**
   * Set the internal debug flag.
   * This is only useful for library developers.
   * @param b True for debug or false otherwise.
   */
  void debug(bool b=true) {m_debug=b;}
  /**
   * Is debug mode set?
   * @returns The current debug mode.
   */
  bool debug() const {return m_debug;}
private:
  /**
   * Convert string salt to internal format.
   * @param salt  The salt.
   */
  void set_salt(const std::string& salt);
  /**
   * Initialize the cipher: set the key and IV values.
   * @param pass  The passphrase.
   */
  void init(const std::string& pass);
  
private:
  std::string m_pass;
  std::string m_cipher;
  std::string m_digest;
  aes_salt_t  m_salt;
  aes_key_t   m_key;
  aes_iv_t    m_iv;
  uint        m_count;
  bool        m_embed;
  bool        m_debug;
};

#endif
