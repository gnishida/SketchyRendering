#include "MainWindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));

	glWidget = new GLWidget3D();
	setCentralWidget(glWidget);
}

MainWindow::~MainWindow() {
}

void MainWindow::onOpen() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open OBJ file..."), "", tr("CGA Files (*.obj)"));
	if (filename.isEmpty()) return;

	glWidget->loadOBJ(filename.toUtf8().data());
}
