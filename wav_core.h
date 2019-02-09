#ifndef WAV_CORE_H
#define WAV_CORE_H

#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#include "wav_header.h"
#include "WavException.h"

class WavData {
private:
	WAVHEADER wavHeader;

	std::vector< std::vector<short> > channelsData;

	void checkHeader(size_t fileSizeBytes) const;
	
	void clearHeader(); // Fill header with zeroes

	void fillHeader(int channelsCount, int bitsPerSample, int sampleRate, int samplesCountPerChan);
public:
	WavData() {};
	WavData(const std::string & filename) {};
	~WavData() {};

	// Prints header's data from 'wavHeader' address.
	std::string getDescription() const;

	// Reads file 'filename' and puts header's data to 'header' address.
	// Also checks header validity, returns.
	void readHeader(const char * filename);

	void createFromFile(const std::string & filename);

	// Reads file 'filename' and puts PCM data (raw sound data) to 'chansData'.
	// Also checks header validity.
	void extractData(const char* filename);

	// Creates a new WAV file 'filename', using 'sample_rate' and PCM data from 'channels_data'.
	void makeWavFile(const char* filename);

	// Makes mono PCM data from stereo 'source'.
	void makeMono(const std::string & filename);

	// Adds a reverberation (it's like an echo).
	void makeReverb(double delaySeconds, float decay);

	// cut last seconds
	void cutLast(int seconds);

	// cut first seconds
	void cutFirst(int seconds);

	// check for stereo
	bool isStereo() const;

	// get sample rate
	int getSampleRate() const;

	// get count of channels
	int getChannelsCount() const;

	// change sample rate
	void makeSampleRate(int sampleRate);
};
#endif // WAV_CORE_H