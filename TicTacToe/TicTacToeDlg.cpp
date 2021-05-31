
// TicTacToeDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "TicTacToe.h"
#include "TicTacToeDlg.h"
#include "afxdialogex.h"
#include <string>
#include <iostream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <winsock2.h>

#define DEFAULT_PORT	5150
#define DEFAULT_BUFFER	2048

HWND hWnd_LB;
HWND hWnd_Dlg;

// Диалоговое окно CTicTacToeDlg

bool	m_IsConnected;
SOCKET	m_sClient;
CTicTacToeDlg *mainDlg;

UINT ListenThread(PVOID lpParam);

CTicTacToeDlg::CTicTacToeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_TICTACTOE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTicTacToeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTBOX, m_ListBox);
}

BEGIN_MESSAGE_MAP(CTicTacToeDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_CONNECT, &CTicTacToeDlg::OnBnClickedConnect)
END_MESSAGE_MAP()


// Обработчики сообщений CTicTacToeDlg

BOOL CTicTacToeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	char Str[128];

	GetDlgItem(IDC_SERVER)->SetWindowText("localhost");
	//sprintf_s(Str, sizeof(Str), "%d", DEFAULT_COUNT);
	//GetDlgItem(IDC_NUMBER)->SetWindowText(Str);
	sprintf_s(Str, sizeof(Str), "%d", DEFAULT_PORT);
	GetDlgItem(IDC_PORT)->SetWindowText(Str);
	//GetDlgItem(IDC_MESSAGE)->SetWindowText(DEFAULT_MESSAGE);
	//m_NoEcho.SetCheck(0);
	SetConnected(false);
	hWnd_LB = m_ListBox.m_hWnd;   // Для ListenThread
	hWnd_Dlg = this->m_hWnd;
	mainDlg = this;
	int i;
	for (i = 0; i < 11; i++) {
		Pastuszak[i] = 0;
		posX[i] = 0;
		posY[i] = 0;
	}
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CTicTacToeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // device context for painting
		CClientDC cdc(this);
		DrawGrid(&dc);

		CDialog::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CTicTacToeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTicTacToeDlg::SetConnected(bool IsConnected)
{
	m_IsConnected = IsConnected;

	//GetDlgItem(IDC_SEND)->EnableWindow(IsConnected);
	//GetDlgItem(IDC_MESSAGE)->EnableWindow(IsConnected);
	//GetDlgItem(IDC_NUMBER)->EnableWindow(IsConnected);
	//GetDlgItem(IDC_NO_ECHO)->EnableWindow(IsConnected);
	GetDlgItem(IDC_SERVER)->EnableWindow(!IsConnected);
	GetDlgItem(IDC_PORT)->EnableWindow(!IsConnected);
	GetDlgItem(IDC_CONNECT)->EnableWindow(!IsConnected);
}



// This Function Draw the tic tac toe GRID on the screen
void CTicTacToeDlg::DrawGrid(CPaintDC* pdc)
{
	// Declare and create the pens
	CPen GridSize(PS_SOLID, 7, RGB(0, 0, 0));

	// Get the Drawing Area.
	CRect lRect;
	GetClientRect(lRect);
	lRect.NormalizeRect();

	// Caclulate the distane 
	CPen* lOldPen;	// The Old penobject

	CPoint pStart;	// Define the starting point
	CPoint pEnd;	// Define the ending point

	// X1=50 X2 =(300-50) 
	pStart.y = 150;
	pStart.x = 50;
	pEnd.y = pStart.y;
	pEnd.x = 350;

	lOldPen = pdc->SelectObject(&GridSize);
	// Draw the line
	pdc->MoveTo(pStart);
	pdc->LineTo(pEnd);

	// Y1=150
	pStart.y = 250;
	pStart.x = 50;
	pEnd.y = pStart.y;
	pEnd.x = 350;

	lOldPen = pdc->SelectObject(&GridSize);
	// Draw the line
	pdc->MoveTo(pStart);
	pdc->LineTo(pEnd);


	// DRAW THE VARTICAL LINES NOW

	pStart.y = 50;
	pStart.x = 150;

	pEnd.y = 350;
	pEnd.x = pStart.x;

	lOldPen = pdc->SelectObject(&GridSize);
	// Draw the line
	pdc->MoveTo(pStart);
	pdc->LineTo(pEnd);


	pStart.y = 50;
	pStart.x = 250;

	pEnd.y = 350;
	pEnd.x = pStart.x;

	lOldPen = pdc->SelectObject(&GridSize);
	// Draw the line
	pdc->MoveTo(pStart);
	pdc->LineTo(pEnd);


	// Select the original pen
	pdc->SelectObject(lOldPen);
}

