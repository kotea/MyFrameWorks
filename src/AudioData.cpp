#include"AudioData.h"

AudioData::AudioData() {

}
AudioData::~AudioData() {

}

int AudioData::GetPcmOffset() {
	return this->pcmOffset;
}

int AudioData::GetLoopLength() {
	return this->loopLength;
}

int AudioData::GetLoopStart() {
	return this->loopStart;
}

int AudioData::GetPcmSize() {
	return this->pcmSize;
}

int AudioData::GetBlockSize() {
	return this->blockSize;
}

SoundFormat AudioData::GetFormat() {
	return this->format;
}

int AudioData::GetSamplingRate() {
	return this->samplingRate;
}