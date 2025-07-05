#ifndef DECODE_H
#define DECODE_H

#include "types.h"  // Contains user defined types

/*
 * Structure to store information required for
 * decoding secret data from stego Image
 */

 #define DECODE_MAX_IMAGE_BUF_SIZE 8
 #define DECODE_MAX_FILE_SUFFIX 5

 typedef struct _DecodeInfo
 {
     /* Stego Image Info */
     char *stego_image_fname;
     FILE *fptr_stego_image;
     char image_data[DECODE_MAX_IMAGE_BUF_SIZE];
 
     /* Output File Info */
     char *output_fname;
     FILE *fptr_output;
 
     /* Decoded Secret File Info */
     char extn_secret_file[DECODE_MAX_FILE_SUFFIX];
     long extn_size;
     long size_secret_file;

 } DecodeInfo;


 /* Decoding function prototype */

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* Decode one byte from LSBs */
char decode_byte_from_lsb(char *image_buffer);

/* Decode Magic String */
Status decode_magic_string(const char *magic_str, DecodeInfo *decInfo);

/* Decode secret file extension */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode 32-bit size from image buffer LSBs */
Status decode_size_from_lsb(char *image_buffer, long *size);

/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif 


