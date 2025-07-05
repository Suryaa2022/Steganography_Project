#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);
	printf("width = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
	// Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
	// Do Error handling
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

		return e_failure;
	}

	// Secret file
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
	// Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}

	// Stego Image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	// Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}

	// No failure return e_success
	return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	//validate .bmp file
	if(argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
	{
		encInfo->src_image_fname = argv[2];
	}
	else
	{
		return e_failure;
	}

	//validate .txt file
	if(argv[3] != NULL && strcmp(strstr(argv[3], "."), ".txt") == 0)
	{
		encInfo->secret_fname = argv[3];
	}
	else
	{
		return e_failure;
	}

	//read optional argument - output image file
	if(argv[4] != NULL)
	{
		encInfo->stego_image_fname = argv[4];
	}
	else
	{
		encInfo->stego_image_fname = "stego.bmp"; //default file
	}
	return e_success;
}

//calculate file size
uint get_file_size(FILE *fptr_secret)
{
	fseek(fptr_secret, 0, SEEK_END);
	return ftell(fptr_secret);
}

//check capacity
Status check_capacity(EncodeInfo *encInfo)
{
	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

	if(encInfo->image_capacity > (54 + ((2 + 4 + 4 + 4 + encInfo->size_secret_file) * 8)))
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

//copy the header from src.bmp to stego.bmp
Status copy_bmp_header(FILE *fptr_src, FILE *fptr_dest)
{
	char header[54];
	fseek(fptr_src, 0, SEEK_SET);
	fread(header, sizeof(char), 54, fptr_src);
	fwrite(header, sizeof(char), 54, fptr_dest);
	return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
	unsigned char mask = 1 << 7;
	for(int i = 0; i < 8; i++)
	{
		//clear LSB bit from image buffer, then fetch a bit from MSB OF Data
		image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & mask) >> (7 - i));
		//change the mask to get the next bit from the data
		mask = mask >> 1;
	}
	return e_success;
}

Status encode_data_to_image(const char *data, int size, FILE *fptr_src, FILE *fptr_stego, EncodeInfo *encInfo)
{
	//run the loop for size of number 
	for(int i = 0; i < size; i++)
	{
		//read 8 bytes from src.bmp
		fread(encInfo->image_data, sizeof(char), 8, fptr_src);
		//call encode_byte_to_lsb to encode each character in a byte of RGB
		encode_byte_to_lsb(data[i], encInfo->image_data);
		//write the encoded image data to the destination image
		fwrite(encInfo->image_data, sizeof(char), 8, fptr_stego);
	}
	return e_success;
}

//Function to encode magic string
Status encode_magic_string(const char *magic_str, EncodeInfo *encInfo)
{
	//To encode every character needs to call encode_data_to_image 
	encode_data_to_image(magic_str, strlen(magic_str), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
	return e_success;
}

Status encode_size_to_lsb(char *image_buffer, int size)
{
	unsigned int mask = 1 << 31;

	for(int i = 0; i < 32; i++)
	{
		//clear LSB bit from image buffer, then fetch a bit from MSB OF Data
		image_buffer[i] = (image_buffer[i] & 0xFE) | ((size & mask) >> (31 - i));
		//change the mask to get the next bit from the data
		mask = mask >> 1;
	}
	return e_success;
}

//Function to encode secret file extension size
Status encode_size(int size, FILE *fptr_src, FILE *fptr_stego)
{
	char str[32];

	fread(str, 32, sizeof(char), fptr_src);
	encode_size_to_lsb(str, size);
	fwrite(str, 32, sizeof(char), fptr_stego);
	return e_success;
}

//Function to encode secret file extension
Status encode_secret_file_extn(const char *file_ext, EncodeInfo *encInfo)
{
	file_ext = ".txt";
	encode_data_to_image(file_ext, strlen(file_ext), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
	return e_success;
}

//Function to encode secret file size
Status encode_secret_file_size(long int size, EncodeInfo *encInfo)
{
	char str[32];

	fread(str, 32, sizeof(char), encInfo->fptr_src_image);
	encode_size_to_lsb(str, size);
	fwrite(str, 32, sizeof(char), encInfo->fptr_stego_image);
	return e_success;   
}

//Function to encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char ch;

	//Bring file pointer to the beginning of the file
	fseek(encInfo->fptr_secret, 0, SEEK_SET);

	for(int i = 0; i < encInfo->size_secret_file; i++)
	{
		//read 8 bytes from source image
		fread(encInfo->image_data, 8, sizeof(char), encInfo->fptr_src_image);
		//read a character from secret file
		fread(&ch, 1, sizeof(char), encInfo->fptr_secret);
		encode_byte_to_lsb(ch, encInfo->image_data);
		fwrite(encInfo->image_data, 8, sizeof(char), encInfo->fptr_stego_image);
	}
	return e_success;
}

//Function to copy the remaining RGB data
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_stego)
{
	char ch;

	while(fread(&ch, 1, 1, fptr_src) > 0)
	{
		fwrite(&ch, 1, 1, fptr_stego);
	}
}

//call the rest of the encoding functions here
Status do_encoding(EncodeInfo *encInfo)
{
	//open all the required files
	if(open_files(encInfo) == e_success)
	{
		printf("Successfully opened all the required files\n");

		//check capacity of .bmp source file
		if(check_capacity(encInfo) == e_success)
		{
			printf("check capacity is successfull\n");

			//copy the header into stego image
			if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
			{
				printf("Successfully copied the header\n");

				//Encode magic string
				if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
				{
					printf("successfully encoded the magic string\n");

					//Encode secret file extension size
					if(encode_size(strlen(".txt"), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
					{
						printf("Successfully encoded the secret file extension size\n");

						//Encode secret file extension
						if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
						{
							printf("Successfully encoded secret file extension\n");

							//Encode secret file size
							if(encode_secret_file_size(encInfo->size_secret_file, encInfo) ==  e_success)
							{
								printf("Successfully encoded secret file size\n");

								//Encode secret file data
								if(encode_secret_file_data(encInfo) == e_success)
								{
									printf("Encoded the secret data successfully\n");

									//copy the reamaining RGB data
									if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
									{
										printf("Successfully copied the remaining RGB Data\n");
									}
									else
									{
										printf("Failed to copy the remaining RGB Data\n");
										return e_failure;
									}
								}
								else
								{
									printf("Failed to encode the secret data\n");
									return e_failure;
								}
							}
							else
							{
								printf("Failed to encode secret file size\n");
								return e_failure;
							}
						}
						else
						{
							printf("Failed to encode the secret file extension\n");
						}
					}
					else
					{
						printf("Failed to encode the secret file extension size\n");
						return e_failure;
					}
				}
				else
				{
					printf("Failed to encode the magic string\n");
					return e_failure;
				}
			}
			else
			{
				printf("Failed to copy the header\n");
				return e_failure;
			}
		}
		else
		{
			printf("check capacity is failed\n");
			return e_failure;
		}

	}
	else
	{
		printf("Failed to open the files\n");
		e_failure;
	}
	return e_success;
}
