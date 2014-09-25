#include "MainWindow.h"
#include <QtWidgets>
#include "SystemTrayIcon.h"
#include "Common.h"

MainWindow::MainWindow(QWidget *parent)
	: QDialog(parent), isStart_(false)
{
	this->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
	QLabel *fromLabel = new QLabel(tr("From:"), this);
	QLabel *toLabel = new QLabel(tr("To  :"), this);
	leFrom_ = new QLineEdit(this);
	leTo_ = new QLineEdit(this);
	pbFromBrowser_ = new QPushButton(tr("Browser"), this);
	pbToBrowser_ = new QPushButton(tr("Browser"), this);
	pbStart_ = new QPushButton(tr("Start"), this);
	tray_ = new SystemTrayIcon(this);

	leFrom_->setReadOnly(true);
	leTo_->setReadOnly(true);

	connect(pbFromBrowser_, SIGNAL(clicked()), this, SLOT(slotFromBrowser()));
	connect(pbToBrowser_, SIGNAL(clicked()), this, SLOT(slotToBrowser()));
	connect(pbStart_, SIGNAL(clicked()), this, SLOT(slotStart()));
	connect(tray_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));
	tray_->show();

	QHBoxLayout *fromLayout = new QHBoxLayout;
	fromLayout->addWidget(fromLabel);
	fromLayout->addWidget(leFrom_);
	fromLayout->addWidget(pbFromBrowser_);

	QHBoxLayout *toLayout = new QHBoxLayout;
	toLayout->addWidget(toLabel);
	toLayout->addWidget(leTo_);
	toLayout->addWidget(pbToBrowser_);

	QHBoxLayout *startLayout = new QHBoxLayout;
	startLayout->addWidget(pbStart_);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(fromLayout);
	mainLayout->addLayout(toLayout);
	mainLayout->addLayout(startLayout);

	this->setLayout(mainLayout);

	watcher_ = new QFileSystemWatcher(this);
}


MainWindow::~MainWindow(void)
{
}

QString MainWindow::oppositeSide(QString path, bool isFrom)
{
	QString fromDir = leFrom_->text();
	QString toDir = leTo_->text();
	if (fromDir.isEmpty() || toDir.isEmpty() || path.isEmpty()) {
		return "";
	}

	if (isFrom) {
		return path.replace(fromDir, toDir);
	} else {
		return path.replace(toDir, fromDir);
	}
}

bool MainWindow::isOppositeSideExist(QString path, bool isFrom)
{
	QFileInfo info(oppositeSide(path, isFrom));
	return info.exists();
}

void MainWindow::slotFromBrowser()
{
	QString curDir = currentDirFromDlg();
	leFrom_->setText(curDir);

	if (!watcher_->files().empty()) {
		watcher_->removePaths(watcher_->files());
	}

	if (!watcher_->directories().empty()) {
		watcher_->removePaths(watcher_->directories());
	}
	QStringList files, subdirs;
	Common::foreachDir(leFrom_->text(), files, subdirs, true);

	watcher_->addPaths(files);
	watcher_->addPath(leFrom_->text());
	watcher_->addPaths(subdirs);
}

void MainWindow::slotToBrowser()
{
	QString curDir = currentDirFromDlg();
	leTo_->setText(curDir);
	start(false);
}

void MainWindow::slotStart()
{
	if (leFrom_->text().isEmpty() || leTo_->text().isEmpty()) {
		return;
	}

	start(!isStart_);
}

QString MainWindow::currentDirFromDlg()
{
	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::Directory);
	if (dlg.exec()) {
		return dlg.directory().path();
	}
	return "";
}

void MainWindow::start(bool isTrue)
{
	if (isTrue) {
		isStart_ = true;
		pbStart_->setText(tr("Stop"));
		connect(watcher_, SIGNAL(fileChanged(QString)), this, SLOT(slotFileChanged(QString)));
		connect(watcher_, SIGNAL(directoryChanged(QString)), this, SLOT(slotDirectoryChanged(QString)));
		Common::asyncCopy(leFrom_->text(), leTo_->text());
	} else {
		isStart_ = false;
		pbStart_->setText(tr("Start"));
		disconnect(watcher_, SIGNAL(fileChanged(QString)), this, SLOT(slotFileChanged(QString)));
		disconnect(watcher_, SIGNAL(directoryChanged(QString)), this, SLOT(slotDirectoryChanged(QString)));
	}
}

void MainWindow::slotFileChanged(const QString &path)
{
	QFileInfo info(path);
	QString dstDir;
	if (info.isFile()) {
		dstDir = oppositeSide(info.absolutePath());
	} else if (info.isDir()) {
		dstDir = oppositeSide(path);
	} else {
		return;
	}

	Common::asyncCopy(path, dstDir);
}

void MainWindow::slotDirectoryChanged(const QString &dir)
{
	QStringList files, subdirs;
	Common::foreachDir(dir, files, subdirs);

	foreach (const QString &file, files) {
		if (!watcher_->files().contains(file)) {
			slotFileChanged(file);
		}
	}

	foreach (const QString &subdir, subdirs) {
		if (!watcher_->directories().contains(subdir)) {
			slotFileChanged(subdir);
		}
	}

	QStringList dstFiles, dstSubdis;
	Common::foreachDir(oppositeSide(dir), dstFiles, dstSubdis);
	foreach (const QString &file, dstFiles) {
		if (!isOppositeSideExist(file, false)) {
			Common::deleteFile(file);
		}
	}


	foreach (const QString &subdir, dstSubdis) {
		if (!isOppositeSideExist(subdir, false)) {
			Common::deleteDir(subdir);
		}
	}
}