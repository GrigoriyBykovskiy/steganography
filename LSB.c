// LSB.c : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS
#define BMPIMAGENAME "OUTPUT_IMAGE.bmp"
#define TXTTEXTNAME "OUTPUT_TEXT.txt"
#define SETCOLORblack "000"
#define SETCOLORwhite "ÿÿÿ"
#include <stdio.h>
#include <windows.h>
#include <locale.h>
#pragma pack(push, 1)//padding (1 byte)
typedef struct BITMAPINFOHEADER {
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
typedef struct BITMAPFILEHEADER {
	WORD bfType;  //specifies the file type.If the file is BMP then it must be 0x4D42 .
	DWORD bfSize;  //specifies the size in bytes of the bitmap file
	WORD bfReserved1;  //reserved; must be 0
	WORD bfReserved2;  //reserved; must be 0
	DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
};
typedef struct RGBTRIPLE {
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
};
#pragma pack(pop)
char *load_text_data(char *text_file_name) {//it was strange,but the length of the resulting string is 1 more!
	char *tmp, *R = NULL;
	char ch;
	int lenght = 0;
	FILE *input_text = fopen(text_file_name, "r");
	if (input_text != NULL) {
		while (!feof(input_text)) {
			ch = fgetc(input_text);
			tmp = (char*)realloc(R, (lenght + 1)*(sizeof(int)));
			if (tmp == NULL) {
				printf("Error! Not enough memory\n");
				fclose(input_text);
				free(R);
				return NULL;
			}
			else {
				R = tmp;
				R[lenght] = ch;
				lenght++;
			}
		}
		tmp = (char*)realloc(R, (lenght)*(sizeof(int)));
		if (tmp == NULL) {
			free(R);
			return NULL;
		}
		else {
			R = tmp;
			R[lenght] = '\0';
		}
		fclose(input_text);
		return R;
	}
	else {
		printf("Can not open text file!\n");
		return NULL;
	}
};
unsigned char *load_image_data(char *image_file_name) {
	FILE *input_image;
	BITMAPFILEHEADER image_file_header;
	BITMAPINFOHEADER image_info_header;
	unsigned char *image_data;
	if ((input_image = fopen(image_file_name, "rb")) != NULL) {
		fread(&image_file_header, sizeof(BITMAPFILEHEADER), 1, input_image);
		fread(&image_info_header, sizeof(BITMAPINFOHEADER), 1, input_image);
		//Is it BMP?
		if ((image_file_header.bfType != 0x4D42) && (image_info_header.biBitCount != 24)) {
			printf("Unsupported format of file!\n");
			fclose(input_image);
			return NULL;
		}
		//Size of data is empty?
		if (image_info_header.biSizeImage == 0) {
			image_info_header.biSizeImage = image_info_header.biWidth * sizeof(RGBTRIPLE) + image_info_header.biWidth % 4;
			image_info_header.biSizeImage *= abs(image_info_header.biHeight);
		}
		//move file point to the begging of bitmap data
		fseek(input_image, image_file_header.bfOffBits, SEEK_SET);
		image_data = (unsigned char*)malloc(image_info_header.biSizeImage);
		if (!image_data) {
			printf("Error! Not enough memory\n");
			free(image_data);
			fclose(input_image);
			return NULL;
		}
		//read in the bitmap image data
		fread(image_data, 1, (image_info_header.biSizeImage), input_image);
		if (image_data == NULL) {
			printf("Can not read image data!\n");
			fclose(input_image);
			return NULL;
		}
		fclose(input_image);
		return image_data;
	}
	else {
		printf("Can not open image!\n");
		return NULL;
	}
};
RGBTRIPLE *get_pixel(BITMAPINFOHEADER *image_info_header, unsigned char *image_data, int x, int y) {
	RGBTRIPLE *pixel;
	int padding = image_info_header->biWidth % 4;
	int offset = sizeof(RGBTRIPLE)*x + (image_info_header->biWidth) * sizeof(RGBTRIPLE)*y + y*padding;
	/*printf("%d",offset);
	printf("%c%c%c", image_data[offset], image_data[offset+1], image_data[offset+2]);*/
	if ((pixel = (RGBTRIPLE *)malloc(sizeof(RGBTRIPLE))) != NULL) {
		pixel->rgbtBlue = image_data[offset];
		pixel->rgbtGreen = image_data[offset + 1];
		pixel->rgbtRed = image_data[offset + 2];
		return pixel;
	}
	else {
		printf("Error! Not enough memory\n");
		free(pixel);
		return NULL;
	}
};
void bit_check(char *symbol) {/*little-endian format*/
	int array_of_bits[8];
	printf("%c : ", *symbol);
	for (int i = 0; i < 8; i++) {
		if ((*symbol & (1 << i)) == 0) {
			array_of_bits[i] = 0;
		}
		else {
			array_of_bits[i] = 1;
		}
		printf("%d", array_of_bits[i]);
	}
	printf("\n");
};
void put_symbol(RGBTRIPLE*pixel, char *symbol) {
	int array_of_bits[8];
	//printf("symbol for encrypt %c : ", *symbol);
	for (int i = 0; i < 8; i++) {/*little-endian format*/
		if ((*symbol & (1 << i)) == 0) {
			array_of_bits[i] = 0;
		}
		else {
			array_of_bits[i] = 1;
		}
		//	printf("%d", array_of_bits[i]);
	}
	/*printf("\n");
	printf("before encrypt:\n%c%c%c\n", pixel->rgbtBlue, pixel->rgbtGreen, pixel->rgbtRed);
	bit_check(&(pixel->rgbtBlue));
	bit_check(&(pixel->rgbtGreen));
	bit_check(&(pixel->rgbtRed));*/
	for (int k = 0; k < 8; k++) {
		if (k <3) {
			//printf("%d\n", k);
			if (array_of_bits[k] == 1) {
				pixel->rgbtBlue |= (1 << k);
			}
			else {
				pixel->rgbtBlue &= ~(1 << k);
			}
		}
		if ((k >2) && (k<6)) {
			//printf("%d\n", k - 3);
			if (array_of_bits[k] == 1) {
				pixel->rgbtGreen |= (1 << k - 3);
			}
			else {
				pixel->rgbtGreen &= ~(1 << k - 3);
			}
		}
		if (k > 5) {
			//printf("%d\n", k-6);
			if (array_of_bits[k] == 1) {
				pixel->rgbtRed |= (1 << k - 6);
			}
			else {
				pixel->rgbtRed &= ~(1 << k - 6);
			}
		}
	}
	/*printf("\n");
	printf("after encrypt:\n%c%c%c\n", pixel->rgbtBlue, pixel->rgbtGreen, pixel->rgbtRed);
	bit_check(&(pixel->rgbtBlue));
	bit_check(&(pixel->rgbtGreen));
	bit_check(&(pixel->rgbtRed));*/
};
void get_symbol(RGBTRIPLE *pixel, unsigned char *symbol) {
	int count = 0;
	unsigned char buffer;
	unsigned char symbol_t = 0;
	while (count < 3) {
		if (count == 0) {
			buffer = pixel->rgbtBlue;
			//bit_check(&buffer);
			for (int i = 0; i < 3; i++) {
				if ((buffer & (1 << i)) == 0) {
					symbol_t &= ~(1 << i);
				}
				else {
					symbol_t |= (1 << i);
				}
			}
		}
		if (count == 1) {
			buffer = pixel->rgbtGreen;
			//bit_check(&buffer);
			for (int i = 0; i < 3; i++) {
				if ((buffer & (1 << i)) == 0) {
					symbol_t &= ~(1 << (i + 3));
				}
				else {
					symbol_t |= (1 << (i + 3));
				}
			}
		}
		if (count == 2) {
			buffer = pixel->rgbtRed;
			//bit_check(&buffer);
			for (int i = 0; i < 2; i++) {
				if ((buffer & (1 << i)) == 0) {
					symbol_t &= ~(1 << (i + 6));
				}
				else {
					symbol_t |= (1 << (i + 6));
				}
			}
		}
		count++;
	}
	*symbol = symbol_t;
	//bit_check(&symbol_t);
};
int check_lsb(RGBTRIPLE *pixel) {
	BYTE color_component;
	color_component = pixel->rgbtRed;
	if ((color_component & (1 << 0)) == 0) {
		return 0;
	}
	else {
		return 0;
	}
};
int decrypt(char *text_file_name, char *image_file_name) {
	FILE *input_image, *output_text;
	BITMAPFILEHEADER image_file_header;
	BITMAPINFOHEADER image_info_header;
	unsigned char *image_data;
	unsigned char *text_data = NULL;
	output_text = fopen(TXTTEXTNAME, "wb+");
	if ((input_image = fopen(image_file_name, "rb+")) != NULL) {
		fread(&image_file_header, sizeof(BITMAPFILEHEADER), 1, input_image);
		fread(&image_info_header, sizeof(BITMAPINFOHEADER), 1, input_image);
		fclose(input_image);
		image_data = load_image_data(image_file_name);
		for (int y = 0; y < image_info_header.biHeight; y++) {
			for (int x = 0; x < image_info_header.biWidth; x++) {
				RGBTRIPLE *pixel;
				char *text_symbol;
				pixel=get_pixel(&image_info_header, &image_data[0], x, y);
				get_symbol(pixel, &text_symbol);
				if (text_symbol != NULL) {
					fwrite(&text_symbol, sizeof(unsigned char), 1, output_text);
				}
				else {
					printf("Something was wrong!\n");
					return NULL;
				}
			}
		}
		printf("File decrypted! Check the working directory!\n");
		fclose(output_text);
		return 1;
	}
	else {
		printf("Can not open image file!\n");
		return NULL;
	}
};
int encrypt(char *text_file_name, char *image_file_name) {
	FILE *input_image, *output_image;
	BITMAPFILEHEADER image_file_header;
	BITMAPINFOHEADER image_info_header;
	unsigned char *image_data;
	char *text_data;
	output_image = fopen(BMPIMAGENAME, "wb+");
	if ((input_image = fopen(image_file_name, "rb")) != NULL) {
		fread(&image_file_header, sizeof(BITMAPFILEHEADER), 1, input_image);
		fwrite(&image_file_header, 1, sizeof(BITMAPFILEHEADER), output_image);
		fread(&image_info_header, sizeof(BITMAPINFOHEADER), 1, input_image);
		fwrite(&image_info_header, 1, sizeof(BITMAPINFOHEADER), output_image);
		fclose(input_image);
		image_data = load_image_data(image_file_name);
		text_data = load_text_data(text_file_name);
		if ((image_data == NULL) || (text_data == NULL)) {
			return NULL;
		}
		int padding = (image_info_header.biWidth) % 4;
		int count_text = 0;
		for (int y = 0; y < image_info_header.biHeight; y++) {
			for (int x = 0; x < image_info_header.biWidth; x++) {
				RGBTRIPLE *pixel;
				pixel = get_pixel(&image_info_header, &image_data[0], x, y);
				if (pixel != NULL) {
					//printf("%c\t%c\t%c\n",pixel->rgbtBlue,pixel->rgbtGreen,pixel->rgbtRed);
					if (count_text < (strlen(text_data) - 1)) {
						put_symbol(pixel,&text_data[count_text]);
						count_text++;
					}
					fwrite(pixel, sizeof(BYTE), sizeof(RGBTRIPLE), output_image);
				}
				else {
					printf("Something was wrong...\n");
					return NULL;
				}
			}
			for (int i = 0; i < padding; i++) {
				fwrite(SETCOLORblack, 1, 1, output_image);
			}
		}
		printf("File encrypted! Check the working directory!\n");
		fclose(output_image);
		return 1;
	}
	else {
		printf("Can not open image file!\n");
		return NULL;
	}
};
int visual_attack(char *text_file_name, char *image_file_name) {
	FILE *input_image, *output_image;
	BITMAPFILEHEADER image_file_header;
	BITMAPINFOHEADER image_info_header;
	unsigned char *image_data;
	output_image = fopen(BMPIMAGENAME, "wb+");
	if ((input_image = fopen(image_file_name, "rb+")) != NULL) {
		fread(&image_file_header, sizeof(BITMAPFILEHEADER), 1, input_image);
		fwrite(&image_file_header, 1, sizeof(BITMAPFILEHEADER), output_image);
		fread(&image_info_header, sizeof(BITMAPINFOHEADER), 1, input_image);
		fwrite(&image_info_header, 1, sizeof(BITMAPINFOHEADER), output_image);
		fclose(input_image);
		image_data = load_image_data(image_file_name);
		int padding = (image_info_header.biWidth) % 4;
		for (int y = 0; y < image_info_header.biHeight; y++) {
			for (int x = 0; x < image_info_header.biWidth; x++) {
				RGBTRIPLE *pixel;
				pixel = get_pixel(&image_info_header, &image_data[0], x, y);
				if (pixel != NULL) {
					unsigned char buffer = pixel->rgbtRed;
					if ((buffer & (1 << 0)) == 0) fwrite(SETCOLORwhite, 1, 3, output_image);
					else  fwrite(SETCOLORblack, 1, 3, output_image);
				}
				else {
					printf("Something was wrong...\n");
					return NULL;
				}
			}
			for (int i = 0; i < padding; i++) {
				fwrite(SETCOLORblack, 1, 1, output_image);
			}
		}
		printf("Process of attack is ending! Check the working directory!\n");
		fclose(output_image);
		return 1;
	}
	else {
		printf("Can not open image file!\n");
		return NULL;
	}
};
int main(int argc, char **argv)
{
	setlocale(LC_ALL, "Russian");
	if (argc == 4) {
		char *keys[4] = { "-encrypt","-decrypt","-analyze","-visual_attack"};
		int is_key = 0;
		int(*lsb)(char *arg1, char *arg2);
		for (int i = 0; i < 4; i++) {
			if (strcmp(keys[i], argv[3]) == 0) {
				switch (i)
				{
				case 0:
					is_key = 1;
					lsb = encrypt;
					if (lsb(argv[1], argv[2]) == NULL) printf("Problem with FILE or low of memory!\n");
					break;
				case 1:
					is_key = 1;
					lsb = decrypt;
					if (lsb(argv[1], argv[2]) == NULL) printf("Problem with FILE or low of memory!\n");
					break;
				case 2:
					is_key = 1;
					//lsb = analyze;
					if (lsb(argv[1], argv[2]) == NULL) printf("Problem with FILE or low of memory!\n");
					break;
				case 3:
					is_key = 1;
					lsb = visual_attack;
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
