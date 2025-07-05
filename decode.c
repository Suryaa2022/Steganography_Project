#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"



/* Open stego image and output file */
Status open_files_decode(DecodeInfo *decInfo)
{
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
	if (decInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open stego image %s\n", decInfo->stego_image_fname);
		return e_failure;
	}

	decInfo->fptr_output = fopen(decInfo->output_fname, "w");
	if (decInfo->fptr_output == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open output file %s\n", decInfo->output_fname);
		return e_failure;
	}

	return e_success;
}


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	// Validate stego image file
	if (argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
	{
		decInfo->stego_image_fname = argv[2];
	}
	else
	{
		return e_failure;
	}

	// Check for output file
	if (argv[3] != NULL)
	{
		decInfo->output_fname = argv[3];
	}
	else
	{
		//Default output file
		decInfo->output_fname = "decoded.txt"; 
	}

	return e_success;
}

char decode_byte_from_lsb(char *image_buffer)
{
	char data = 0;
	for (int i = 0; i < 8; i++)
	{
		// Shift data left and add the LSB from each image byte
		data = (data << 1) | (image_buffer[i] & 0x01);
	}
	return data;
}

Status decode_data_from_image(char *data, int size, FILE *fptr_stego, DecodeInfo *decInfo)
{
	for (int i = 0; i < size; i++)
	{
		// Read 8 bytes from stego image
		fread(decInfo->image_data, sizeof(char), 8, fptr_stego);

		// Decode one character from LSB of 8 bytes
		data[i] = decode_byte_from_lsb(decInfo->image_data);
	}
	return e_success;
}

//Function to decode Magic string
Status decode_magic_string(const char *magic_str, DecodeInfo *decInfo)
{
	char str[strlen(magic_str)];

	decode_data_from_image(str, strlen(magic_str), decInfo->fptr_stego_image, decInfo);

	// Compare extracted magic string
	if (strncmp(str, magic_str, strlen(magic_str)) == 0)
	{
		printf("Magic string matched\n");
		return e_success;
	}
	else
	{
		printf("Magic string not match\n");
		return e_failure;
	}
}

/* Function to decode size (32 bits) from image buffer */
Status decode_size_from_lsb(char *image_buffer, long *size)
{
	*size = 0;

	for (int i = 0; i < 32; i++)
	{
		*size = (*size << 1) | (image_buffer[i] & 1);
	}
	return e_success;
}

/* Function to decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	char buffer[32];

	// Read 32 bytes from stego image
	fread(buffer, sizeof(char), 32, decInfo->fptr_stego_image);

	// Decode size 
	decode_size_from_lsb(buffer, &decInfo->extn_size);

	return e_success;
}

/* Function to decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	decode_data_from_image(decInfo->extn_secret_file, decInfo->extn_size, decInfo->fptr_stego_image, decInfo);

	// Add null-termination
	decInfo->extn_secret_file[decInfo->extn_size] = '\0';

	return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
	char buffer[32];

	// Read 32 bytes from stego image
	fread(buffer, sizeof(char), 32, decInfo->fptr_stego_image);

	// Decode size 
	decode_size_from_lsb(buffer, &decInfo->size_secret_file);

	return e_success;
}

// Function to decode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
	char data;

	// Loop to decode each byte of secret file data
	for (long i = 0; i < decInfo->size_secret_file; i++)
	{
		// Read 8 bytes from stego image
		fread(decInfo->image_data, sizeof(char), 8, decInfo->fptr_stego_image);

		// Decode one byte from LSB
		data = decode_byte_from_lsb(decInfo->image_data);

		// Write decoded byte to output file
		fputc(data, decInfo->fptr_output);
	}
	return e_success;
}

//call the rest of the decoding functions
Status do_decoding(DecodeInfo *decInfo)
{

	//open all the required files
	if(open_files_decode(decInfo) == e_success)
	{
		printf("Successfully opened all the required files for decoding\n");

		// Skip 54-byte BMP header
		fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

		if(decode_magic_string(MAGIC_STRING, decInfo) == e_success)
		{
			printf("Successfully decoded the Magic string\n");

			//Decode secret file extension size
			if(decode_secret_file_extn_size(decInfo) == e_success)
			{
				printf("Successfully decoded secret file extension size\n");
				printf("Decoded secret file extension size is : %ld\n", decInfo->extn_size);

				//Decode secret file extension
				if(decode_secret_file_extn(decInfo) == e_success)
				{
					printf("Successfully decoded secret file extension\n");
					printf("Decoded secret file extension is : %s\n", decInfo->extn_secret_file);

					//Decode secret file size
					if(decode_secret_file_size(decInfo) == e_success)
					{
						printf("Successfully decoded secret file size\n");
						printf("Decoded secret file size is : %ld\n", decInfo->size_secret_file);

						//Decode secret file data
						if(decode_secret_file_data(decInfo) == e_success)
						{
							printf("Successfully decoded secret file data\n");
						}
						else
						{
							printf("Failed to decode secret file data\n");
							return e_failure;
						}
					}
					else
					{
						printf("Failed to decode secret file size\n");
						return e_failure;
					}
				}
				else
				{
					printf("Failed to decode secret file extension\n");
					return e_failure;
				}
			}
			else
			{
				printf("Failed to decode secret file extension size\n");
				return e_failure;
			}
		}
		else
		{
			printf("Failed to decode the magic string\n");
			return e_failure;
		}

	}
	else
	{
		printf("Failed to open required files for decoding\n");
		return e_failure;
	}

	return e_success;

}

