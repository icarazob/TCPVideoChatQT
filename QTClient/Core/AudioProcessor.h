#pragma once
#include <QObject>
#include <QIODevice>
#include <QAudioOutput>
#include <QAudioInput>
#include <thread>
#include "SharedQueue.h"

class AudioProcessor: public QObject{
	Q_OBJECT	
private:
	void InitializeAudio();
	void CreateAudioInput();
	void CreateAudioOutput();
	void ProcessData(QByteArray buffer);

public:
	explicit AudioProcessor();

	void CloseInput();
	void StartInput();
	void StopThread();
	void AddToQueue(QByteArray buffer);


	~AudioProcessor();

Q_SIGNALS:
	void audioDataPrepare(QByteArray, int lenght);

private slots:
	void readMore();

private:

	void ProcessRoutine();

	bool m_terminating = false;
	const int BufferSize = 14096;
	QAudioDeviceInfo m_Inputdevice;
	QAudioDeviceInfo m_Outputdevice;
	QAudioFormat m_format;
	QAudioOutput *m_audioOutput;
	QAudioInput *m_audioInput;
	QIODevice *m_input;
	QIODevice *m_output;
	QByteArray m_buffer;

	std::unique_ptr<SharedQueue<QByteArray>> m_queueBuffers;
	std::thread m_thread;
};