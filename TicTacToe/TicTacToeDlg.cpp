
// TicTacToeDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "TicTacToe.h"
#include "TicTacToeDlg.h"
#include "afxdialogex.h"
#include <string>
#include <iostream>
#include <winsock2.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEFAULT_PORT	5150
#define DEFAULT_BUFFER	2048

HWND hWnd_LB;
HWND hWnd_Dlg;// Дискриптор окна

// Диалоговое окно CTicTacToeDlg
bool bPrint = false; // Выводить ли отладочные сообщения
bool m_IsConnected;
SOCKET m_sClient;
CTicTacToeDlg* mainDlg;

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
	DDX_Control(pDX, IDC_STATUS, m_status);
}

BEGIN_MESSAGE_MAP(CTicTacToeDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_CONNECT, &CTicTacToeDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, &CTicTacToeDlg::OnBnClickedDisconnect)
	ON_BN_CLICKED(IDC_PRINT, &CTicTacToeDlg::OnBnClickedPrint)
	ON_BN_CLICKED(IDC_REFRESH, &CTicTacToeDlg::OnBnClickedRefresh)
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

	sprintf_s(Str, sizeof(Str), "%d", DEFAULT_PORT);
	GetDlgItem(IDC_PORT)->SetWindowText(Str);

	SetConnected(false);
	hWnd_LB = m_ListBox.m_hWnd;   // Для ListenThread
	hWnd_Dlg = this->m_hWnd;     // Для ListenThread
	mainDlg = this;
	int i;
	for (i = 0; i < 11; i++) {
		stepsGrid[i] = 0;
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
		CPaintDC dc(this);
		CClientDC cdc(this);

		DrawGrid(&dc);// Отрисовка игрового поля

		CDialog::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CTicTacToeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTicTacToeDlg::DrawGrid(CPaintDC* pdc)
{
	CPen blackPen(PS_SOLID, 7, RGB(0, 0, 0));

	CRect Rect;
	GetClientRect(Rect);
	Rect.NormalizeRect();

	CPen* pOldPen= pdc->SelectObject(&blackPen);

	CPoint pStart;
	CPoint pFinish;

	// Горизонтальные
	pStart.y = 150;
	pStart.x = 50;
	pFinish.y = pStart.y;
	pFinish.x = 350;
	pdc->MoveTo(pStart);
	pdc->LineTo(pFinish);


	pStart.y = 250;
	pStart.x = 50;
	pFinish.y = pStart.y;
	pFinish.x = 350;
	pdc->MoveTo(pStart);
	pdc->LineTo(pFinish);


	// Вертикальные
	pStart.y = 50;
	pStart.x = 150;
	pFinish.y = 350;
	pFinish.x = pStart.x;
	pdc->MoveTo(pStart);
	pdc->LineTo(pFinish);


	pStart.y = 50;
	pStart.x = 250;
	pFinish.y = 350;
	pFinish.x = pStart.x;
	pdc->MoveTo(pStart);
	pdc->LineTo(pFinish);
}

void CTicTacToeDlg::DrawX(CClientDC* pdc, int x, int y)
{
	CPen XSize(PS_SOLID, 7, RGB(0, 0, 0));

	CRect Rect;
	GetClientRect(Rect);
	Rect.NormalizeRect();

	CPen* pOldPen;

	CPoint pStart;
	CPoint pEnd;

	pStart.y = y;
	pStart.x = x;
	pEnd.y = y + 50;
	pEnd.x = x + 50;

	pOldPen = pdc->SelectObject(&XSize);
	pdc->MoveTo(pStart);
	pdc->LineTo(pEnd);

	pStart.y = y;
	pStart.x = x + 50;
	pEnd.y = y + 50;
	pEnd.x = x;

	pOldPen = pdc->SelectObject(&XSize);
	pdc->MoveTo(pStart);
	pdc->LineTo(pEnd);

	pdc->SelectObject(pOldPen);
}

void CTicTacToeDlg::DrawO(CClientDC* pdc, int x, int y)
{
	CPen OSize(PS_SOLID, 6, RGB(0, 0, 0));
	CBrush lBlack(RGB(192, 192, 192));

	CPen* pOldPen;
	CBrush* pOldBrush;

	pOldPen = pdc->SelectObject(&OSize);
	pOldBrush = pdc->SelectObject(&lBlack);

	pdc->Ellipse(x, y, x + 75, y + 75);

	pdc->SelectObject(pOldPen);
	pdc->SelectObject(pOldBrush);
}

void CTicTacToeDlg::DrawLine(CClientDC* pdc, int x, int y, int x2, int y2)// Фиолетовая линия
{
	CPen line(PS_SOLID, 7, RGB(125, 0, 200));

	CRect Rect;
	GetClientRect(Rect);
	Rect.NormalizeRect();

	CPen* pOldPen;

	CPoint pStart;
	CPoint pFinish;

	pStart.y = y;
	pStart.x = x;
	pFinish.y = y2;
	pFinish.x = x2;

	pOldPen = pdc->SelectObject(&line);
	pdc->MoveTo(pStart);
	pdc->LineTo(pFinish);

	pdc->SelectObject(pOldPen);
}

void CTicTacToeDlg::PlotPlayer(CPoint point)
{
	if (GAME_STATUS == 3)
		return;
	m_Player++;

	CClientDC sdc(this);// device context for painting
	if (point.x > 47 && point.x < 143 && point.y > 47 && point.y < 143)
	{
		if (stepsGrid[1] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		if (m_Player % 2 != 0)
		{
			// Top - Left
			stepsGrid[1] = 1;
			posX[1] = 70;
			posY[1] = 70;
			DrawX(&sdc, posX[1], posY[1]);
			GridErr = 0;
			return;
		}
		else if (m_Player % 2 != 1) {
			stepsGrid[1] = 2;
			posX[1] = 65;
			posY[1] = 65;
			DrawO(&sdc, posX[1], posY[1]);
			GridErr = 0;
			return;
		}

	}
	else if (point.x > 152 && point.x < 249 && point.y > 47 && point.y < 143) {
		if (stepsGrid[2] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}	
		if (m_Player % 2 != 0)
		{
			// Top - Middle
			stepsGrid[2] = 1;
			posX[2] = 170;
			posY[2] = 70;
			DrawX(&sdc, posX[2], posY[2]);
			GridErr = 0;
			return;
		}
		else if (m_Player % 2 != 1) {
			stepsGrid[2] = 2;
			posX[2] = 165;
			posY[2] = 65;
			DrawO(&sdc, posX[2], posY[2]);
			GridErr = 0;
			return;
		}
	}
	else if (point.x > 253 && point.x < 350 && point.y > 47 && point.y < 143) {
		if (stepsGrid[3] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		if (m_Player % 2 != 0)
		{
			// Top - Right
			stepsGrid[3] = 1;
			posX[3] = 270;
			posY[3] = 70;
			DrawX(&sdc, posX[3], posY[3]);
			GridErr = 0;
			return;
		}
		else if (m_Player % 2 != 1) {
			stepsGrid[3] = 2;
			posX[3] = 265;
			posY[3] = 65;
			DrawO(&sdc, posX[3], posY[3]);
			GridErr = 0;
			return;
		}
	}
	else if (point.x > 47 && point.x < 143 && point.y > 152 && point.y < 245) {
		if (stepsGrid[4] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		if (m_Player % 2 != 0)
		{
			// Middle - left
			stepsGrid[4] = 1;
			posX[4] = 70;
			posY[4] = 180;
			GridErr = 0;
			DrawX(&sdc, posX[4], posY[4]);
			return;
		}
		else if (m_Player % 2 != 1) {
			stepsGrid[4] = 2;
			posX[4] = 65;
			posY[4] = 165;
			DrawO(&sdc, posX[4], posY[4]);
			GridErr = 0;
			return;
		}
	}
	else if (point.x > 152 && point.x < 249 && point.y > 152 && point.y < 245) {
		if (stepsGrid[5] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		
		if (m_Player % 2 != 0)
		{
			// Middle - Middle
			stepsGrid[5] = 1;
			posX[5] = 170;
			posY[5] = 180;
			DrawX(&sdc, posX[5], posY[5]);
			GridErr = 0;
			return;
		}
		else if (m_Player % 2 != 1) {
			stepsGrid[5] = 2;
			posX[5] = 165;
			posY[5] = 165;
			DrawO(&sdc, posX[5], posY[5]);
			GridErr = 0;
			return;
		}
	}
	else if (point.x > 253 && point.x < 350 && point.y > 152 && point.y < 245) {
		if (stepsGrid[6] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		
		if (m_Player % 2 != 0)
		{
			//Middle - Right
			stepsGrid[6] = 1;
			posX[6] = 270;
			posY[6] = 180;
			DrawX(&sdc, posX[6], posY[6]);
			GridErr = 0;
			return;
		}
		else if (m_Player % 2 != 1) {
			point.x = 265;
			point.y = 165;
			stepsGrid[6] = 2;
			posX[6] = 265;
			posY[6] = 165;
			DrawO(&sdc, posX[6], posY[6]);
			GridErr = 0;
			return;
		}

	}
	else if (point.x > 47 && point.x < 143 && point.y > 251 && point.y < 351) {
		if (stepsGrid[7] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		
		if (m_Player % 2 != 0)
		{
			// Bottom - left
			stepsGrid[7] = 1;
			posX[7] = 70;
			posY[7] = 280;
			DrawX(&sdc, posX[7], posY[7]);
			GridErr = 0;
			return;
		}
		else if (m_Player % 2 != 1) {
			point.x = 65;
			point.y = 265;
			stepsGrid[7] = 2;
			posX[7] = 65;
			posY[7] = 265;
			DrawO(&sdc, posX[7], posY[7]);
			GridErr = 0;
			return;
		}

	}
	else if (point.x > 152 && point.x < 249 && point.y > 251 && point.y < 351) {
		if (stepsGrid[8] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		
		if (m_Player % 2 != 0)
		{
			// Bottom - Middle
			stepsGrid[8] = 1;
			posX[8] = 170;
			posY[8] = 280;
			DrawX(&sdc, posX[8], posY[8]);
			GridErr = 0;
			return;
		}
		else if (m_Player % 2 != 1) {
			stepsGrid[8] = 2;
			posX[8] = 165;
			posY[8] = 265;
			DrawO(&sdc, posX[8], posY[8]);
			GridErr = 0;
			return;
		}
	}
	else if (point.x > 254 && point.x < 350 && point.y > 251 && point.y < 351) {
		if (stepsGrid[9] != 0)
		{
			m_Player--;
			MessageBox("This square is aready taken.");
			GridErr = 1;
			return;
		}
		
		if (m_Player % 2 != 0)
		{
			// Bottom - Right
			stepsGrid[9] = 1;
			posX[9] = 270;
			posY[9] = 280;
			DrawX(&sdc, posX[9], posY[9]);
			GridErr = 0;
			return;
		}
		else if (m_Player % 2 != 1) {
			stepsGrid[9] = 2;
			posX[9] = 265;
			posY[9] = 265;
			DrawO(&sdc, posX[9], posY[9]);
			GridErr = 0;
			return;
		}
	}
	else {
		GridErr = 2;
		m_Player--;
		return;
	}
}

void CTicTacToeDlg::SetConnected(bool IsConnected)
{
	m_IsConnected = IsConnected;
	GetDlgItem(IDC_SERVER)->EnableWindow(!IsConnected);
	GetDlgItem(IDC_PORT)->EnableWindow(!IsConnected);
	GetDlgItem(IDC_CONNECT)->EnableWindow(!IsConnected);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(IsConnected);
}

void CTicTacToeDlg::RefreshGame() {
	int i;
	for (i = 0; i < 11; i++) {
		stepsGrid[i] = 0;
		posX[i] = 0;
		posY[i] = 0;
	}
	m_Player = 0;
	GridErr = 0;
	GAME_STATUS = 1;
	this->UpdateWindow();
	this->RedrawWindow();
}

void CTicTacToeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CString status;
	m_status.GetWindowTextA(status);
	if (status == "Не подключен к серверу" || status == "Ход противника" || status == "Ожидание противника")
	{
		return;
	}
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
			sprintf_s(Str, sizeof(Str), "Отправка сообщения не удалась(ошибка: %d)",
				WSAGetLastError());
			m_ListBox.AddString((LPTSTR)Str);
		}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

int CTicTacToeDlg::CheckWhoWon()
{
	if (GAME_STATUS == 3)
		return 0;
	CClientDC pdc(this);
	int cell;// Первый из трех элементов для проверки победы

	cell = stepsGrid[1];
	if (stepsGrid[5] == cell && stepsGrid[9] == cell && cell != 0)
	{
		// Если кто-то победил
		DrawLine(&pdc, 50, 50, 350, 350);
		if (m_Player % 2 != 0)
		{
			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			MessageBox("Победили крестики");
		}
		if (m_Player % 2 == 0)
		{

			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			MessageBox("Победили нолики");
		}
		GAME_STATUS = 3;
		return 1;
	}

	cell = stepsGrid[3];
	if (stepsGrid[5] == cell && stepsGrid[7] == cell && cell != 0)
	{
		// Если кто-то победил
		DrawLine(&pdc, 350, 50, 50, 350);
		if (m_Player % 2 != 0)
		{

			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			MessageBox("Победили крестики");
		}
		if (m_Player % 2 == 0)
		{

			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			MessageBox("Победили нолики");
		}
		GAME_STATUS = 3;
		return 1;
	}

	cell = stepsGrid[1];
	if (stepsGrid[2] == cell && stepsGrid[3] == cell && cell != 0)
	{
		// Если кто-то победил
		DrawLine(&pdc, 50, 100, 350, 100);
		if (m_Player % 2 != 0)
		{

			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			MessageBox("Победили крестики");
		}
		if (m_Player % 2 == 0)
		{

			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			MessageBox("Победили нолики");
		}
		GAME_STATUS = 3;
		return 1;
	}

	cell = stepsGrid[3];
	if (stepsGrid[6] == cell && stepsGrid[9] == cell && cell != 0)
	{
		// Если кто-то победил
		DrawLine(&pdc, 300, 50, 300, 350);
		if (m_Player % 2 != 0)
		{

			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			MessageBox("Победили крестики");
		}
		if (m_Player % 2 == 0)
		{

			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			MessageBox("Победили нолики");
		}
		GAME_STATUS = 3;
		return 1;
	}

	cell = stepsGrid[7];
	if (stepsGrid[8] == cell && stepsGrid[9] == cell && cell != 0)
	{
		// Если кто-то победил
		DrawLine(&pdc, 50, 300, 350, 300);
		if (m_Player % 2 != 0)
		{
			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			MessageBox("Победили крестики");
		}
		if (m_Player % 2 == 0)
		{
			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			MessageBox("Победили нолики");
		}
		GAME_STATUS = 3;
		return 1;
	}

	cell = stepsGrid[1];
	if (stepsGrid[4] == cell && stepsGrid[7] == cell && cell != 0)
	{
		// Если кто-то победил
		DrawLine(&pdc, 100, 50, 100, 350);
		if (m_Player % 2 != 0)
		{
			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			MessageBox("Победили крестики");
		}
		if (m_Player % 2 == 0)
		{
			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			MessageBox("Победили нолики");
		}
		GAME_STATUS = 3;
		return 1;
	}

	cell = stepsGrid[2];
	if (stepsGrid[5] == cell && stepsGrid[8] == cell && cell != 0)
	{
		// Если кто-то победил
		DrawLine(&pdc, 200, 50, 200, 350);
		if (m_Player % 2 != 0)
		{
			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			MessageBox("Победили крестики");
		}
		if (m_Player % 2 == 0)
		{

			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			MessageBox("Победили нолики");
		}
		GAME_STATUS = 3;
		return 1;
	}

	cell = stepsGrid[4];
	if (stepsGrid[5] == cell && stepsGrid[6] == cell && cell != 0)
	{
		// Если кто-то победил
		DrawLine(&pdc, 50, 200, 350, 200);
		if (m_Player % 2 != 0)
		{
			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			MessageBox("Победили крестики");
		}
		if (m_Player % 2 == 0)
		{
			if (player)
				m_status.SetWindowTextA("Игра окончена. Вы проиграли.");
			else
				m_status.SetWindowTextA("Игра окончена. Вы одержали победу над противником.");
			MessageBox("Победили нолики");
		}
		GAME_STATUS = 3;
		return 1;
	}
	return 0;
}

void CTicTacToeDlg::OnBnClickedConnect()
{
	//Сначала опишем необходимые локальные переменные и загрузим библиотеку Winsock :
	char szServer[128];	// Имя или IP-адрес сервера
	int	iPort;// Порт
	WSADATA	wsd;
	struct sockaddr_in 	server;
	struct hostent* host = NULL;
	char Str[256];

	GetDlgItem(IDC_SERVER)-> GetWindowText(szServer, sizeof(szServer));
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

	//Далее создадим сокет:
	m_sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

	if (connect(m_sClient, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		if (WSAGetLastError())// Может тут неверно проверяю??
			sprintf_s(Str, sizeof(Str), "Не удалось установить соединение");
		m_ListBox.AddString((LPTSTR)Str);
		return;
	}

	char szBuffer[DEFAULT_BUFFER];
	int ret;
	ret = recv(m_sClient, szBuffer, DEFAULT_BUFFER, 0);
	if (ret == 0)
	{
		sprintf_s(Str, sizeof(Str), "Сервер переполнен.");
		m_ListBox.AddString((LPTSTR)Str);
		return;
	}
	else 
	{
		SetConnected(true);
		sprintf_s(Str, sizeof(Str), "Соединение установлено");
		m_ListBox.AddString((LPTSTR)Str);
		AfxBeginThread(ListenThread, NULL);
	}

}

void CTicTacToeDlg::ComputeInformation(CPoint point)
{
	// Обработка принятых даннных(координат мыши)
	if (GAME_STATUS == 3)
	{
		return;
	}

	PlotPlayer(point);
	if (GridErr)
	{
		return;
	}
	if (CheckWhoWon() != 1)
	{
		int i, j = 1;
		for (i = 1; i < 10; i++)
		{
			if (stepsGrid[i] == 1 || stepsGrid[i] == 2) j++;
		}

		if (j >= 10) {
			MessageBox("Игра окончена. Ничья.");
			m_status.SetWindowTextA("Игра окончена. Ничья.");
			GAME_STATUS = 3;
			return;
		}
	}
	if (GAME_STATUS != 3)
	{
		if (turn == 1)
		{
			turn = 0;
			m_status.SetWindowTextA("Ход противника");
		}
		else if (turn == 0)
		{
			turn = 1;
			m_status.SetWindowTextA("Ваш ход");
		}
	}
}

UINT ListenThread(PVOID lpParam)
{
	// Прием данных
	char szBuffer[DEFAULT_BUFFER];
	int	 ret;
	char Str[256];
	CListBox* pLB = (CListBox*)(CListBox::FromHandle(hWnd_LB));

	while (true)
	{
		ret = recv(m_sClient, szBuffer, DEFAULT_BUFFER, 0);
		if (ret == 0)
		{
			AfxEndThread(1, true);
			continue;
		}

		else if (ret == SOCKET_ERROR)
		{
			if (WSAGetLastError() == 10054)
				sprintf_s(Str, sizeof(Str), "Вы были отключены от сервера");
			else
				sprintf_s(Str, sizeof(Str), "recv() failed: %d", WSAGetLastError());

			pLB->AddString((LPTSTR)Str);
			mainDlg->RefreshGame();
			mainDlg->m_status.SetWindowTextA("Не подключен к серверу");
			mainDlg->GetDlgItem(IDC_REFRESH)->EnableWindow(FALSE);
			mainDlg->SetConnected(false);
			return 1;
		}
		szBuffer[ret] = '\0';
		sprintf_s(Str, sizeof(Str), "RECV [%d bytes]: '%s'", ret, szBuffer);
		if (bPrint)
			pLB->AddString((LPTSTR)Str);
		mainDlg->ScroolToLastItem();// Автоматическое пролистывание списка в конец
		if (ret == 1)
		{
			CString str1;
			str1.Append((CString)szBuffer[0]);
			if (str1 == "9")// Пришло сообщение о том, что подключился второй игрок
			{
				if (mainDlg->turn)
				{
					mainDlg->m_status.SetWindowTextA("Ваш ход");
					mainDlg->GetDlgItem(IDC_REFRESH)->EnableWindow(TRUE);
				}
			}
			else if (str1 == "8")// Пришло сообщение о том, что второй игрок отключился
			{
				mainDlg->MessageBox("Противник отключился, помянем");
				mainDlg->m_status.SetWindowTextA("Ожидание противника");
				mainDlg->GetDlgItem(IDC_REFRESH)->EnableWindow(FALSE);
				mainDlg->turn = 1;
				mainDlg->player = 1;
				mainDlg->RefreshGame();
			}
			else if (str1 == "7")// Пришло сообщение о том, что второй игрок начал игру сначала
			{
				mainDlg->NewGame();
			}
			else// Пришло сообщение о том, что подключение прошло успешно и значение очереди хода. Крестики ходят первыми(1)
			{
				mainDlg->player = atoi(str1);// Распределение ролей
				mainDlg->turn = atoi(str1);
				if (mainDlg->turn)
				{
					mainDlg->m_status.SetWindowTextA("Ожидание противника");
				}
				if (!mainDlg->turn)
				{
					mainDlg->m_status.SetWindowTextA("Ход противника");
					mainDlg->GetDlgItem(IDC_REFRESH)->EnableWindow(TRUE);
				}
			}
		}
		else// Пришло сообщение с координатами хода игрока
		{
			int i = 0;
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
}

void CTicTacToeDlg::OnBnClickedDisconnect()
{
	int iResult = shutdown(m_sClient, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("disconnect failed: %d\n", WSAGetLastError());
		closesocket(m_sClient);
		WSACleanup();
		return;
	}
	SetConnected(false);
	GetDlgItem(IDC_REFRESH)->EnableWindow(FALSE);
	RefreshGame();
	m_status.SetWindowTextA("Не подключен к серверу");
	m_ListBox.AddString("Соединение разорвано");
}

void CTicTacToeDlg::OnBnClickedPrint()
{
	if (((CButton*)GetDlgItem(IDC_PRINT))->GetCheck() == 1)
		bPrint = true;
	else
		bPrint = false;
}

void CTicTacToeDlg::ScroolToLastItem()
{
	int nNumItems = m_ListBox.GetCount();
	m_ListBox.SetCurSel(nNumItems - 1);
	m_ListBox.SetCurSel(-1);
}

void CTicTacToeDlg::NewGame() {
	if (player)// Если игрок играл за крестики, то он будет за нолики
	{
		m_status.SetWindowTextA("Ход противника");
		turn = 0;
		player = 0;
	}
	else// Если игрок играл за нолики, то он будет за крестики
	{
		m_status.SetWindowTextA("Ваш ход");
		turn = 1;
		player = 1;
	}
	RefreshGame();
}// Для кнопки "Начать сначала"

void CTicTacToeDlg::OnBnClickedRefresh()// Перезаупуск игры(Кнопка "начать сначала")
{
	int ret;
	char Str[256];
	char szMessage[1024];
	std::string string = "7";// Признак перезапуска игры
	const char* cstr = string.c_str();
	sprintf_s(szMessage, sizeof(szMessage), cstr);
	ret = send(m_sClient, szMessage, strlen(szMessage), 0);
	if (ret == 0)
		;
	else if (ret == SOCKET_ERROR)
	{
		sprintf_s(Str, sizeof(Str), "Отправка сообщения не удалась(ошибка: %d)",
			WSAGetLastError());
		m_ListBox.AddString((LPTSTR)Str);
	}
}
