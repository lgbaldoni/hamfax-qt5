// HamFax -- an application for sending and receiving amateur radio facsimiles
// Copyright (C) 2001 Christof Schmittt, DH1CS <cschmit@suse.de>
//  
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "FaxWindow.hpp"
#include <qpopupmenu.h>
#include <qfiledialog.h>
#include <qstring.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include "Error.hpp"
#include "FaxView.hpp"
#include "OptionsDialog.hpp"
#include "ScaleDialog.hpp"

FaxWindow::FaxWindow(const QString& version)
	: version(version)
{
	config=new Config(this);
	faxImage=new FaxImage(this);
	FaxView* faxView=new FaxView(this,faxImage);
	faxControl=new FaxControl(this);
	faxTransmitter=new FaxTransmitter(this,faxImage);
	faxReceiver=new FaxReceiver(this);
	sound=new Sound(this);
	file=new File(this);
	ptt=new PTT(this);
	ptc=new PTC(this);
	faxModulator=new FaxModulator(this);
	faxDemodulator=new FaxDemodulator(this);
	timer=new QTimer(this);
	transmitDialog=new TransmitDialog(this);
	receiveDialog=new ReceiveDialog(this);

	connect(config,SIGNAL(carrier(unsigned int)),
		faxControl,SLOT(setCarrier(unsigned int)));
	connect(faxControl,SIGNAL(newCarrier(unsigned int)),
		config,SLOT(setCarrier(unsigned int)));
	connect(config,SIGNAL(carrier(unsigned int)),
		faxModulator,SLOT(setCarrier(unsigned int)));
	connect(config,SIGNAL(carrier(unsigned int)),
		faxDemodulator,SLOT(setCarrier(unsigned int)));

	connect(config,SIGNAL(lpm(unsigned int)),
		faxControl,SLOT(setLPM(unsigned int)));
	connect(faxControl,SIGNAL(newLPM(unsigned int)),
		config,SLOT(setLpm(unsigned int)));
	connect(config,SIGNAL(lpm(unsigned int)),
		faxControl,SLOT(setLPM(unsigned int)));

	connect(config,SIGNAL(aptStartFreq(unsigned int)),
		faxControl,SLOT(setAptStartFreq(unsigned int)));
	connect(faxControl,SIGNAL(newAptStartFreq(unsigned int)),
		config,SLOT(setAptStartFreq(unsigned int)));
	connect(config,SIGNAL(aptStartFreq(unsigned int)),
		faxTransmitter,SLOT(setAptStartFreq(unsigned int)));
	connect(config,SIGNAL(aptStartFreq(unsigned int)),
		faxReceiver,SLOT(setAptStartFreq(unsigned int)));

	connect(config,SIGNAL(aptStartLength(unsigned int)),
		faxControl,SLOT(setAptStartLength(unsigned int)));
	connect(faxControl,SIGNAL(newAptStartLength(unsigned int)),
		config,SLOT(setAptStartLength(unsigned int)));
	connect(config,SIGNAL(aptStartLength(unsigned int)),
		faxTransmitter,SLOT(setAptStartLength(unsigned int)));

	connect(config,SIGNAL(aptStopFreq(unsigned int)),
		faxControl,SLOT(setAptStopFreq(unsigned int)));
	connect(faxControl,SIGNAL(newAptStopFreq(unsigned int)),
		config,SLOT(setAptStopFreq(unsigned int)));
	connect(config,SIGNAL(aptStopFreq(unsigned int)),
		faxTransmitter,SLOT(setAptStopFreq(unsigned int)));
	connect(config,SIGNAL(aptStopFreq(unsigned int)),
		faxReceiver,SLOT(setAptStopFreq(unsigned int)));

	connect(config,SIGNAL(aptStopLength(unsigned int)),
		faxControl,SLOT(setAptStopLength(unsigned int)));
	connect(faxControl,SIGNAL(newAptStopLength(unsigned int)),
		config,SLOT(setAptStopLength(unsigned int)));
	connect(config,SIGNAL(aptStopLength(unsigned int)),
		faxTransmitter,SLOT(setAptStopLength(unsigned int)));

	connect(config,SIGNAL(phaseLines(unsigned int)),
		faxControl,SLOT(setPhasingLines(unsigned int)));
	connect(faxControl,SIGNAL(newPhasingLength(unsigned int)),
		config,SLOT(setPhaseLines(unsigned int)));
	connect(config,SIGNAL(phaseLines(unsigned int)),
		faxTransmitter,SLOT(setPhasingLines(unsigned int)));

	connect(config,SIGNAL(phaseInvert(bool)),
		faxControl,SLOT(setPhasingPol(bool)));
	connect(faxControl,SIGNAL(newPhasingPol(bool)),
		config,SLOT(setPhaseInvert(bool)));
	connect(config,SIGNAL(phaseInvert(bool)),
		faxTransmitter,SLOT(setPhasePol(bool)));
	connect(config,SIGNAL(phaseInvert(bool)),
		faxReceiver,SLOT(setPhasePol(bool)));

	connect(config,SIGNAL(deviation(unsigned int)),
		faxControl,SLOT(setDeviation(unsigned int)));
	connect(faxControl,SIGNAL(newDeviation(unsigned int)),
		config,SLOT(setDeviation(unsigned int)));
	connect(config,SIGNAL(deviation(unsigned int)),
		faxModulator,SLOT(setDeviation(unsigned int)));
	connect(config,SIGNAL(deviation(unsigned int)),
		faxDemodulator,SLOT(setDeviation(unsigned int)));
	connect(config,SIGNAL(deviation(unsigned int)),
		ptc,SLOT(setDeviation(unsigned int)));

	connect(config,SIGNAL(useFM(bool)),
		faxControl,SLOT(setModulation(bool)));
	connect(faxControl,SIGNAL(newModulation(bool)),
		config,SLOT(setUseFM(bool)));
	connect(config,SIGNAL(useFM(bool)),
		faxModulator,SLOT(setFM(bool)));
	connect(config,SIGNAL(useFM(bool)),
		faxDemodulator,SLOT(setFM(bool)));
	connect(config,SIGNAL(useFM(bool)),
		ptc,SLOT(setFM(bool)));

	// FaxWindow -- FaxImage -- FaxView
	connect(this,SIGNAL(loadFile(QString)),faxImage,SLOT(load(QString)));
	connect(this,SIGNAL(saveFile(QString)),faxImage,SLOT(save(QString)));
	connect(faxImage,SIGNAL(sizeUpdated(unsigned int, unsigned int)),
		faxView,SLOT(updateView(unsigned int, unsigned int)));
	connect(faxImage,SIGNAL(sizeUpdated(unsigned int, unsigned int)),
		faxControl,SLOT(setImageSize(unsigned int, unsigned int)));
	connect(faxImage,SIGNAL(widthUpdated(unsigned int)),
		faxReceiver,SLOT(setWidth(unsigned int)));
	connect(faxImage,SIGNAL(contentUpdated(int,int,int,int)),
		faxView,SLOT(update(int,int,int,int)));
	connect(faxImage,SIGNAL(scrollTo(int,int)),
		faxView,SLOT(ensureVisible(int,int)));
	faxImage->create(904,1);

	// FaxReceiver -- FaxImage
	connect(faxReceiver,
		SIGNAL(newImageHeight(unsigned int, unsigned int)),
		faxImage,SLOT(resizeHeight(unsigned int, unsigned int)));
	connect(faxReceiver,
		SIGNAL(newGrayPixel(unsigned int,unsigned int,unsigned int)),
		faxImage,
		SLOT(setPixelGray(unsigned int,unsigned int,unsigned int)));

	// FaxTransmitter -- TransmitDialog
	connect(faxTransmitter,SIGNAL(aptStart()),
		transmitDialog,SLOT(aptStart()));
	connect(faxTransmitter,SIGNAL(phasing()),
		transmitDialog,SLOT(phasing()));
	connect(faxTransmitter,SIGNAL(imageLine(unsigned int)),
		transmitDialog,SLOT(imageLine(unsigned int)));
	connect(faxTransmitter,SIGNAL(aptStop()),
		transmitDialog,SLOT(aptStop()));
	connect(transmitDialog,SIGNAL(cancelClicked()),
		this,SLOT(endTransmission()));

	// FaxReceiver -- ReceiveDialog
	connect(faxReceiver,SIGNAL(aptFound(unsigned int)),
		receiveDialog,SLOT(apt(unsigned int)));
	connect(faxReceiver,SIGNAL(searchingAptStart()),
		receiveDialog,SLOT(aptStart()));
	connect(faxReceiver,SIGNAL(startingPhasing()),
		receiveDialog,SLOT(phasing()));
	connect(faxReceiver,SIGNAL(phasingLine(double)),
		receiveDialog,SLOT(phasingLine(double)));
	connect(receiveDialog,SIGNAL(skipClicked()),
		faxReceiver,SLOT(startPhasing()));
	connect(faxReceiver,SIGNAL(imageRow(unsigned int)),
		receiveDialog,SLOT(imageRow(unsigned int)));
	connect(receiveDialog,SIGNAL(cancelClicked()),
		faxReceiver,SLOT(endReception()));
	connect(faxReceiver,SIGNAL(receptionEnded()),
		this,SLOT(endReception()));

	config->readFile();
	buildMenuBar();
	QHBoxLayout* layout=new QHBoxLayout(this);
	layout->setMenuBar(menuBar);
	layout->addWidget(faxControl);
	layout->addWidget(faxView);
	resize(650,0);
}

