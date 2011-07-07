// HamFax -- an application for sending and receiving amateur radio facsimiles
// Copyright (C) 2001,2002
// Christof Schmitt, DH1CS <cschmitt@users.sourceforge.net>
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

#ifndef SOUND_HPP
#define SOUND_HPP

/**
 * \file
 * Soundcard interface definition.
 */

#include <qobject.h>
#include <qstring.h>
#include <qthread.h>
#include <qsocketnotifier.h>
#include "PTT.hpp"
#include "config.h"

#ifdef USE_ALSA
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
#endif

class Sound : public QObject {
	Q_OBJECT
public:
	Sound(QObject* parent);
	~Sound(void);
	int startOutput(void);
	int startInput(void);
private:
	int sampleRate;
	int use_alsa;
#ifdef USE_ALSA
	snd_pcm_t *pcm;
	snd_async_handler_t *handler;
	snd_pcm_uframes_t frames;
	static void ALSA_read_callback(snd_async_handler_t *handler);
	static void ALSA_write_callback(snd_async_handler_t *handler);
	bool readALSAdata();
	int		  framesize;
	short		  *buffer;
#endif
#ifdef QT_THREAD_SUPPORT
	QThread   *xfer_thread;
#else
	int	  callbackSocket[2];
#endif
	int dsp;
	QSocketNotifier* notifier;
	PTT ptt;
signals:
        void data(short*, int);
	void deviceClosed(void);
	void spaceLeft(int);
public slots:
	void closeNow(void);
	void end(void);
	void write(short* samples, int number);
private slots:
	void checkSpace(int fd);
        void read(int fd);
	// readALSA should be ifdef'ed, but this does not work with moc
        void readALSA(int fd);
	void close(void);
};

#endif
