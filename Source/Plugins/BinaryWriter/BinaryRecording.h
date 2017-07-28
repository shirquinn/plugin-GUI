/*
------------------------------------------------------------------

This file is part of the Open Ephys GUI
Copyright (C) 2013 Open Ephys

------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef BINARYRECORDING_H
#define BINARYRECORDING_H

#include <RecordingLib.h>
#include "SequentialBlockFile.h"

//Defines for the old-stype original recording spikes and event files
#define HEADER_SIZE 1024
#define BLOCK_LENGTH 1024
#define VERSION 0.4
#define VSTR(s) #s
#define VSTR2(s) VSTR(s)
#define VERSION_STRING VSTR2(VERSION)

namespace BinaryRecordingEngine
{

	class BinaryRecording : public RecordEngine
	{
	public:
		BinaryRecording();
		~BinaryRecording();

		String getEngineID() const override;
		void openFiles(File rootFolder, int experimentNumber, int recordingNumber) override;
		void closeFiles() override;
		void writeData(int writeChannel, int realChannel, const float* buffer, int size) override;
		void writeEvent(int eventIndex, const MidiMessage& event) override;
		void resetChannels() override;
		void addSpikeElectrode(int index, const SpikeChannel* elec) override;
		void writeSpike(int electrodeIndex, const SpikeEvent* spike) override;
		void writeTimestampSyncText(uint16 sourceID, uint16 sourceIdx, int64 timestamp, float, String text) override;

		static RecordEngineManager* getEngineManager();

	private:
		typedef Array<const DataChannel*> ContinuousGroup;
		void openSpikeFile(String basepath, int spikeIndex, int recordingNumber);
		String generateSpikeHeader(const SpikeChannel* elec);
		String generateEventHeader();

		void openMessageFile(String basepath, int recordingNumber);
		void openEventFile(String basepath, int recordingNumber);
		void writeTTLEvent(int eventIndex, const MidiMessage& event);
		void writeMessage(String message, uint16 processorID, uint16 channel, int64 timestamp);

		HeapBlock<float> m_scaledBuffer;
		HeapBlock<int16> m_intBuffer;
		int m_bufferSize;

		OwnedArray<SequentialBlockFile>  m_DataFiles;
		Array<unsigned int> m_channelIndexes;
		Array<unsigned int> m_fileIndexes;
		Array<ContinuousGroup> m_dataChannels;

		FILE* eventFile;
		FILE* messageFile;
		Array<FILE*> spikeFileArray;
		int m_recordingNum;
		Array<int64> m_startTS;

		CriticalSection diskWriteLock;

		//Compile-time constants
		const int samplesPerBlock{ 4096 };

	};

}

#endif