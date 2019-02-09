#include <iostream>

#include "wav_core.h"

using namespace std;

int main()
{
	try {
		WavData wav1;
		wav1.createFromFile("test.wav");
		std::cout << wav1.getDescription() << std::endl;
		wav1.makeWavFile("test.wav");
		wav1.cutLast(20);
		wav1.cutFirst(6);
		wav1.makeReverb(0.5, 0.6f);
		wav1.makeMono("out.wav");
		std::cout << wav1.getDescription() << std::endl;
		bool isStereo = wav1.isStereo();
		int sampleRate = wav1.getSampleRate();
		int chanCount = wav1.getChannelsCount();
	}
	catch (std::exception & we) {
		cerr << we.what();
	}

	return 0;
}