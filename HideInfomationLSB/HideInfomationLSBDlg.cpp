
// HideInfomationLSBDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "HideInfomationLSB.h"
#include "HideInfomationLSBDlg.h"
#include "afxdialogex.h"
#include "zlib.h"
#pragma comment(lib, "zlibstatic.lib")
#include <vector>
#include <string>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHideInfomationLSBDlg dialog



CHideInfomationLSBDlg::CHideInfomationLSBDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HIDEINFOMATIONLSB_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHideInfomationLSBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, INPUT_INFORMATION, inputInformation);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, inputDirecotyImage);
}

BEGIN_MESSAGE_MAP(CHideInfomationLSBDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_HIDE_INFORMATION, &CHideInfomationLSBDlg::OnBnClickedHideInformation)
	ON_BN_CLICKED(ID_SEPARATE_INFORMATION, &CHideInfomationLSBDlg::OnBnClickedSeparateInformation)
END_MESSAGE_MAP()


// CHideInfomationLSBDlg message handlers

BOOL CHideInfomationLSBDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHideInfomationLSBDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHideInfomationLSBDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHideInfomationLSBDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void HideBit(unsigned char& pixelByte, int bit) {
	if (bit == 1) pixelByte |= 1;  
	else pixelByte &= ~1;         
}

