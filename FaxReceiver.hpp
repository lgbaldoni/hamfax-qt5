// HamFax -- an application for sending and receiving amateur radio facsimiles
// Copyright (C) 2001 Christof Schmitt, DH1CS <cschmit@suse.de>
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

#ifndef FAXRECEIVER_HPP
#define FAXRECEIVER_HPP

#include <qobject.h>
#include <qstring.h>
#include <qarray.h>
#include <qtimer.h>

class FaxReceiver : public QObject {
	Q_OBJECT
public:
	FaxReceiver(QObject* parent);
	void setSampleRate(unsigned int rate);
	void init(void);
private:
	void decodeApt(unsigned int& x);
	void decodePhasing(unsigned int& x);
	void decodeImage(unsigned int& x);
	enum { APTSTART, PHASING, IMAGE, DONE } state;
	unsigned int sampleRate;
	unsigned int currentValue;
	bool aptHigh;
	unsigned int aptTrans;    // APT low-high transitions
	unsigned int aptCount;    // samples counted for aptTrans
	unsigned int aptStartFreq;
	unsigned int aptStopFreq;
	bool aptStop;
	bool phaseHigh;
	unsigned int currPhaseLength;
	unsigned int currPhaseHigh;
	unsigned int phaseLines;
	unsigned int noPhaseLines;
	bool phaseInvers;
	double lpm;
	double lpmSum;
	unsigned int width;
	int imageSample;
	unsigned int lastCol;
	unsigned int lastRow;
	unsigned int currRow;
	unsigned int pixel;
	unsigned int pixelSamples;
	bool color;
	QTimer* timer;
	QArray<unsigned char> rawData;
	QArray<unsigned char>::Iterator rawIt;
signals:
	void aptFound(unsigned int f);
	void aptStopDetected(void);
	void newImageHeight(unsigned int y, unsigned int h);
	void newPixel(unsigned int c, unsigned int h, 
		      unsigned int v, unsigned int rgbg);
	void start(void);
	void end(void);
	void startingPhasing(void);
	void phasingLine(double lpm);
	void imageRow(unsigned int row);
public slots:
	void decode(unsigned int*, unsigned int n);
        void setAptStartFreq(int f);
	void setAptStopFreq(int f);
	void setWidth(unsigned int width);
	void setPhasePol(bool pol);
	void startPhasing(void);
	void endReception(void);
	void setColor(bool b);
	void widthAdjust(double d);
private slots:
        void adjustNext(void);
};

#endif
