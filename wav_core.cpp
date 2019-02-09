#include <cstdio>
#include "wav_core.h"


void WavData::checkHeader(size_t fileSizeBytes) const
{
	// Go to wav_header.h for details

	if (wavHeader.chunkId[0] != 'R' ||
		wavHeader.chunkId[1] != 'I' ||
		wavHeader.chunkId[2] != 'F' ||
		wavHeader.chunkId[3] != 'F')
	{
		throw HeaderException("HEADER_RIFF_ERROR"); //return HEADER_RIFF_ERROR;
	}

	if (wavHeader.chunkSize != fileSizeBytes - 8) {
		throw HeaderException("HEADER_FILE_SIZE_ERROR"); //return HEADER_FILE_SIZE_ERROR;
	}

	if (wavHeader.format[0] != 'W' ||
		wavHeader.format[1] != 'A' ||
		wavHeader.format[2] != 'V' ||
		wavHeader.format[3] != 'E')
	{
		throw HeaderException("HEADER_WAVE_ERROR"); //return HEADER_WAVE_ERROR;
	}

	if (wavHeader.subchunk1Id[0] != 'f' ||
		wavHeader.subchunk1Id[1] != 'm' ||
		wavHeader.subchunk1Id[2] != 't' ||
		wavHeader.subchunk1Id[3] != ' ')
	{
		throw HeaderException("HEADER_FMT_ERROR"); //return HEADER_FMT_ERROR;
	}

	if (wavHeader.audioFormat != 1) {
		throw HeaderException("HEADER_NOT_PCM"); //return HEADER_NOT_PCM;
	}

	if (wavHeader.subchunk1Size != 16) {
		throw HeaderException("HEADER_SUBCHUNK1_ERROR"); //return HEADER_SUBCHUNK1_ERROR;
	}

	if (wavHeader.byteRate != wavHeader.sampleRate * wavHeader.numChannels * wavHeader.bitsPerSample / 8) {
		throw HeaderException("HEADER_BYTES_RATE_ERROR"); //return HEADER_BYTES_RATE_ERROR;
	}

	if (wavHeader.blockAlign != wavHeader.numChannels * wavHeader.bitsPerSample / 8) {
		throw HeaderException("HEADER_BLOCK_ALIGN_ERROR"); //return HEADER_BLOCK_ALIGN_ERROR;
	}

	if (wavHeader.subchunk2Id[0] != 'd' ||
		wavHeader.subchunk2Id[1] != 'a' ||
		wavHeader.subchunk2Id[2] != 't' ||
		wavHeader.subchunk2Id[3] != 'a')
	{
		throw HeaderException("HEADER_FMT_ERROR"); // return HEADER_FMT_ERROR;
	}

	if (wavHeader.subchunk2Size != fileSizeBytes - 44)
	{
		throw HeaderException("HEADER_SUBCHUNK2_SIZE_ERROR"); // return HEADER_SUBCHUNK2_SIZE_ERROR;
	}
}

void WavData::clearHeader()
{
	memset(&wavHeader, 0, sizeof(WAVHEADER));
}

void WavData::fillHeader(int channelsCount, int bitsPerSample, int sampleRate, int samplesCountPerChan)
{
	// Go to wav_header.h for details

	wavHeader.chunkId[0] = 'R';
	wavHeader.chunkId[1] = 'I';
	wavHeader.chunkId[2] = 'F';
	wavHeader.chunkId[3] = 'F';

	wavHeader.format[0] = 'W';
	wavHeader.format[1] = 'A';
	wavHeader.format[2] = 'V';
	wavHeader.format[3] = 'E';

	wavHeader.subchunk1Id[0] = 0x66;
	wavHeader.subchunk1Id[1] = 0x6d;
	wavHeader.subchunk1Id[2] = 0x74;
	wavHeader.subchunk1Id[3] = 0x20;

	wavHeader.subchunk2Id[0] = 0x64;
	wavHeader.subchunk2Id[1] = 0x61;
	wavHeader.subchunk2Id[2] = 0x74;
	wavHeader.subchunk2Id[3] = 0x61;

	wavHeader.audioFormat = 1;
	wavHeader.subchunk1Size = 16;
	wavHeader.bitsPerSample = 16;
	wavHeader.numChannels = channelsCount;
	wavHeader.sampleRate = sampleRate;
	wavHeader.chunkSize = sizeof(WAVHEADER) + wavHeader.bitsPerSample / 8 * samplesCountPerChan * channelsCount - 8;
	wavHeader.subchunk2Size = wavHeader.bitsPerSample / 8 * samplesCountPerChan * channelsCount;
	wavHeader.byteRate = wavHeader.sampleRate * wavHeader.numChannels * wavHeader.bitsPerSample / 8;
	wavHeader.blockAlign = wavHeader.numChannels * wavHeader.bitsPerSample / 8;

}

