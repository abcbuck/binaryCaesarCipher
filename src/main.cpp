#define UNICODE
#define _UNICODE

#include <iostream>
#include <fstream>
#include <Windows.h>
using namespace std;

enum buttons{
	BUTTON_ENCRYPT,
	BUTTON_DECRYPT
};

HWND	hEditFileName;
HWND	hEditEncryptionCode;
HWND	hEditOutputPanel;

LRESULT CALLBACK WndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow ){

	WNDCLASSEX WndClassEx;

	WndClassEx.cbSize					= sizeof( WNDCLASSEX );
	WndClassEx.style					= NULL;
	WndClassEx.lpfnWndProc		= WndProc;
	WndClassEx.cbClsExtra			= NULL;
	WndClassEx.cbWndExtra			= NULL;
	WndClassEx.hInstance			= hInstance;
	WndClassEx.hIcon					= NULL;
	WndClassEx.hCursor				= NULL;
	WndClassEx.hbrBackground	= GetSysColorBrush( COLOR_3DFACE );
	WndClassEx.lpszMenuName		= NULL;
	WndClassEx.lpszClassName	= L"WndClassEx";
	WndClassEx.hIconSm				= NULL;

	if( !RegisterClassEx( &WndClassEx ) ){
		MessageBox( NULL, L"Register Class failed.", L"ERROR", MB_ICONERROR | MB_OK );
		return false;
	}
	
	HWND hWnd = CreateWindowEx( WS_EX_CLIENTEDGE, L"WndClassEx", L"Data encryption program \x00A9 2011 by Robin Buck", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 444, 180, NULL, NULL, hInstance, NULL );
	ShowWindow( hWnd, nCmdShow );

	MSG Msg;

	while( GetMessage( &Msg, hWnd, NULL, NULL ) > NULL ){
		TranslateMessage( &Msg );
		DispatchMessage( &Msg );
	}

	return 0;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam ){
	switch( Msg ){
	case WM_CREATE:
		CreateWindow( L"static", L"File name:", WS_CHILD | WS_VISIBLE, 10, 10, 70, 20, hWnd, NULL, NULL, NULL );
		CreateWindow( L"static", L"Encryption code:", WS_CHILD | WS_VISIBLE, 10, 35, 110, 20, hWnd, NULL, NULL, NULL );
		hEditFileName = CreateWindowEx( WS_EX_CLIENTEDGE, L"edit", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 90, 10, 73, 20, hWnd, NULL, NULL, NULL );
		hEditEncryptionCode = CreateWindowEx( WS_EX_CLIENTEDGE, L"edit", L"", WS_CHILD | WS_VISIBLE, 130, 35, 33, 20, hWnd, NULL, NULL, NULL );
		hEditOutputPanel = CreateWindowEx( WS_EX_CLIENTEDGE, L"edit", L"Program started.", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 173, 10, 241, 120, hWnd, NULL, NULL, NULL );
		CreateWindow( L"button", L"Encrypt file", WS_CHILD | WS_VISIBLE, 10, 65, 153, 30, hWnd, (HMENU)BUTTON_ENCRYPT, NULL, NULL );
		CreateWindow( L"button", L"Decrypt file", WS_CHILD | WS_VISIBLE, 10, 100, 153, 30, hWnd, (HMENU)BUTTON_DECRYPT, NULL, NULL );
		break;
	case WM_COMMAND:
		switch( wParam ){
		case BUTTON_ENCRYPT:
		case BUTTON_DECRYPT:
			int encryptionCode;
			{
				wchar_t tmp[4];
				GetWindowText( hEditEncryptionCode, tmp, sizeof( tmp ) );
				encryptionCode = _wtoi( tmp );
			}
			if( encryptionCode <= 0 || encryptionCode >= 256 ){
				SetWindowText( hEditOutputPanel, L"Please enter an encryption code. It must be a value from 1 to 255." );
				break;
			}
			wchar_t Filename[238];
			GetWindowText( hEditFileName, Filename, sizeof( Filename ) );
			SetWindowText( hEditOutputPanel, L"Searching file..." );
			fstream fsFile( Filename, ios::binary | ios::_Nocreate | ios::in );
			if( !fsFile.is_open() ){
				SetWindowText( hEditOutputPanel, L"File was not found. Please enter a valid file name." );
				break;
			}
			SetWindowText( hEditOutputPanel, L"File was found." );
			SetWindowText( hEditOutputPanel, L"Opening file..." );
			long lSize;
			char* buffer;
			// obtain file size:
			fsFile.seekg( 0, ios_base::end );
			lSize = fsFile.tellg();
			fsFile.seekg( 0, ios_base::beg );
			// allocate memory to contain the whole file:
			buffer = new char [ sizeof(char) * lSize ];
			if( buffer == NULL ){
				SetWindowText( hEditOutputPanel, L"Memory error!!!" );
				break;
			}
			// copy the file into the buffer
			fsFile.read( buffer, lSize );
			/* the whole file is now loaded in the memory buffer. */
			SetWindowText( hEditOutputPanel, L"File opened." );
			fsFile.close();
			if( wParam == BUTTON_ENCRYPT )
				SetWindowText( hEditOutputPanel, L"Encrypting data..." );
			else if( wParam == BUTTON_DECRYPT )
				SetWindowText( hEditOutputPanel, L"Decrypting data..." );
			if( wParam == BUTTON_ENCRYPT ){
				for( long l = 0; l < lSize; l++ ){
					buffer[l] = (char)( (int)buffer[l] + encryptionCode );
				}
			}
			else if( wParam == BUTTON_DECRYPT ){
				for( long l = 0; l < lSize; l++ ){
					buffer[l] = (char)( (int)buffer[l] - encryptionCode );
				}
			}
			if( wParam == BUTTON_ENCRYPT )
				SetWindowText( hEditOutputPanel, L"Data encrypted." );
			else if( wParam == BUTTON_DECRYPT )
				SetWindowText( hEditOutputPanel, L"Data decrypted." );
			SetWindowText( hEditOutputPanel, L"Copying data back into the file..." );
			fsFile.open( Filename, ios::binary | ios::out );
			fsFile.write( buffer, sizeof(char) * lSize );
			SetWindowText( hEditOutputPanel, L"Data copied." );
			fsFile.close();
			delete buffer;
			if( wParam == BUTTON_ENCRYPT )
				SetWindowText( hEditOutputPanel, L"Encryption completed." );
			else if( wParam == BUTTON_DECRYPT )
				SetWindowText( hEditOutputPanel, L"Decryption completed." );
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(1);
		break;
	case WM_QUIT:
		DestroyWindow( hWnd );
		break;
	default:
		return DefWindowProc( hWnd, Msg, wParam, lParam );
	}
}
