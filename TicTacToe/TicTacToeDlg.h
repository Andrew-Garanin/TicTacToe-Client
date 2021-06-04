
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
	int turn;
	int player;
	// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TICTACTOE_DIALOG };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	


	void DrawGrid(CPaintDC* pdc);
	void DrawX(CClientDC* pdc, int x, int y);
	void DrawO(CClientDC* pdc, int x, int y);
	void PlotPlayer(CPoint point);
	void DrawLine(CClientDC* pdc, int x, int y, int x2, int y2);
	int posY[10];
	int posX[10];
	int	m_Player;
	int stepsGrid[10];
	int GridErr;
	int GAME_STATUS;

	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void SetConnected(bool IsConnected);
	void NewGame();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	int CheckWhoWon();
	void RefreshGame();
	CListBox m_ListBox;
	afx_msg void OnBnClickedConnect();
	CStatic m_status;
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnBnClickedPrint();
	void ScroolToLastItem();
	afx_msg void OnBnClickedRefresh();
};