std::string WavData::getDescription() const
{
	return "audioFormat: " + std::to_string(wavHeader.audioFormat) + "\n"
		+ "numChannels: " + std::to_string(wavHeader.numChannels) + "\n"
		+ "sampleRate: " + std::to_string(wavHeader.sampleRate) + "\n"
		+ "bitsPerSample: " + std::to_string(wavHeader.bitsPerSample) + "\n"
		+ "byteRate: " + std::to_string(wavHeader.byteRate) + "\n"
		+ "blockAlign: " + std::to_string(wavHeader.blockAlign) + "\n"
		+ "chunkSize: " + std::to_string(wavHeader.chunkSize) + "\n"
		+ "subchunk1Size: " + std::to_string(wavHeader.subchunk1Size) + "\n"
		+ "subchunk2Size: " + std::to_string(wavHeader.subchunk2Size) + "\n"
		+ "-------------------------";
}

void WavData::readHeader(const char * filename)
{
	printf(">>>> read_header( %s )\n", filename);

	std::ifstream inputFile(filename, std::ifstream::binary);
	if (!inputFile.is_open()) {
		throw IOError("IO error with " + (std::string)filename);
	}

	// source: https://rsdn.org/forum/cpp/1838147.hot ------------------------------>>>>
	inputFile.read((char *)&wavHeader, sizeof(WAVHEADER));

	if (inputFile.gcount() != sizeof(WAVHEADER)) {
		// can't read header, because the file is too small.
		throw BadFormat((std::string)filename + ": can't read header, because the file has bad format");
	}
	// <<<<<<<<<<--------------------

	inputFile.seekg(0, std::ifstream::end); // seek to the end of the file
	
	size_t fileSize = inputFile.tellg(); // current position is a file size

	checkHeader(fileSize);
	inputFile.close();
}

void WavData::createFromFile(const std::string & filename)
{
	extractData(filename.c_str());
}

void WavData::extractData(const char * filename)
{
	printf(">>>> extract_data_int16( %s )\n", filename);
	clearHeader();

	std::ifstream inputFile(filename, std::ifstream::binary);
	if (!inputFile.is_open()) {
		throw IOError("IO error with " + (std::string)filename);
	}
	
	inputFile.read((char *)&wavHeader, sizeof(WAVHEADER));

	if (inputFile.gcount() != sizeof(WAVHEADER)) {
		// can't read header, because the file is too small.
		throw BadFormat((std::string)filename + ": can't read header, because the file has bad format");
	}

	if (wavHeader.bitsPerSample != 16) {
		// Only 16-bit samples is supported.
		throw UnsupportedFormat("Only 16-bit samples is supported"); //return UNSUPPORTED_FORMAT;
	}

	inputFile.seekg(0, std::ifstream::end); // seek to the end of the file

	checkHeader(inputFile.tellg());

	inputFile.seekg(sizeof(WAVHEADER), std::ios_base::beg); // Seek to the begining of PCM data.

	int channels—ount = wavHeader.numChannels;
	int samplesPerChan = (wavHeader.subchunk2Size / sizeof(short)) / channels—ount;

	// 1. Reading all PCM data from file to a single vector.
	std::vector<short> allChannels;
	allChannels.resize(channels—ount * samplesPerChan);
	inputFile.read((char *)allChannels.data(), wavHeader.subchunk2Size);
	if (inputFile.gcount() != wavHeader.subchunk2Size) {
		throw IOError("IO error: extract_data_int16() read only" + std::to_string(inputFile.gcount())); //return IO_ERROR;
	}
	inputFile.close();

	

	// 2. Put all channels to its own vector.
	channelsData.resize(channels—ount);
	for (size_t ch = 0; ch < channelsData.size(); ch++) {
		channelsData[ch].resize(samplesPerChan);
	}

	for (int ch = 0; ch < channels—ount; ch++) {
		std::vector<short>& chdata = channelsData[ch];
		for (size_t i = 0; i < samplesPerChan; i++) {
			chdata[i] = allChannels[channels—ount * i + ch];
		}
	}
}

