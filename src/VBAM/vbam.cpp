//===========================================================================
//===========================================================================
//===========================================================================
//==   Direct_Access_Image_Sample.cpp  ==  Author: Costin-Anton BOIANGIU   ==
//===========================================================================
//===========================================================================
//===========================================================================

//===========================================================================
//===========================================================================
#include "stdafx.h"
#include "Direct_Access_Image.h"
#include <string>
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "dirent.h"


//===========================================================================
//===========================================================================

//===========================================================================
//===========================================================================

using namespace std;

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

int main(int argc, char** argv)
{
    //Verify command-line usage correctness
    if (argc != 7)
    {
        _tprintf(_T("Use: %s <timeout_pixel timeout_init path_to_bams input_image_name path_to_dir_out output_image_name>\n"), argv[0]);
        return -1;
    }

	STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD dwRes;
    BOOL bRes;

	ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

	char path_to_exe[900];
	char command[956];

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (argv[3])) != NULL) {
	// print all the files and directories within directory
		while ((ent = readdir (dir)) != NULL) {
			if (strcmp(get_filename_ext(ent->d_name), "exe") == 0) {
				memset(path_to_exe, 0, 100);
				sprintf_s(path_to_exe, "%s\\\\%s", argv[3], ent->d_name);

				memset(command, 0, 256);
				sprintf_s(command, " /C %s %s", path_to_exe, argv[4]);

				//TODO get path to cmd from %COMSPEC% environment variable
				LPCWSTR cmd = (LPTSTR)"C:\\windows\\system32\\cmd.exe";
				LPTSTR params = (LPTSTR) command;

				//printf("%s\n", cmd);
				printf("%s%s\n", cmd, params);

				// execute bam
				bRes = CreateProcess( cmd, 
								params, 
								NULL,           // Process handle not inheritable 
								NULL,           // Thread handle not inheritable 
								FALSE,          // Set handle inheritance to FALSE 
								0,              // No creation flags 
								NULL,           // Use parent's environment block
								NULL,           // Use parent's starting directory 
								&si,            // Pointer to STARTUPINFO structure
								&pi             // Pointer to PROCESS_INFORMATION structure
								);

				if(bRes == FALSE) printf("CreateProcess failed\n");
 
				//Wait for the child to finish 
				dwRes = WaitForSingleObject(pi.hProcess, INFINITE);
				if(dwRes == WAIT_FAILED) printf("WaitForSingleObject");
 
				bRes = GetExitCodeProcess(pi.hProcess, &dwRes);
				if(bRes == FALSE) printf("%d\n", bRes);

				CloseHandle(&pi.hThread);
				CloseHandle(&pi.hProcess);
				
			}

		}
		closedir (dir);
	} else {
	// could not open directory
		perror ("");
		return EXIT_FAILURE;
	}
	
	
/*
    //Buffer for the new file names
    TCHAR strNewFileName[0x100];

    //Load and verify that input image is a True-Color one
    KImage *pImage = new KImage(argv[1]);
    if (pImage == NULL || !pImage->IsValid() || pImage->GetBPP() != 24)
    {
        _tprintf(_T("File %s does is not a valid True-Color image!"), argv[0]);
        return -2;
    }
	
    //Apply a Gaussian Blur with small radius to remove obvious noise
    pImage->GaussianBlur(0.5);
    _stprintf_s(strNewFileName, sizeof(strNewFileName) / sizeof(TCHAR), _T("%s_blurred.TIF"), argv[0]);
    pImage->SaveAs(strNewFileName, SAVE_TIFF_LZW);

    //Convert to grayscale
    KImage *pImageGrayscale = pImage->ConvertToGrayscale();
    //Don't forget to delete the original, now useless image
    delete pImage;

    //Verify conversion success...
    if (pImageGrayscale == NULL || !pImageGrayscale->IsValid() || pImageGrayscale->GetBPP() != 8)
    {
        _tprintf(_T("Conversion to grayscale was not successfull!"));
        return -3;
    }
    //... and save grayscale image
    _stprintf_s(strNewFileName, sizeof(strNewFileName) / sizeof(TCHAR), _T("%s_grayscale.TIF"), argv[0]);
    pImageGrayscale->SaveAs(strNewFileName, SAVE_TIFF_LZW);
    
    //Request direct access to image pixels in raw format
    BYTE **pDataMatrixGrayscale = NULL;
    if (pImageGrayscale->BeginDirectAccess() && (pDataMatrixGrayscale = pImageGrayscale->GetDataMatrix()) != NULL)
    {
        //If direct access is obtained get image attributes and start processing pixels
        int intWidth = pImageGrayscale->GetWidth();
        int intHeight = pImageGrayscale->GetHeight();

        //Create binary image
        KImage *pImageBinary = new KImage(intWidth, intHeight, 1);
        if (pImageBinary->BeginDirectAccess())
        {
            //Apply a threshold at half the grayscale range (0x00 is Full-Black, 0xFF is Full-White, 0x80 is the Middle-Gray)
            for (int y = intHeight - 1; y >= 0; y--)
                for (int x = intWidth - 1; x >= 0; x--)
                {
                    //You may use this instead of the line below: 
                    //    BYTE PixelAtXY = pImageGrayscale->Get8BPPPixel(x, y)
                    BYTE &PixelAtXY = pDataMatrixGrayscale[y][x];
                    if (PixelAtXY < 0x80)
                        //...if closer to black, set to black
                        pImageBinary->Put1BPPPixel(x, y, false);
                    else
                        //...if closer to white, set to white
                        pImageBinary->Put1BPPPixel(x, y, true);
                }

            //Close direct access
            pImageBinary->EndDirectAccess();
            
            //Save binarized image
            _stprintf_s(strNewFileName, sizeof(strNewFileName) / sizeof(TCHAR), _T("%s_Black_and_White.TIF"), argv[0]);
            pImageBinary->SaveAs(strNewFileName, SAVE_TIFF_CCITTFAX4);

            //Don't forget to delete the binary image
            delete pImageBinary;
        }
        else
        {
            _tprintf(_T("Unable to obtain direct access in binary image!"));
            return -3;
        }

        //Close direct access
        pImageGrayscale->EndDirectAccess();
    }
    else
    {
        _tprintf(_T("Unable to obtain direct access in grayscale image!"));
        return -4;
    }

    //Don't forget to delete the grayscale image
    delete pImageGrayscale;
	*/
    //Return with success
    return 0;
}
//===========================================================================
//===========================================================================