void FaxWindow::buildMenuBar(void)
{
	QPopupMenu* fileMenu=new QPopupMenu(this);
	fileMenu->insertItem(tr("&Open"),this,SLOT(load()));
	fileMenu->insertItem(tr("&Save"),this,SLOT(save()));
	fileMenu->insertItem(tr("&Quick save"),this,SLOT(quickSave()));
	fileMenu->insertSeparator();
	fileMenu->insertItem(tr("&Exit"),this,SLOT(close()));

	QPopupMenu* transmitMenu=new QPopupMenu(this);
	transmitMenu->insertItem(tr("Transmit using &dsp"),DSP);
	transmitMenu->insertItem(tr("Transmit to &file"),FILE);
	transmitMenu->insertItem(tr("Transmit using &PTC"),SCSPTC);

	QPopupMenu* receiveMenu=new QPopupMenu(this);
	receiveMenu->insertItem(tr("Receive from d&sp"),DSP);
	receiveMenu->insertItem(tr("Receive from f&ile"),FILE);
	receiveMenu->insertItem(tr("Receive from P&TC"),SCSPTC);

	QPopupMenu* imageMenu=new QPopupMenu(this);
	imageMenu->insertItem(tr("&Scale image / adjust IOC"),
			      this,SLOT(doScaleDialog()));
	imageMenu->insertItem(tr("Scale image to IOC &288"),SC288);
	imageMenu->insertItem(tr("Scale image to IOC &576"),SC576);
	imageMenu->insertSeparator();
	imageMenu->insertItem(tr("correct IOC from 576 to 288"),R576288);
	imageMenu->insertItem(tr("correct IOC from 288 to 576"),R288576);

	optionsMenu=new QPopupMenu(this);
	optionsMenu->insertItem(tr("device settings"),
				this,SLOT(doOptionsDialog()));
	optionsMenu->insertSeparator();
	pttID=optionsMenu->
		insertItem(tr("key PTT while transmitting with DSP"),
			   this,SLOT(changePTT()));
	optionsMenu->setItemChecked(pttID,config->getKeyPTT());

	QPopupMenu* helpMenu=new QPopupMenu(this);
	helpMenu->insertItem(tr("&About"),this,SLOT(about()));
	helpMenu->insertItem(tr("About &QT"),this,SLOT(aboutQT()));

	menuBar=new QMenuBar(this);
	menuBar->insertItem(tr("&File"),fileMenu);
	menuBar->insertItem(tr("&Transmit"),transmitMenu);
	menuBar->insertItem(tr("&Receive"),receiveMenu);
	menuBar->insertItem(tr("&Image"),imageMenu);
	menuBar->insertItem(tr("&Options"),optionsMenu);
	menuBar->insertSeparator();
	menuBar->insertItem(tr("&Help"),helpMenu);
	menuBar->setFrameStyle(QFrame::Panel|QFrame::Raised);

	connect(transmitMenu,SIGNAL(activated(int)),
		this,SLOT(initTransmit(int)));
	connect(receiveMenu,SIGNAL(activated(int)),
		this,SLOT(initReception(int)));
	connect(imageMenu,SIGNAL(activated(int)),
		this,SLOT(scaleImage(int)));
}