void CTicTacToeDlg::DrawX(CClientDC* pdc, int x, int y)
{
	// Check if the user is between anything
	// Declare and create the pens
	CPen XSize(PS_SOLID, 7, RGB(0, 0, 0));

	// Get the Drawing Area.
	CRect lRect;
	GetClientRect(lRect);
	lRect.NormalizeRect();

	// Caclulate the distane 
	CPen* lOldPen;	// The Old penobject

	CPoint pStart;	// Define the starting point
	CPoint pEnd;	// Define the ending point

	// Starting Point
	pStart.y = y;
	pStart.x = x;

	pEnd.y = y + 50;
	pEnd.x = x + 50;

	lOldPen = pdc->SelectObject(&XSize);
	// Draw the line
	pdc->MoveTo(pStart);
	pdc->LineTo(pEnd);

	// DO the Other Side

	// Starting Point
	pStart.y = y;
	pStart.x = x + 50;

	pEnd.y = y + 50;
	pEnd.x = x;

	lOldPen = pdc->SelectObject(&XSize);
	// Draw the line
	pdc->MoveTo(pStart);
	pdc->LineTo(pEnd);


	// Select the original pen
	pdc->SelectObject(lOldPen);
}

void CTicTacToeDlg::DrawO(CClientDC* pdc, int x, int y)
{
	// Declare and create the pens
	CPen OSize(PS_SOLID, 6, RGB(0, 0, 0));
	CBrush lBlack(RGB(192, 192, 192));

	CPen* lOldPen;	// The Old penobject
	CBrush* lOldBrush;

	lOldPen = pdc->SelectObject(&OSize);
	lOldBrush = pdc->SelectObject(&lBlack);

	pdc->Ellipse(x, y, x + 75, y + 75);


	// Select the original pen
	pdc->SelectObject(lOldPen);
	pdc->SelectObject(lOldBrush);
}

