#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
	//check operation type == -e (encode)
	if(check_operation_type(argv) == e_encode)
	{
		EncodeInfo encInfo;

		printf("------------Selected Encoding--------------\n");

		//Read and validate encode arguments
		if(read_and_validate_encode_args(argv, &encInfo) == e_success)
		{
			printf("Read and validation is sucessful\n");

			//Encoding part is called here
			if(do_encoding(&encInfo) == e_success)
			{
				printf("Encoding completed\n");
			}
			else
			{
				printf("Encoding failed\n");
			}
		}
		else
		{
			printf("Failed to validate the arguments\n");
		}
	}
	//check operation type == -d (decode)
	else if(check_operation_type(argv) == e_decode)
	{
		DecodeInfo decInfo;

		printf("------------Selected decoding--------------\n");

		//Read and validate decoding arguments
		if(read_and_validate_decode_args(argv, &decInfo) == e_success)
		{
			printf("Read and validation is successfull\n");

			//Decoding part is called here
			if(do_decoding(&decInfo) == e_success)
			{
				printf("Decoding completed\n");
			}
			else
			{
				printf("Decoding Failed\n");
			} 
		}
		else
		{
			printf("Failed to validate the arguments\n");
		}
	}
	else
	{
		printf("-------Invalid Option-------------\n");
		printf("**************Usage**************\n");
		printf("Encoding : ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
		printf("Decoding : ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
	}


	return 0;
}

//check operation type == -e or -d (encode or decode)
OperationType check_operation_type(char *argv[])
{
	//./a.out -e beautiful.bmp secret.txt
	if(strcmp(argv[1], "-e") == 0)
	{
		return e_encode;
	}
	else if(strcmp(argv[1], "-d") == 0)
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;
	}

}