void FaxWindow::about(void)
{
	QMessageBox::information(this,this->caption(),
				 tr("HamFax is a QT application for "
				    "transmitting and receiving "
				    "ham radio facsimiles.\n"
				    "Author: Christof Schmitt, DH1CS\n"
				    "License: GPL\n"
				    "Version: %1").arg(version));
}

void FaxWindow::aboutQT(void)
{
	QMessageBox::aboutQt(this,this->caption());
}

void FaxWindow::load(void)
{
	QString fileName=getFileName(tr("load image"),"*.png");
	if(!fileName.isEmpty()) {
		emit loadFile(fileName);
	}
}

void FaxWindow::save(void)
{
	QString fileName=getFileName(tr("save image"),"*.png");
	if(!fileName.isEmpty()) {
		emit saveFile(fileName);
	}
}

void FaxWindow::doOptionsDialog(void)
{
	OptionsDialog* d=new OptionsDialog(this);
	d->devDSPName=config->getDSPDevice();
	d->devPTTName=config->getPTTDevice();
	d->devPTCName=config->getPTCDevice();
	d->init();
	if(d->exec()) {
		config->setDSP(d->devDSPName);
		config->setPTT(d->devPTTName);
		config->setPTC(d->devPTCName);
	}
	delete d;
}

void FaxWindow::doScaleDialog(void)
{
	ScaleDialog* d=new ScaleDialog(this);
	d->width=faxImage->getCols();
	d->height=faxImage->getRows();
	d->init();
	if(d->exec()) {
		faxImage->scale(d->width,d->height);
	}
	delete d;
}

