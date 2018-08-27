#include "AudioProcessor.h"
#include <QAudioFormat>
#include <algorithm>
#include <iostream>

void AudioProcessor::InitializeAudio()
{
	m_format.setChannelCount(1);
	m_format.setSampleRate(8000);
	m_format.setSampleSize(16);
	m_format.setCodec("audio/pcm");
	m_format.setByteOrder(QAudioFormat::LittleEndian);
	m_format.setSampleType(QAudioFormat::UnSignedInt);
	
	QAudioDeviceInfo infoIn(QAudioDeviceInfo::defaultInputDevice());
	if (!infoIn.isFormatSupported(m_format))
	{
		//Default format not supported - trying to use nearest
		m_format = infoIn.nearestFormat(m_format);
	}

	QAudioDeviceInfo infoOut(QAudioDeviceInfo::defaultOutputDevice());
	if (!infoOut.isFormatSupported(m_format))
	{
		//Default format not supported - trying to use nearest
		m_format = infoOut.nearestFormat(m_format);
	}

	CreateAudioInput();
	CreateAudioOutput();
}

void AudioProcessor::readMore()
{
	if (!m_audioInput)
		return;

	qint64 len = m_audioInput->bytesReady();

	if (len >= 4096)
	{
		len = 4096;
	}

	qint64 l = m_input->read(m_buffer.data(), len);

	if (l > 0)
	{


		Q_EMIT audioDataPreapre(m_buffer, len);
		//m_output->write((char*)outdata, len);
	}
}

void AudioProcessor::CreateAudioInput()
{
	if (m_input != 0) {
		disconnect(m_input, 0, this, 0);
		m_input = 0;
	}

	m_audioInput = new QAudioInput(m_Inputdevice, m_format, this);
}

void AudioProcessor::CreateAudioOutput()
{
	m_audioOutput = new QAudioOutput(m_Outputdevice, m_format, this);
}

AudioProcessor::AudioProcessor():
	m_Inputdevice(QAudioDeviceInfo::defaultInputDevice())
	, m_Outputdevice(QAudioDeviceInfo::defaultOutputDevice())
	, m_audioInput(0)
	, m_audioOutput(0)
	, m_input(0)
	, m_buffer(BufferSize, 0)
{
	InitializeAudio();

	m_output = m_audioOutput->start();

	m_input = m_audioInput->start();

	connect(m_input, SIGNAL(readyRead()), SLOT(readMore()));

	CloseInput();
}

void AudioProcessor::CloseInput()
{
	m_audioInput->stop();
}

void AudioProcessor::StartInput()
{
	m_input = m_audioInput->start();

	connect(m_input, SIGNAL(readyRead()), SLOT(readMore()));
}

void AudioProcessor::ProcessData(QByteArray buffer, int length)
{
	m_output->write(buffer.data(), length);
}

AudioProcessor::~AudioProcessor()
{

}



