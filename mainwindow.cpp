/*
This file is part of LumberLog Copyright (C) 2017 Erik de Jong
LumberLog is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
LumberLog is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with LumberLog.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	socket = new QUdpSocket(this);
	connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

	settings = new QSettings();
	this->restoreGeometry(settings->value("geometry").toByteArray());
	this->restoreState(settings->value("windowState").toByteArray());

	socket->bind(QHostAddress::Any, 12345);
	ui->actionIgnore_linefeeds->setChecked(settings->value("linefeeds").toBool());
	ui->actionTimestamps->setChecked(settings->value("timestamps").toBool());
}

MainWindow::~MainWindow()
{
	settings->setValue("geometry", this->saveGeometry());
	settings->setValue("windowState", this->saveState());
	settings->setValue("linefeeds", ui->actionIgnore_linefeeds->isChecked());
	settings->setValue("timestamps", ui->actionTimestamps->isChecked());
	settings->sync();
	delete ui;
}


void MainWindow::readyRead()
{
	QByteArray buffer;
	buffer.resize(socket->pendingDatagramSize());
	QHostAddress sender;
	quint16 senderPort;
	socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
	QString line;
	if (ui->actionTimestamps->isChecked()) {
		line.append(QDateTime::currentDateTime().toString("dd/MM hh:mm:ss "));
	}
	line.append(QHostAddress(sender.toIPv4Address()).toString());
	line.append(" ");
	line.append(buffer);
	if (ui->actionIgnore_linefeeds->isChecked()) {
		line = line.replace("\r", "").replace("\n", "");
	}
	ui->pteLog->appendPlainText(line);
}

void MainWindow::on_actionClear_triggered()
{
	ui->pteLog->clear();
}