void FaxWindow::changePTT(void)
{
	if(optionsMenu->isItemChecked(pttID)) {
		optionsMenu->setItemChecked(pttID,false);
		config->setKeyPTT(false);
	} else {
		optionsMenu->setItemChecked(pttID,true);
		config->setKeyPTT(true);
	}
}

QString FaxWindow::getFileName(QString caption, QString filter)
{
	QFileDialog* fd=new QFileDialog(this,0,true);
	fd->setSizeGripEnabled(false);
	fd->setCaption(caption);
	fd->setFilter(filter);
	fd->exec();
	QString s=fd->selectedFile();
	delete fd;
	return s;
}

void FaxWindow::initTransmit(int item)
{
	try {
		unsigned int sampleRate=0;
		QString fileName;
		switch(interface=item) {
		case FILE:
			sampleRate=8000;
			fileName=getFileName(tr("output file name"),"*.au");
			if(fileName.isEmpty()) {
				return;
			}
			file->openOutput(fileName,sampleRate);
			break;
		case DSP:
			sampleRate=8000;
			sound->openOutput(sampleRate);
			ptt->openDevice();
			ptt->set(true);
			break;
		case SCSPTC:
			sampleRate=5760;
			ptc->open();
			break;
		}
		faxModulator->setSampleRate(sampleRate);
		faxTransmitter->setSampleRate(sampleRate);
		faxTransmitter->startTransmission();
		menuBar->setDisabled(true);
		faxControl->setDisabled(true);
		timer->start(0);
		connect(timer,SIGNAL(timeout()),this,SLOT(transmitNext()));
		transmitDialog->show();

	} catch (Error e) {
		QMessageBox::warning(this,tr("error"),e.getText());
	}
}