void CTicTacToeDlg::PlotPlayer(CPoint point)
{
	//UpdateData(TRUE);
	m_Player++;
	// User Choice to play with X.

	CClientDC sdc(this); // device context for painting
	//CString strPoint;
	//strPoint.Format("%d, %d, %d",point.x,point.y, m_Player);
	// Check to see where then mouse was click
	if (point.x > 47 && point.x < 143 && point.y > 47 && point.y < 143)
	{
		// Check if their is a player in that postion.
		if (Pastuszak[1] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}

		if (m_Player % 2 == 0)
		{
			// Top - Left
			//Set that an X was Plot.
			Pastuszak[1] = 1;
			posX[1] = 70;
			posY[1] = 70;
			DrawX(&sdc, posX[1], posY[1]);
			GridErr = 0;
			return;
		}
		else if (m_Player % 2 == 1) {
			// Set the X & Y positions to draw the cricle
			// Set that an O was plot.
			Pastuszak[1] = 2;
			posX[1] = 65;
			posY[1] = 65;
			DrawO(&sdc, posX[1], posY[1]);
			GridErr = 0;
			return;
		}

	}
	else if (point.x > 152 && point.x < 249 && point.y > 47 && point.y < 143) {
		// Check if their is a player in that postion.
		if (Pastuszak[2] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		// Top - Middle
		if (m_Player % 2 == 0)
		{
			Pastuszak[2] = 1;
			posX[2] = 170;
			posY[2] = 70;

			DrawX(&sdc, posX[2], posY[2]);
			GridErr = 0;
			return;
		}
		else if (m_Player % 2 == 1) {
			// Set the X & Y positions to draw the cricle

			Pastuszak[2] = 2;
			posX[2] = 165;
			posY[2] = 65;
			DrawO(&sdc, posX[2], posY[2]);
			GridErr = 0;
			return;
		}

	}
	else if (point.x > 253 && point.x < 350 && point.y > 47 && point.y < 143) {
		// Check if their is a player in that postion.
		if (Pastuszak[3] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		if (m_Player % 2 == 0)
		{
			// Top - Right

			Pastuszak[3] = 1;
			posX[3] = 270;
			posY[3] = 70;

			DrawX(&sdc, posX[3], posY[3]);
			GridErr = 0;

			return;
		}
		else if (m_Player % 2 == 1) {
			// Set the X & Y positions to draw the cricle

			Pastuszak[3] = 2;
			posX[3] = 265;
			posY[3] = 65;
			DrawO(&sdc, posX[3], posY[3]);
			GridErr = 0;
			return;
		}

	}
	else if (point.x > 47 && point.x < 143 && point.y > 152 && point.y < 245) {
		// Check if their is a player in that postion.
		if (Pastuszak[4] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		// Middle - left
		if (m_Player % 2 == 0)
		{
			Pastuszak[4] = 1;
			posX[4] = 70;
			posY[4] = 180;
			GridErr = 0;

			DrawX(&sdc, posX[4], posY[4]);

			return;
		}
		else if (m_Player % 2 == 1) {
			// Set the X & Y positions to draw the cricle

			Pastuszak[4] = 2;
			posX[4] = 65;
			posY[4] = 165;
			DrawO(&sdc, posX[4], posY[4]);
			GridErr = 0;

			return;
		}

	}
	else if (point.x > 152 && point.x < 249 && point.y > 152 && point.y < 245) {
		// Check if their is a player in that postion.
		if (Pastuszak[5] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		// Middle - Middle
		if (m_Player % 2 == 0)
		{

			Pastuszak[5] = 1;
			posX[5] = 170;
			posY[5] = 180;

			DrawX(&sdc, posX[5], posY[5]);
			GridErr = 0;


			return;
		}
		else if (m_Player % 2 == 1) {
			// Set the X & Y positions to draw the cricle

			Pastuszak[5] = 2;
			posX[5] = 165;
			posY[5] = 165;
			DrawO(&sdc, posX[5], posY[5]);
			GridErr = 0;

			return;
		}

	}
	else if (point.x > 253 && point.x < 350 && point.y > 152 && point.y < 245) {
		// Check if their is a player in that postion.
		if (Pastuszak[6] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		// Middle - Right
		if (m_Player % 2 == 0)
		{


			Pastuszak[6] = 1;
			posX[6] = 270;
			posY[6] = 180;

			DrawX(&sdc, posX[6], posY[6]);
			GridErr = 0;

			return;
		}
		else if (m_Player % 2 == 1) {
			// Set the X & Y positions to draw the cricle
			point.x = 265;
			point.y = 165;

			Pastuszak[6] = 2;
			posX[6] = 265;
			posY[6] = 165;
			DrawO(&sdc, posX[6], posY[6]);
			GridErr = 0;

			return;
		}

	}
	else if (point.x > 47 && point.x < 143 && point.y > 251 && point.y < 351) {
		// Check if their is a player in that postion.
		if (Pastuszak[7] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		// Bottom - left
		if (m_Player % 2 == 0)
		{
			Pastuszak[7] = 1;
			posX[7] = 70;
			posY[7] = 280;

			DrawX(&sdc, posX[7], posY[7]);
			GridErr = 0;

			return;
		}
		else if (m_Player % 2 == 1) {
			// Set the X & Y positions to draw the cricle
			point.x = 65;
			point.y = 265;

			Pastuszak[7] = 2;
			posX[7] = 65;
			posY[7] = 265;
			DrawO(&sdc, posX[7], posY[7]);
			GridErr = 0;

			return;
		}

	}
	else if (point.x > 152 && point.x < 249 && point.y > 251 && point.y < 351) {
		// Check if their is a player in that postion.
		if (Pastuszak[8] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		// Bottom - Middle
		if (m_Player % 2 == 0)
		{

			Pastuszak[8] = 1;
			posX[8] = 170;
			posY[8] = 280;

			DrawX(&sdc, posX[8], posY[8]);
			GridErr = 0;

			return;
		}
		else if (m_Player % 2 == 1) {
			// Set the X & Y positions to draw the cricle

			Pastuszak[8] = 2;
			posX[8] = 165;
			posY[8] = 265;
			DrawO(&sdc, posX[8], posY[8]);
			GridErr = 0;
			return;
		}

	}
	else if (point.x > 254 && point.x < 350 && point.y > 251 && point.y < 351) {
		// Check if their is a player in that postion.
		if (Pastuszak[9] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		// Bottom - Right
		if (m_Player % 2 == 0)
		{

			Pastuszak[9] = 1;
			posX[9] = 270;
			posY[9] = 280;

			DrawX(&sdc, posX[9], posY[9]);
			GridErr = 0;

			return;
		}
		else if (m_Player % 2 == 1) {

			// Set the X & Y positions to draw the cricle
			Pastuszak[9] = 2;
			posX[9] = 265;
			posY[9] = 265;
			DrawO(&sdc, posX[9], posY[9]);
			GridErr = 0;
			return;
		}
	}
	m_Player--;
}

void CTicTacToeDlg::DrawLine(CClientDC* pdc, int x, int y, int x2, int y2)// Red Line drawing at game's end
{
	// Declare and create the pens
	CPen cLine(PS_SOLID, 7, RGB(200, 0, 0));

	// Get the Drawing Area.
	CRect lRect;
	GetClientRect(lRect);
	lRect.NormalizeRect();

	// Caclulate the distane 
	CPen* lOldPen;	// The Old penobject

	CPoint pStart;	// Define the starting point
	CPoint pEnd;	// Define the ending point

	// X1=50 X2 =(300-50) 
	pStart.y = y;
	pStart.x = x;
	pEnd.y = y2;
	pEnd.x = x2;

	lOldPen = pdc->SelectObject(&cLine);
	// Draw the line
	pdc->MoveTo(pStart);
	pdc->LineTo(pEnd);

	// Select the original pen
	pdc->SelectObject(lOldPen);
}

void CTicTacToeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) == MK_LBUTTON)
	{
		char	szMessage[1024];// Сообщение для отправки
		char	szBuffer[DEFAULT_BUFFER];
		int		ret, i;

		char	Str[256];
		UpdateData(TRUE);

		long x = point.x;
		long y = point.y;

		std::string string1 = std::to_string(x);
		std::string string2 = std::to_string(y);
		std::string string3 = string1 + " " + string2;
		const char* cstr = string3.c_str();

		sprintf_s(szMessage, sizeof(szMessage), cstr);

		// Отправка данных 
		ret = send(m_sClient, szMessage, strlen(szMessage), 0);
		if (ret == 0)
			;
		else if (ret == SOCKET_ERROR)
		{
			sprintf_s(Str, sizeof(Str), "send() failed: %d",
				WSAGetLastError());
			m_ListBox.AddString((LPTSTR)Str);
			;
		}

	}

	// Обработка принятых даннных(наверное)
	//if ((nFlags & MK_LBUTTON) == MK_LBUTTON)
	//{
	//	UpdateData(TRUE);

	//	// Check to see if the hole grid is filled out.
	//	int i, g = 1;
	//	for (i = 1; i < 10; i++)
	//	{
	//		if (Pastuszak[i] == 1 || Pastuszak[i] == 2) g++;
	//	}

	//	// Show a message if g = 9 to tell the user it's game over.
	//	if (g >= 10) {
	//		MessageBox("Game Over", "Game Over.");
	//		//GAME_STATUS = 3; // Game Over.
	//		return;
	//	}

	//	PlotPlayer(point);
	//	CheckWhoWon();
	//}
	CDialog::OnLButtonDown(nFlags, point);
}

void CTicTacToeDlg::CheckWhoWon()
{
	CClientDC pdc(this);
	//int found=1;	// dis hole the count how many found max 3
	int iLook;		// iLook is what the current piecs being look at.

	// This show how the grid looks like and that how i looking up so that every equals

	//   1|  2  | 3
	// -------------
	//   4|  5  | 6
	// -------------
	//   7|  8  | 9
	//MessageBox("Click");
	// Frist we are going to check down to right \ //
	iLook = Pastuszak[1]; // the frist element.
	if (Pastuszak[5] == iLook && Pastuszak[9] == iLook && iLook != 0)
	{
		// Some won the game.
		DrawLine(&pdc, 50, 50, 350, 350);
		// Check who won
		if ((m_Player + 1) == iLook) {
			MessageBox("Player won the game.");
		}
		else {
			MessageBox("Computer won the game.");
		}
		GAME_STATUS = 3;
		return;
	}
	// We Find the other way /
	iLook = Pastuszak[3]; // the frist element.
	//if (iLook == 0) { return;}
	if (Pastuszak[5] == iLook && Pastuszak[7] == iLook && iLook != 0)
	{
		// Some won the game.
		DrawLine(&pdc, 350, 50, 50, 350);
		// Check who won
		if ((m_Player + 1) == iLook) {
			MessageBox("Player won the game.");
		}
		else {
			MessageBox("Computer won the game.");
		}
		GAME_STATUS = 3;
		return;
	}
	// So Far Done Check
	// D=done
	// D | 2 | D
	// ---------
	// 4 | D | 6
	// ---------
	// D | 8 | D

	// Look at the top.
	iLook = Pastuszak[1]; // the frist element.
	//if (iLook == 0) { return;}
	if (Pastuszak[2] == iLook && Pastuszak[3] == iLook && iLook != 0)
	{
		// Some won the game.

		// Check who won
		DrawLine(&pdc, 50, 100, 350, 100);
		if ((m_Player + 1) == iLook) {
			MessageBox("Player won the game.");
		}
		else {
			MessageBox("Computer won the game.");
		}
		GAME_STATUS = 3;

		return;
	}

	// Check the right side going down.
	iLook = Pastuszak[3]; // the frist element.
	//if (iLook == 0) { return;}
	if (Pastuszak[6] == iLook && Pastuszak[9] == iLook && iLook != 0)
	{
		// Some won the game.
		DrawLine(&pdc, 300, 50, 300, 350);
		// Check who won
		if ((m_Player + 1) == iLook) {
			MessageBox("Player won the game.");
		}
		else {
			MessageBox("Computer won the game.");
		}
		GAME_STATUS = 3;
		return;
	}

	// So Far Done Check
	// D=done
	// D | D | D
	// ---------
	// 4 | D | D
	// ---------
	// D | 8 | D

	// Check the bottom.
	iLook = Pastuszak[7]; // the frist element.
	//if (iLook == 0) { return;}
	if (Pastuszak[8] == iLook && Pastuszak[9] == iLook && iLook != 0)
	{
		// Some won the game.
		// Check who won
		DrawLine(&pdc, 50, 300, 350, 300);
		if ((m_Player + 1) == iLook) {
			MessageBox("Player won the game.");
		}
		else {
			MessageBox("Computer won the game.");
		}
		GAME_STATUS = 3;
		return;
	}

	// Check the left going down.
	iLook = Pastuszak[1]; // the frist element.
	//if (iLook == 0) { return;}
	if (Pastuszak[4] == iLook && Pastuszak[7] == iLook && iLook != 0)
	{
		// Some won the game.
		DrawLine(&pdc, 100, 50, 100, 350);
		// Check who won
		if ((m_Player + 1) == iLook) {
			MessageBox("Player won the game.");
		}
		else {
			MessageBox("Computer won the game.");
		}
		GAME_STATUS = 3;

		return;
	}
	// So Far Done Check
	// D=done
	// M=Middle Not Done
	// D  | DM2  | D
	// ---------
	// DM4 | DM5 | DM6
	// ---------
	// D  | DM8  | D

	// The Last Two Fun need to done middle and middle up

	// Middle Up
	iLook = Pastuszak[2]; // the frist element.
	//if (iLook == 0) { return;}
	if (Pastuszak[5] == iLook && Pastuszak[8] == iLook && iLook != 0)
	{
		// Some won the game.
		// Check who won
		DrawLine(&pdc, 200, 50, 200, 350);
		if ((m_Player + 1) == iLook) {
			MessageBox("Player won the game.");
		}
		else {
			MessageBox("Computer won the game.");
		}
		GAME_STATUS = 3;
		return;
	}

	// Middle Side
	iLook = Pastuszak[4]; // the frist element.
	//if (iLook == 0) { return;}
	if (Pastuszak[5] == iLook && Pastuszak[6] == iLook && iLook != 0)
	{
		// Some won the game.

		// Check who won
		DrawLine(&pdc, 50, 200, 350, 200);
		if ((m_Player + 1) == iLook) {
			MessageBox("Player won the game.");
		}
		else {
			MessageBox("Computer won the game.");
		}

		GAME_STATUS = 3;
		return;
	}
}


void CTicTacToeDlg::OnBnClickedConnect()
{
	//Сначала опишем необходимые локальные переменные и загрузим библиотеку Winsock :
	char		szServer[128];	// Имя или IP-адрес сервера
	int		iPort;			// Порт
	WSADATA	wsd;
	struct sockaddr_in 	server;
	struct hostent* host = NULL;
	char Str[256];

	GetDlgItem(IDC_SERVER)->
		GetWindowText(szServer, sizeof(szServer));
	GetDlgItem(IDC_PORT)->GetWindowText(Str, sizeof(Str));
	iPort = atoi(Str);
	if (iPort <= 0 || iPort >= 0x10000)
	{
		m_ListBox.AddString((LPTSTR)"Port number incorrect");
		return;
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		m_ListBox.AddString
		((LPTSTR)"Failed to load Winsock library!");
		return;
	}

	//Далее создадим сокет :
	m_sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//0
	if (m_sClient == INVALID_SOCKET)
	{
		sprintf_s(Str, sizeof(Str), "socket() failed: %d\n",
			WSAGetLastError());
		m_ListBox.AddString((LPTSTR)Str);
		return;
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(iPort);
	server.sin_addr.s_addr = inet_addr(szServer);

	//Если адрес сервера не представлен в форме "aaa.bbb.ccc.ddd", это имя узла.
	//Пробуем его разрешить, после этого вызываем функцию connect().
	//Наконец, перенастраиваем пользовательский интерфейс.
	if (server.sin_addr.s_addr == INADDR_NONE)
	{
		host = gethostbyname(szServer);
		if (host == NULL)
		{
			sprintf_s(Str, sizeof(Str),
				"Unable to resolve server: %s", szServer);
			m_ListBox.AddString((LPTSTR)Str);
			return;
		}
		CopyMemory(&server.sin_addr, host->h_addr_list[0],
			host->h_length);
	}

	if (connect(m_sClient, (struct sockaddr*)&server,
		sizeof(server)) == SOCKET_ERROR)
	{
		sprintf_s(Str, sizeof(Str), "connect() failed: %d",
			WSAGetLastError());
		m_ListBox.AddString((LPTSTR)Str);
		return;
	}
	SetConnected(true);
	sprintf_s(Str, sizeof(Str), "connect() success");
	m_ListBox.AddString((LPTSTR)Str);
	AfxBeginThread(ListenThread, NULL);
}

void CTicTacToeDlg::ComputeInformation(CPoint point)
{
	// Обработка принятых даннных(наверное)
	int i, g = 1;
	for (i = 1; i < 10; i++)
	{
		if (Pastuszak[i] == 1 || Pastuszak[i] == 2) g++;
	}

	// Show a message if g = 9 to tell the user it's game over.
	if (g >= 10) {
		MessageBox("Game Over", "Game Over.");
		//GAME_STATUS = 3; // Game Over.
		return;
	}

	PlotPlayer(point);
	CheckWhoWon();
}

UINT ListenThread(PVOID lpParam)
{
	// Прием данных
	char	szBuffer[DEFAULT_BUFFER];
	int		ret;
	char	Str[256];
	CListBox* pLB =
		(CListBox*)(CListBox::FromHandle(hWnd_LB));

	CTicTacToeDlg* pDlg = (CTicTacToeDlg*)(CTicTacToeDlg::FromHandle(hWnd_Dlg));
	while (true)
	{
		ret = recv(m_sClient, szBuffer, DEFAULT_BUFFER, 0);
		if (ret == 0)	// Корректное завершение
			continue;
		else if (ret == SOCKET_ERROR)
		{
			sprintf_s(Str, sizeof(Str), "recv() failed: %d", WSAGetLastError());
			pLB->AddString((LPTSTR)Str);
			return 1;
		}
		szBuffer[ret] = '\0';
		sprintf_s(Str, sizeof(Str), "RECV [%d bytes]: '%s'", ret, szBuffer);
		pLB->AddString((LPTSTR)Str);
		int i = 0;
		CString str = szBuffer;
		CString str1;
		CString str2;
		while (szBuffer[i] != ' ')
		{
			str1.Append((CString)szBuffer[i]);
			i++;
		}
		i++;
		while (szBuffer[i] != '\0')
		{
			str2.Append((CString)szBuffer[i]);
			i++;
		}
		CPoint point;
		point.x = atoi(str1);
		point.y = atoi(str2);
		mainDlg->ComputeInformation(point);
	}
}

