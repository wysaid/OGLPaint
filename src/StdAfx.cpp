// stdafx.cpp : source file that includes just the standard includes
//	xy.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


OPENFILENAME ofn ;
extern int g_cliWidth,g_cliHeight;
extern HDC g_hdc;
void FileInitialize (HWND hwnd)
{
     static TCHAR szFilter[] = TEXT ("Bitmaps (*.bmp)\0*.bmp\0")\
							   TEXT ("All Files (*.*)\0*.*\0\0");
     
     ofn.lStructSize       = sizeof (OPENFILENAME) ;
     ofn.hwndOwner         = hwnd ;
     ofn.hInstance         = NULL ;
     ofn.lpstrFilter       = szFilter ;
     ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = 0 ;
     ofn.nFilterIndex      = 0 ;
     ofn.lpstrFile         = NULL ;         
     ofn.nMaxFile          = MAX_PATH ;
     ofn.lpstrFileTitle    = NULL ;         
     ofn.nMaxFileTitle     = MAX_PATH ;
     ofn.lpstrInitialDir   = NULL ;
     ofn.lpstrTitle        = NULL ;
     ofn.Flags             = 0 ; 
     ofn.nFileOffset       = 0 ;
     ofn.nFileExtension    = 0 ;
     ofn.lpstrDefExt       = TEXT ("bmp") ;
     ofn.lCustData         = 0L ;
     ofn.lpfnHook          = NULL ;
     ofn.lpTemplateName    = NULL ;
}

BOOL FileSaveDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
     ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
     ofn.Flags             = OFN_OVERWRITEPROMPT ;
     
     return GetSaveFileName (&ofn) ;
}

void translateBitMapPixel(DWORD* &pixels, int width, int height)
{
	DWORD *tmp = new DWORD[height*width];
	//上下反转+颜色反转
	for(int i = 0; i != height; ++i)
	{
		int h = (height - i - 1) * width;
		for(int j=0; j != width; ++j)
		{
			DWORD px = pixels[h + j];
			unsigned char* pindex = (unsigned char*)(tmp +  i * width + j);
			*pindex = px >> 16 & 0xff;
			*(pindex + 1) = px >> 8 & 0xff;
			*(pindex + 2) = px & 0xff;
			*(pindex + 3) = 0xff;
		}
	}
	
// 	int cnt = width * height;
// 	for(int i = 0; i != cnt; ++i)
// 	{
// 		*(unsigned char*)(tmp+i) = pixels[i] >> 16 & 0xff;  //B
// 		*((unsigned char*)(tmp+i) + 1) = pixels[i] >> 8 & 0xff;  //G
// 		*((unsigned char*)(tmp+i) + 2) = pixels[i] & 0xff;  //R
// 		*((unsigned char*)(tmp+i) + 3) = 0xff;
// 	}

	delete pixels;
	pixels = tmp;
}

HBITMAP getBitMap()
{
	DWORD *pixels = new DWORD[g_cliWidth*g_cliHeight + 10];
	glReadPixels(0,0,g_cliWidth,g_cliHeight,GL_RGBA, GL_UNSIGNED_BYTE,pixels);
	translateBitMapPixel(pixels, g_cliWidth, g_cliHeight);
	HBITMAP hBitMap;
	hBitMap = CreateCompatibleBitmap(g_hdc, g_cliWidth, g_cliHeight);
	SetBitmapBits(hBitMap, g_cliWidth*g_cliHeight * 4,pixels);
	delete pixels;
	return hBitMap;
}

bool save(HWND hwnd)
{
	char szFilename[MAX_PATH] = "", szTitleName[MAX_PATH] = "";
	if(FileSaveDlg(hwnd, szFilename, szTitleName))
	{
		HDC   hDC; 
		WORD   wBitCount; 
		DWORD   dwPaletteSize=0,   dwBmBitsSize=0,   dwDIBSize=0,   dwWritten=0;   
		BITMAP   Bitmap;     
		BITMAPFILEHEADER   bmfHdr;     
		BITMAPINFOHEADER   bi;     
		LPBITMAPINFOHEADER   lpbi;     
		HANDLE   fh,   hDib,   hPal,hOldPal=NULL;   
		HBITMAP hBitmap = getBitMap();
 
		wBitCount = 24;
		GetObject(hBitmap,   sizeof(Bitmap),   (LPSTR)&Bitmap); 
		bi.biSize	 =   sizeof(BITMAPINFOHEADER); 
		bi.biWidth	 =   Bitmap.bmWidth; 
		bi.biHeight	 =   Bitmap.bmHeight; 
		bi.biPlanes	 =   1; 
		bi.biBitCount	 =   wBitCount; 
		bi.biCompression	=   BI_RGB; 
		bi.biSizeImage	 =   0; 
		bi.biXPelsPerMeter	=   0; 
		bi.biYPelsPerMeter	=   0; 
		bi.biClrImportant	=   0; 
		bi.biClrUsed	 =   0; 
		
		dwBmBitsSize   =   ((Bitmap.bmWidth   *   wBitCount   +   31)   /   32)   *   4   *   Bitmap.bmHeight;
		hDib   =   GlobalAlloc(GHND,dwBmBitsSize   +   dwPaletteSize   +   sizeof(BITMAPINFOHEADER));   
		lpbi   =   (LPBITMAPINFOHEADER)GlobalLock(hDib);   
		*lpbi   =   bi;   
		hPal   =   GetStockObject(DEFAULT_PALETTE);   
		if   (hPal)   
		{   
			hDC   =   ::GetDC(NULL);   
			hOldPal   =   ::SelectPalette(hDC,   (HPALETTE)hPal,   FALSE);   
			RealizePalette(hDC);   
		} 
		GetDIBits(hDC,   hBitmap,   0,   (UINT)   Bitmap.bmHeight,   (LPSTR)lpbi   +   sizeof(BITMAPINFOHEADER)   
			+dwPaletteSize,	(BITMAPINFO   *)lpbi,   DIB_RGB_COLORS);   
		if   (hOldPal)   
		{   
			::SelectPalette(hDC,   (HPALETTE)hOldPal,   TRUE);   
			RealizePalette(hDC);   
			::ReleaseDC(NULL,   hDC);   
		}   
		
		fh   =   CreateFile(szFilename,   GENERIC_WRITE,0,   NULL,   CREATE_ALWAYS,   
			FILE_ATTRIBUTE_NORMAL   |   FILE_FLAG_SEQUENTIAL_SCAN,   NULL);   
		
		if   (fh   ==   INVALID_HANDLE_VALUE)     return   false;   

		bmfHdr.bfType   =   0x4D42;  
		dwDIBSize   =   sizeof(BITMAPFILEHEADER)   +   sizeof(BITMAPINFOHEADER)   +   dwPaletteSize   +   dwBmBitsSize;     
		bmfHdr.bfSize   =   dwDIBSize;   
		bmfHdr.bfReserved1   =   0;   
		bmfHdr.bfReserved2   =   0;   
		bmfHdr.bfOffBits   =   (DWORD)sizeof(BITMAPFILEHEADER)   +   (DWORD)sizeof(BITMAPINFOHEADER)   +   dwPaletteSize;   
		WriteFile(fh,   (LPSTR)&bmfHdr,   sizeof(BITMAPFILEHEADER),   &dwWritten,   NULL);   
		WriteFile(fh,   (LPSTR)lpbi,   dwDIBSize,   &dwWritten,   NULL);   
		GlobalUnlock(hDib);   
		GlobalFree(hDib);   
		CloseHandle(fh);   
	}
	return true;
}