void FaxWindow::transmitNext(void)
{
	try {
		unsigned int n=512;
		signed short sample[n];
		double buffer[n];
		faxTransmitter->getValues(buffer,n);
		if(n>0) {
			switch(interface) {
			case SCSPTC:
				ptc->transmit(buffer,n);
				break;
			case DSP:
				faxModulator->modulate(sample,buffer,n);
				sound->write(sample,n);
				break;
			case FILE:
				faxModulator->modulate(sample,buffer,n);
				file->write(sample,n);
				break;
			}
		} else {
			if(interface!=DSP || sound->outputBufferEmpty()) {
				endTransmission();
			}
		}
	} catch(Error e) {
		endTransmission();
		QMessageBox::warning(this,tr("error"),e.getText());
	}
}

void FaxWindow::endTransmission(void)
{
	timer->stop();
	disconnect(timer,SIGNAL(timeout()),this,SLOT(transmitNext()));
	switch(interface) {
	case FILE:
		file->close();
		break;
	case DSP:
		ptt->set(false);
		sound->close();
		ptt->closeDevice();
		break;
	case SCSPTC:
		ptc->close();
	}
	menuBar->setDisabled(false);
	faxControl->setDisabled(false);
	transmitDialog->hide();
}

void FaxWindow::initReception(int item)
{
	try {
		unsigned int sampleRate;
		QString fileName;
		switch(interface=item) {
		case FILE:
			fileName=getFileName(tr("input file name"),"*.au");
			if(fileName.isEmpty()) {
				return;
			}
			file->openInput(fileName,sampleRate);
			break;
		case DSP:
			sampleRate=8000;
			sound->openInput(sampleRate);
			break;
		case SCSPTC:
			sampleRate=5760;
			ptc->open();
			break;
		}
		faxDemodulator->setSampleRate(sampleRate);
		faxReceiver->setSampleRate(sampleRate);
		faxReceiver->init();
		menuBar->setDisabled(true);
		faxControl->setDisabled(true);
		timer->start(0);
		connect(timer,SIGNAL(timeout()),
			this,SLOT(receiveNext()));
		receiveDialog->show();
	} catch(Error e) {
		QMessageBox::warning(this,tr("error"),e.getText());
	}
}

void FaxWindow::receiveNext(void)
{
	unsigned int n=256;
	signed short samples[n];
	unsigned int buffer[n];
	switch(interface) {
	case FILE:
		file->read(samples,n);
		faxDemodulator->demodulate(buffer,samples,n);
		if(n==0) {
			endReception();
		}
		break;
	case DSP:
		sound->read(samples,n);
		faxDemodulator->demodulate(buffer,samples,n);
		break;
	case SCSPTC:
		ptc->receive(buffer,n);
		break;
	}
	faxReceiver->decode(buffer,n);
}

void FaxWindow::endReception(void)
{
	timer->stop();
	disconnect(timer,SIGNAL(timeout()),this,SLOT(receiveNext()));
	switch(interface) {
	case FILE:
		file->close();
		break;
	case DSP:
		sound->close();
		break;
	case SCSPTC:
		ptc->close();
		break;
	}
	menuBar->setDisabled(false);
	faxControl->setDisabled(false);
	receiveDialog->hide();
}

void FaxWindow::closeEvent(QCloseEvent* close)
{
	switch(QMessageBox::information(this,this->caption(),
					tr("Really exit?"),
					tr("&Exit"),tr("&Don't Exit"))) {
	case 0:
		timer->stop();
		close->accept();
		config->writeFile();
		break;
	case 1:
		break;
	}
}

void FaxWindow::scaleImage(int item)
{
	switch(item)
	{
	case SC288:
		faxImage->scaleToIOC(288);
		break;
	case SC576:
		faxImage->scaleToIOC(576);
		break;
	case R576288:
		faxImage->scale(faxImage->getCols()/2,faxImage->getRows());
		break;
	case R288576:
		faxImage->scale(faxImage->getCols()*2,faxImage->getRows());
		break;
	default:
		break;
	}
}

void FaxWindow::quickSave(void)
{
	QDateTime dt=QDateTime::currentDateTime();
	QDate date=dt.date();
	QTime time=dt.time();
	emit saveFile(QString().
		      sprintf("%04d-%02d-%02d-%02d-%02d-%02d.png",
			      date.year(),date.month(),date.day(),
			      time.hour(),time.minute(),time.second()));
}
