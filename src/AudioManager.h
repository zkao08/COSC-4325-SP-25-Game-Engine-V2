#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

class AudioManager
{
	public:
		AudioManager();
		~AudioManager();

		bool startUp();
		void shutDown();

	private:
		IXAudio2* pXAudio2;
		IXAudio2MasteringVoice* pMasteringVoice;
};

//global singleton
extern AudioManager gAudioManager;

#endif