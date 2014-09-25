#pragma once

#include <QDialog>

class QLineEdit;
class QPushButton;
class SystemTrayIcon;
class QFileSystemWatcher;

class MainWindow : public QDialog
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow(void);

	QString oppositeSide(QString path, bool isFrom = true);
	bool isOppositeSideExist(QString path, bool isFrom = true);

private slots:
	void slotFromBrowser();
	void slotToBrowser();
	void slotStart();
	void slotFileChanged(const QString &path);
	void slotDirectoryChanged(const QString &dir);

private:
	QString currentDirFromDlg();
	void start(bool isTrue);

private:
	QLineEdit *leFrom_;
	QLineEdit *leTo_;
	QPushButton *pbFromBrowser_;
	QPushButton *pbToBrowser_;
	QPushButton *pbStart_;
	bool isStart_;
	SystemTrayIcon *tray_;

	QFileSystemWatcher *watcher_;
};