bool EmbedMessageLSB(const CString& imagePath, const std::string& message)
{
	// Load file PNG
	HANDLE hFile = CreateFileW(imagePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return false;

	DWORD fileSize = GetFileSize(hFile, NULL);
	std::vector<unsigned char> fileBuffer(fileSize);
	DWORD bytesRead;
	ReadFile(hFile, fileBuffer.data(), fileSize, &bytesRead, NULL);
	CloseHandle(hFile);

	// PNG signature
	const unsigned char pngHeader[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	if (memcmp(fileBuffer.data(), pngHeader, 8) != 0)
		return false; // không phải PNG

	// Thu thập toàn bộ IDAT
	std::vector<unsigned char> idatData;
	size_t pos = 8;

	while (pos + 12 <= fileBuffer.size())
	{
		// Length chunk
		uint32_t length =
			(fileBuffer[pos] << 24) |
			(fileBuffer[pos + 1] << 16) |
			(fileBuffer[pos + 2] << 8) |
			fileBuffer[pos + 3];

		if (pos + 12 + length > fileBuffer.size())
			break; // lỗi file

		char type[5] = { 0 };
		memcpy(type, &fileBuffer[pos + 4], 4);

		if (strcmp(type, "IDAT") == 0)
		{
			idatData.insert(idatData.end(),
				fileBuffer.begin() + pos + 8,
				fileBuffer.begin() + pos + 8 + length);
		}

		pos += length + 12;
	}

	if (idatData.empty())
		return false;

	// Giải nén toàn bộ IDAT
	unsigned long rawSize = 20 * 1024 * 1024; // 20MB
	std::vector<unsigned char> rawPixels(rawSize);

	if (uncompress(rawPixels.data(), &rawSize, idatData.data(), idatData.size()) != Z_OK)
		return false;

	rawPixels.resize(rawSize);

	// Chèn dữ liệu LSB vào rawPixels
	int pixelIndex = 0;
	for (char c : message)
	{
		for (int i = 7; i >= 0; i--)
		{
			int bit = (c >> i) & 1;

			if (pixelIndex >= rawPixels.size())
				return false;

			if (bit == 1) rawPixels[pixelIndex] |= 1;
			else rawPixels[pixelIndex] &= ~1;

			pixelIndex++;
		}
	}

	// Nén lại rawPixels → newIDAT
	unsigned long newIDATSize = compressBound(rawPixels.size());
	std::vector<unsigned char> newIDAT(newIDATSize);

	if (compress(newIDAT.data(), &newIDATSize, rawPixels.data(), rawPixels.size()) != Z_OK)
		return false;

	newIDAT.resize(newIDATSize);

	// Tạo file output PNG mới
	HANDLE out = CreateFileW(L"C:\\Users\\luong\\Downloads\\output_stego.png", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (out == INVALID_HANDLE_VALUE) return false;

	DWORD written;

	// 1. Ghi PNG header
	WriteFile(out, pngHeader, 8, &written, NULL);

	// 2. Ghi lại toàn bộ chunk cũ, nhưng thay IDAT bằng IDAT mới
	pos = 8;

	while (pos + 12 <= fileBuffer.size())
	{
		uint32_t length =
			(fileBuffer[pos] << 24) |
			(fileBuffer[pos + 1] << 16) |
			(fileBuffer[pos + 2] << 8) |
			fileBuffer[pos + 3];

		char type[5] = { 0 };
		memcpy(type, &fileBuffer[pos + 4], 4);

		if (strcmp(type, "IDAT") != 0)
		{
			// ghi nguyên chunk
			WriteFile(out, &fileBuffer[pos], 12 + length, &written, NULL);
		}
		else
		{
			// Ghi chunk IDAT mới
			uint32_t len = (uint32_t)newIDAT.size();
			unsigned char hdr[8] = {
				(len >> 24) & 0xFF,
				(len >> 16) & 0xFF,
				(len >> 8) & 0xFF,
				len & 0xFF,
				'I','D','A','T'
			};

			WriteFile(out, hdr, 8, &written, NULL);
			WriteFile(out, newIDAT.data(), newIDAT.size(), &written, NULL);

			// CRC tạm (bạn có thể tính đúng sau)
			uint32_t crc = 0;
			WriteFile(out, &crc, 4, &written, NULL);
		}

		pos += length + 12;
	}

	CloseHandle(out);
	return true;
}
std::string ExtractMessageLSB(const CString& imagePath, int msgLength)
{
	HANDLE hFile = CreateFileW(imagePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return "";

	DWORD fileSize = GetFileSize(hFile, NULL);
	std::vector<unsigned char> fileBuffer(fileSize);
	DWORD bytesRead;
	ReadFile(hFile, fileBuffer.data(), fileSize, &bytesRead, NULL);
	CloseHandle(hFile);

	const unsigned char pngHeader[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	if (memcmp(fileBuffer.data(), pngHeader, 8) != 0)
		return "";

	// Lấy tất cả IDAT
	std::vector<unsigned char> idatData;
	size_t pos = 8;

	while (pos + 12 <= fileBuffer.size())
	{
		uint32_t length =
			(fileBuffer[pos] << 24) |
			(fileBuffer[pos + 1] << 16) |
			(fileBuffer[pos + 2] << 8) |
			fileBuffer[pos + 3];

		char type[5] = { 0 };
		memcpy(type, &fileBuffer[pos + 4], 4);

		if (strcmp(type, "IDAT") == 0)
		{
			idatData.insert(idatData.end(),
				fileBuffer.begin() + pos + 8,
				fileBuffer.begin() + pos + 8 + length);
		}

		pos += length + 12;
	}

	unsigned long rawSize = 20 * 1024 * 1024;
	std::vector<unsigned char> rawPixels(rawSize);

	if (uncompress(rawPixels.data(), &rawSize, idatData.data(), idatData.size()) != Z_OK)
		return "";

	rawPixels.resize(rawSize);

	// Tách dữ liệu LSB
	std::string output;
	int pixelIndex = 0;

	for (int k = 0; k < msgLength; k++)
	{
		unsigned char c = 0;
		for (int i = 7; i >= 0; i--)
		{
			int bit = rawPixels[pixelIndex++] & 1;
			c |= (bit << i);
		}
		output.push_back(c);
	}

	return output;
}


void CHideInfomationLSBDlg::OnBnClickedHideInformation()
{
	CString inputStr, pathImage;
	inputInformation.GetWindowTextW(inputStr);
	inputDirecotyImage.GetWindowTextW(pathImage);

	std::string msg = CW2A(inputStr.GetString());

	if (EmbedMessageLSB(pathImage, msg))
		MessageBox(_T("Đã giấu tin thành công!"));
	else
		MessageBox(_T("Giấu tin thất bại!"));
}


void CHideInfomationLSBDlg::OnBnClickedSeparateInformation()
{
	CString pathImage;
	inputDirecotyImage.GetWindowTextW(pathImage);

	// ví dụ tách 50 ký tự
	std::string output = ExtractMessageLSB(pathImage, 50);

	CString msg(output.c_str());
	MessageBox(msg);
}
