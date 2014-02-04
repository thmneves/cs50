/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize factor-number infile outfile\n");
        return 1;
    }
    
    int n = atoi(argv[1]);
    
    if (n < 0 || n > 100)
    {
        printf("factor number must be between 0 and 100\n");
        return 1;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

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
    
    //update header info
    int oldbiWidth = bi.biWidth;
    int oldbiHeight = bi.biHeight;
    bi.biWidth = oldbiWidth * n;
    bi.biHeight = oldbiHeight * n;
    
    

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int oldpadding =  (4 - (oldbiWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = (bi.biWidth * sizeof(RGBTRIPLE) + padding) * abs(bi.biHeight);
    bf.bfSize = (bi.biSizeImage) + 54; 
    
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(oldbiHeight); i < biHeight; i++)
    {
        // write scanlines n-1 times
        for (int l = 0; l < n; l++)
        {
            fpos_t position;
            fgetpos(inptr, &position);
            int track = 0;

            // iterate over pixels in scanline
            for (int j = 0; j < oldbiWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                     
                // write RGB triple n times to outfile
                for (int g = 0; g < n; g++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);               
                }
            
                track++;
            
                if (track < n)
                    fsetpos(inptr, &position);                      
            }
        }            

        // skip over padding, if any
        fseek(inptr, oldpadding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
