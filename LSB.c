// LSB.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <locale.h>
typedef struct BITMAPINFOHEADER
{
	DWORD biSize;  //specifies the number of bytes required by the struct
	LONG biWidth;  //specifies width in pixels
	LONG biHeight;  //species height in pixels
	WORD biPlanes; //specifies the number of color planes, must be 1
	WORD biBitCount; //specifies the number of bit per pixel
	DWORD biCompression;//specifies the type of compression
	DWORD biSizeImage;  //size of image in bytes
	LONG biXPelsPerMeter;  //number of pixels per meter in x axis
	LONG biYPelsPerMeter;  //number of pixels per meter in y axis
	DWORD biClrUsed;  //number of colors used by th ebitmap
	DWORD biClrImportant;  //number of colors that are important
};
typedef struct BITMAPFILEHEADER
{
	WORD bfType;  //specifies the file type.If the file is BMP then it must be 0x4D42 .
	DWORD bfSize;  //specifies the size in bytes of the bitmap file
	WORD bfReserved1;  //reserved; must be 0
	WORD bfReserved2;  //reserved; must be 0
	DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
};
unsigned char *load_image_data(BITMAPINFOHEADER *image_info_header,char *image_file_name) {
		FILE *input_image; 
		BITMAPFILEHEADER image_file_header; 
		unsigned char *image_data;
		if ((input_image = fopen(image_file_name, "rb")) != NULL) {
			fread(&image_file_header, sizeof(BITMAPFILEHEADER), 1, input_image);
			if (image_file_header.bfType != 0x4D42) {//Is it BMP?
				fclose(input_image);
				return NULL;
			}
			fread(image_info_header, sizeof(BITMAPINFOHEADER), 1, input_image);
			//move file point to the begging of bitmap data
			fseek(input_image, image_file_header.bfOffBits, SEEK_SET);
			image_data = (unsigned char*)malloc(image_info_header->biSizeImage);
			if (!image_data) {
				free(image_data);
				fclose(input_image);
				return NULL;
			}
			//read in the bitmap image data
			fread(image_data, 1, (image_info_header->biSizeImage), input_image);
			if (image_data == NULL){
				fclose(input_image);
				return NULL;
			}
			return image_data;
		}
		else {
			printf("Can not open image!\n");
			return NULL;
		}
};
int encrypt(char *text_file_name, char *image_file_name) {
	BITMAPINFOHEADER image_info_header;
	unsigned char *image_data;
	image_data = load_image_data(&image_info_header,image_file_name);
	printf("%d", image_info_header.biSizeImage);
	//output the bitmap image data to the console
	for (int i = 0; i < image_info_header.biSizeImage; i++) {
		printf("%c", image_data[i]);
		if (i % 4 == 0) printf("\t");
	}
	return 1;
};
int decrypt(char *text_file_name, char *image_file_name) {\
	printf("Test passed\n");
};
int main(int argc,char **argv)
{
	setlocale(LC_ALL, "Russian");
	if (argc == 4) {
		char *keys[3] = { "-e","-d"};
		int is_key = 0;
		int(*lsb)(char *arg1, char *arg2);
		for (int i = 0; i < 2; i++) {
			if (strcmp(keys[i], argv[3]) == 0) {
				switch (i)
				{
				case 0:
					is_key = 1;
					lsb = encrypt;
					if (lsb(argv[1],argv[2]) == NULL) printf("Problem with FILE or low of memory!\n");
					break;
				case 1:
					is_key = 1;
					lsb = decrypt;
					if (lsb(argv[1], argv[2]) == NULL) printf("Problem with FILE or low of memory!\n");
					break;
				default:
					break;
				}
			}
		}
		if (is_key == 0) printf("Wrong parameter has been transferred\n");
		
	}
	else printf("Error! Only 3 parameters are requirted!\n");
    return 0;
}

