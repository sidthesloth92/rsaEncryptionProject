/*****************************************************************************************************************************
 * Project Name:
 *  Image Encryption and Image Recovery
 *
 * Authors:
 *  Dinesh Balaji
 *  dineshbalajiv.it@gmail.com
 *  Pratheesh
 *  iamprathu@gmail.com
 *
 ****************************************************************************************************************************/
 
#include<stdio.h>
#include<stdlib.h>
//contains structure definitions for bitmap images
#include "bmp.h"
 
int encryptColor(char*, char*);
int decryptColor(char*, char*);
int encryptTransposition(char*, char*);
int decryptTransposition(char*, char*);
int encrypt();
int decrypt();
void recover();
 
int main()
{
    int choice, option;
    char input[20], output[20];
    
    do{
        //displays menu to get to get input
	printf("\nMenu\n"
               "1. Encrypt\n"
               "2. Decrypt\n"
               "3. Recover\n"
               "4. Exit\n\n"
               "Enter your choice: \n");
               
        scanf("%d", &choice);
        
        switch(choice)
        {
            case 1: //reads input and output file names from the user
		    printf("Enter the input file name: ");
                    scanf("%s", input);
                    printf("Enter the output file name: ");
                    scanf("%s", output);
                    
                    do{
                        //asks for type of encryption to be performed
			printf("\nMethod\n"
                               "1. Color\n"
                               "2. Transposition\n"
                               "3. Exit\n"
                               "Enter your option: ");
                               
                        scanf("%d", &option);
                        
                        switch(option)
                        {
                            case 1: encryptColor(input, output);
                                    break;
                            
                            case 2: encryptTransposition(input, output);
                                    break;
                        }       
                    }while(option != 3);
                    
                    break;
                    
            case 2: //reads input and output file names from the user
		    printf("Enter the input file name: ");
                    scanf("%s", input);
                    printf("Enter the output file name: ");
                    scanf("%s", output);
                    
                    do{
                        //asks for type of encryption to be performed
			printf("\nMethod\n"
                               "1. Color\n"
                               "2. Transposition\n"
                               "3. Exit\n"
                               "Enter your option: \n");
                        scanf("%d", &option);
                        
                        switch(option)
                        {
                            case 1: decryptColor(input, output);
                                    break;
                            
                            case 2: decryptTransposition(input, output);
                                    break;
                        }       
                    }while(option != 3);
                    break;
                    
            case 3: recover();
                    break;
        }
        
    }while(choice != 4);
 
    return 0;
}

//performs encryption based on color distortion 
int encryptColor(char* infile,char* outfile) 
{

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
	    //if pixel color is white seed with red else convert to desired color
            if(triple.rgbtRed == 0xff && triple.rgbtGreen == 0xff && triple.rgbtBlue == 0xff) 
            {
                if(((rand() % 4) + 1) > 2)
                {
                    triple.rgbtRed = 0xff;
                    triple.rgbtGreen = 0x00;
                    triple.rgbtBlue = 0x00;
                }
            }
            else 
            {
                triple.rgbtRed = 0xff;
                triple.rgbtGreen = 0x45;
                triple.rgbtBlue= 0x00;
            }
            // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
            fputc(0x00, outptr);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);
    
    printf("Color Encryption Successful..:)\n");
    //that's all folks
    return 0;
} 

//performs decryption based on color distortion
int decryptColor(char* infile, char* outfile) 
{
    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            //if pixel color is red turn to white
	    //else if pixel color is the desired color cast to black
            if(triple.rgbtRed == 0xff && triple.rgbtGreen == 0x00 && triple.rgbtBlue == 0x00) 
            {
                triple.rgbtGreen = 0xff;
                triple.rgbtBlue = 0xff;
            }
            else if(triple.rgbtRed == 0xff && triple.rgbtGreen == 0x45 && triple.rgbtBlue == 0x00) 
            {
                triple.rgbtRed = 0x00;
                triple.rgbtGreen = 0x00;
                triple.rgbtBlue = 0x00;
            }

            // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
            fputc(0x00, outptr);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    printf("Color Decryption successful..:)");
    //that's all folks
    return 0;
}

