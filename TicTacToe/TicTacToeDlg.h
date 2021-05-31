
// TicTacToeDlg.h: файл заголовка
//

#pragma once


// Диалоговое окно CTicTacToeDlg
class CTicTacToeDlg : public CDialog
{
// Создание
public:
	CTicTacToeDlg(CWnd* pParent = nullptr);	// стандартный конструктор
void ComputeInformation(CPoint point);
// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TICTACTOE_DIALOG };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV

	
// Реализация
protected:
	void SetConnected(bool IsConnected);
	

	void DrawGrid(CPaintDC* pdc);
	void DrawX(CClientDC* pdc, int x, int y);
	void DrawO(CClientDC* pdc, int x, int y);
	void PlotPlayer(CPoint point);
	void DrawLine(CClientDC* pdc, int x, int y, int x2, int y2);
	int posY[10];
	int posX[10];
	int	m_Player=-1;
	int Pastuszak[10];
	int GridErr;
	int GAME_STATUS;
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void CheckWhoWon();
	CListBox m_ListBox;
	afx_msg void OnBnClickedConnect();
};