void WavData::makeWavFile(const char * filename)
{
	printf(">>>> make_wav_file( %s )\n", filename);

	int channelsCount = channelsData.size();

	if (channelsCount < 1) {
		throw BadParams("Bad params"); //return BAD_PARAMS;
	}

	int samplesCountPerChan = channelsData[0].size();

	// Verify that all channels have the same number of samples.
	for (size_t ch = 0; ch < channelsCount; ch++) {
		if (channelsData[ch].size() != (size_t)samplesCountPerChan) {
			throw BadParams("Bad params"); //return BAD_PARAMS;
		}
	}

	fillHeader(channelsCount, wavHeader.bitsPerSample, wavHeader.sampleRate, samplesCountPerChan);

	std::vector<short> allChannels;
	allChannels.resize(channelsCount * samplesCountPerChan);

	for (int ch = 0; ch < channelsCount; ch++) {
		const std::vector<short>& channelData = channelsData[ch];
		for (size_t i = 0; i < samplesCountPerChan; i++) {
			allChannels[channelsCount * i + ch] = channelData[i];
		}
	}

	std::ofstream outputFile(filename, std::ofstream::binary | std::ofstream::out);
	outputFile.write((char *)&wavHeader, sizeof(WAVHEADER));
	outputFile.write((char *)allChannels.data(), allChannels.size() * sizeof(short));
	if (!outputFile.is_open()) {
		throw IOError("IO error with " + (std::string)filename); //return IO_ERROR;
	}

	outputFile.close();
}

void WavData::makeMono(const std::string & filename) {
	int channelsCount = channelsData.size();

	if (channelsCount != 2) {
		throw BadParams("Count of channels not equal 2"); //return BAD_PARAMS;
	}

	int samplesCountPerChan = channelsData[0].size();

	// Verify that all channels have the same number of samples.
	for (size_t ch = 0; ch < channelsCount; ch++) {
		if (channelsData[ch].size() != (size_t)samplesCountPerChan) {
			throw BadParams("Count of channels not equal 2"); //return BAD_PARAMS;
		}
	}

	std::vector<short> mono = channelsData[0];
	mono.resize(1);
	mono.resize(samplesCountPerChan);

	// Mono channel is an arithmetic mean of all (two) channels.
	for (size_t i = 0; i < samplesCountPerChan; i++) {
		mono[i] = (channelsData[0][i] + channelsData[1][i]) / 2;
	}

	channelsData[0] = mono;
	channelsData.pop_back();
	makeWavFile(filename.c_str());
}

void WavData::makeReverb(double delaySeconds, float decay)
{
	int channelsCount = channelsData.size();

	if (channelsCount < 1) {
		throw BadParams("Count of channels not equal 2"); //return BAD_PARAMS;
	}

	int samplesCountPerChan = channelsData[0].size();

	// Verify that all channels have the same number of samples.
	for (size_t ch = 0; ch < channelsCount; ch++) {
		if (channelsData[ch].size() != (size_t)samplesCountPerChan) {
			throw BadParams("Count of channels not equal 2"); //return BAD_PARAMS;
		}
	}

	int delaySamples = (int)(delaySeconds * wavHeader.sampleRate);


	for (size_t ch = 0; ch < channelsCount; ch++) {
		std::vector<float> tmp;
		tmp.resize(channelsData[ch].size());

		// Convert signal from short to float
		for (size_t i = 0; i < samplesCountPerChan; i++) {
			tmp[i] = channelsData[ch][i];
		}

		// Add a reverb
		for (size_t i = 0; i < samplesCountPerChan - delaySamples; i++) {
			tmp[i + delaySamples] += decay * tmp[i];
		}

		// Find maximum signal's magnitude
		float maxMagnitude = 0.0f;
		for (size_t i = 0; i < samplesCountPerChan - delaySamples; i++) {
			if (abs(tmp[i]) > maxMagnitude) {
				maxMagnitude = abs(tmp[i]);
			}
		}

		// Signed short can keep values from -32768 to +32767,
		// After reverb, usually there are values large 32000.
		// So we must scale all values back to [ -32768 ... 32768 ]
		float normCoef = 30000.0f / maxMagnitude;
		printf("max_magnitude = %.1f, coef = %.3f\n", maxMagnitude, normCoef);

		// Scale back and transform floats to shorts.
		for (size_t i = 0; i < samplesCountPerChan; i++) {
			channelsData[ch][i] = (short)(normCoef * tmp[i]);
		}
	}
}

void WavData::cutLast(int seconds)
{
	int toDelete = wavHeader.bitsPerSample / 8 * wavHeader.sampleRate * seconds / sizeof(short);

	for (auto &i : channelsData) {
		i.erase(i.end() - toDelete, i.end());
	}
}

void WavData::cutFirst(int seconds)
{
	int toDelete = wavHeader.bitsPerSample / 8 * wavHeader.sampleRate * seconds / sizeof(short);

	for (auto &i : channelsData) {
		i.erase(i.begin(), i.begin() + toDelete);
	}
}

bool WavData::isStereo() const
{
	return channelsData.size() == 2;
}

int WavData::getSampleRate() const
{
	return wavHeader.sampleRate;
}

int WavData::getChannelsCount() const
{
	return wavHeader.numChannels;
}

void WavData::makeSampleRate(int sampleRate)
{
	wavHeader.sampleRate = sampleRate;
}