//performs encryption based on transposition
int encryptTransposition(char* infile, char* outfile)
{
    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    
	
    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    RGBTRIPLE triple[abs(bi.biHeight)][bi.biWidth];
    
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // read RGB triple from infile
            fread(&triple[i][j], sizeof(RGBTRIPLE), 1, inptr);            
        }																																																																																																												
    
        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);
    }
    
    int key = encrypt();
    printf("Key Used: %d\n", key);
    key %= bi.biWidth;
        
    for (int i = 0, k = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++, k++)
        {
            // write RGB triple to outfile
            if((j %2) == 0)
                fwrite(&triple[i][(j + key) % bi.biWidth], sizeof(RGBTRIPLE), 1, outptr);
            else
                fwrite(&triple[i][( j + (bi.biWidth - key)) % bi.biWidth], sizeof(RGBTRIPLE), 1, outptr);           
        }
        
        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
            fputc(0x00, outptr);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    printf("Transposition Encryption Successful..:)");
    // that's all folks
    return 0;
}

//performs RSA encryption
int encrypt()
{
	long long e, n, temp = 1;	
	int pl;
	
	//gets public key from the user
	printf("Enter public key: ");
	scanf("%lld%lld", &e, &n);
	
	//gets user desired key
	printf("Enter the key: ");
	scanf("%d", &pl);

	for(int i = 0 ; i < e; i++)
		temp *= pl;
	
	//computes the key to be transmitted
	long long c = (temp % n);
	printf("Encrypted Value: %lld\n", c);
	
	//returns the user-desired key for encryption
	return pl;
}

//performs decryption based on transposition
int decryptTransposition(char* infile, char* outfile)
{
    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    RGBTRIPLE triple[abs(bi.biHeight)][bi.biWidth];
    
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // read RGB triple from infile
            fread(&triple[i][j], sizeof(RGBTRIPLE), 1, inptr);
        }
        
        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);
    }
    
    int key = decrypt();
    printf("Key Used: %d\n", key);
    key %= bi.biWidth;
    
    
    for (int i = 0, k = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++, k++)
        {
            // write RGB triple to outfile
            if((j %2) == 0)
                fwrite(&triple[i][( j + (bi.biWidth - key)) % bi.biWidth], sizeof(RGBTRIPLE), 1, outptr);
            else
                fwrite(&triple[i][(j + key) % bi.biWidth], sizeof(RGBTRIPLE), 1, outptr);           
        }
        
        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
            fputc(0x00, outptr);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}

//performs rsa decryption
int decrypt()
{
	long long d, n, c, temp = 1;

	//gets private key from the user
	printf("Enter private key: ");
	scanf("%lld%lld", &d, &n);
	
	//gets the transmitted key
	printf("Enter the key: ");
	scanf("%lld", &c);

	for(int i = 0 ; i < d; i++)
		temp *= c;
	
	//returns key for decryption
	return temp % n;
}

//declarations for recover 
typedef unsigned char MYBYTE;

//structure for 512 byte block 
struct jpeg
{
    MYBYTE block[512];
};
 
//defines the number of files to be recovered
#define FILENO 51

//performs recovery of images 
void recover()
{
    FILE *input, *output;
    char filename[8];
    input = fopen("card.raw", "r");
    int count = 0;
    int flag = 0;
    struct jpeg object;
    
    do 
    {
        //read a 512 byte block
	fread(&object, sizeof(object), 1, input);
        
        
        //if it is a jpeg block
	if(object.block[0] == 0xff)
            if(object.block[1] == 0xd8)
                if(object.block[2] == 0xff)
                    if(object.block[3] == 0xe0 || object.block[3] == 0xe1)
                    {    
                        if(flag == 2 && count == FILENO)
                             break;
                        flag = 1;
                    }
                        
        //create a new file and copy the contents of the block
	if(flag == 1)
        {
            count++;
            sprintf(filename, "%3d.jpg", count); 
            output = fopen(filename, "w");
            fwrite(&object, sizeof(object), 1, output);
            flag = 2;
        }

	//add some more contents and close the file if it is the last block of the jpeg
        if(flag == 2)
        {
            //open the file in append mode
	    output = fopen(filename, "a");
           
	    //move to end of output field
	    fseek(output,0,SEEK_END);
           
	    //write to the output file
	    fwrite(&object, sizeof(object), 1, output);
            
	    //close the output file
            fclose(output);
        }         
    }while(!feof(input));
    
    //close the input file
    fclose(input);
    
    printf("Images Successfully recovered...:)\n");
 }
