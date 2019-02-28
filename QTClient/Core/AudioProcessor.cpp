#include "AudioProcessor.h"
#include <QAudioFormat>
#include <algorithm>
#include <iostream>
#include <QBuffer>
#include <QEventLoop>
#include <QDebug>

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
		Q_EMIT audioDataPrepare(m_buffer, len);
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
	, m_terminating(false)
	, m_queueBuffers(std::make_unique<SharedQueue<QByteArray>>())
{
	InitializeAudio();

	m_output = m_audioOutput->start();
	m_input = m_audioInput->start();

	connect(m_input, SIGNAL(readyRead()), SLOT(readMore()));

	CloseInput();

	m_thread = std::thread(&AudioProcessor::ProcessRoutine, this);
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

void AudioProcessor::StopThread()
{
	m_terminating = true;
}

void AudioProcessor::AddToQueue(QByteArray buffer)
{
	m_queueBuffers->push_back(buffer);
}

void AudioProcessor::ProcessData(QByteArray buffer)
{
	if (!m_Outputdevice.isNull())
	{
		//if (m_output->isOpen())
		//{
		m_output->write(buffer.data(), buffer.size());
		//}
	}
}

AudioProcessor::~AudioProcessor()
{
	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

void AudioProcessor::ProcessRoutine()
{
	while (!m_terminating)
	{
		while (!m_queueBuffers->empty())
		{
			auto currentBuffer = m_queueBuffers->front();

			this->ProcessData(currentBuffer);

			m_queueBuffers->pop_front();
		}
	}
}



