#pragma once
#include <QObject>
#include <QWidget>
#include <QIODevice>
#include <QAudioOutput>
#include <QAudioInput>
#include <mutex>

class AudioProcessor: public QObject{
	Q_OBJECT	
private:
	void InitializeAudio();
	void CreateAudioInput();
	void CreateAudioOutput();

public:
	explicit AudioProcessor();
	void CloseInput();
	void StartInput();
	void ProcessData(QByteArray buffer, int length);
	~AudioProcessor();
signals:
	void audioDataPreapre(QByteArray, int lenght);
private slots:
	void readMore();
private:
	const int BufferSize = 14096;
	QAudioDeviceInfo m_Inputdevice;
	QAudioDeviceInfo m_Outputdevice;
	QAudioFormat m_format;
	QAudioOutput *m_audioOutput;
	QAudioInput *m_audioInput;
	QIODevice *m_input;
	QIODevice *m_output;
	QByteArray m_buffer;
	int m_iVolume = 10;
	std::mutex m_mutex;
};