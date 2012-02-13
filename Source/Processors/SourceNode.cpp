/*
  ==============================================================================

    SourceNode.cpp
    Created: 7 May 2011 5:07:14pm
    Author:  jsiegle

  ==============================================================================
*/


#include "SourceNode.h"
#include "Editors/SourceNodeEditor.h"
#include <stdio.h>

SourceNode::SourceNode(const String& name_)
	: GenericProcessor(name_),
	  dataThread(0),
	  sourceCheckInterval(1500)
{
	if (getName().equalsIgnoreCase("Intan Demo Board")) {
		dataThread = new IntanThread(this);
	} else if (getName().equalsIgnoreCase("Custom FPGA")) {
		dataThread = new FPGAThread(this);
	} else if (getName().equalsIgnoreCase("File Reader")) {
		dataThread = new FileReaderThread(this);
	}

	setNumInputs(0);

	if (dataThread != 0) {
		setNumOutputs(dataThread->getNumChannels());
		inputBuffer = dataThread->getBufferAddress();
	} else {
		setNumOutputs(10);
	}

	setPlayConfigDetails(getNumInputs(), getNumOutputs(), 44100.0, 128);

	if (dataThread != 0)
	{
		if (!dataThread->foundInputSource())
		{
			enabledState(false);
		}
	} else {
		enabledState(false);
	}

	// check for input source every two seconds
	startTimer(sourceCheckInterval); 

}

SourceNode::~SourceNode() 
{
	if (dataThread != 0)
		deleteAndZero(dataThread);

	config->removeDataSource(this);	
}

float SourceNode::getSampleRate()
{

	if (dataThread != 0)
		return dataThread->getSampleRate();
	else
		return 44100.0;
}

void SourceNode::enabledState(bool t)
{
	if (t && !dataThread->foundInputSource())
	{
		isEnabled = false;
	} else {
		isEnabled = t;
	}


}

void SourceNode::setConfiguration(Configuration* cf)
{
	config = cf;

     DataSource* d = new DataSource(this, config);

  //   // add tetrodes -- should really be doing this dynamically
     d->addTrode(4, "TT1");
     d->addTrode(4, "TT2");
     d->addTrode(4, "TT3");
     d->addTrode(4, "TT4");

     for (int n = 0; n < d->numTetrodes(); n++)
      {
           std::cout << d->getTetrode(n)->getName();
      }
      std::cout << std::endl;

	 // // add a new data source to this configuration
     config->addDataSource(d);

}


void SourceNode::setParameter (int parameterIndex, float newValue)
{
	//std::cout << "Got parameter change notification";
}

void SourceNode::prepareToPlay (double sampleRate_, int estimatedSamplesPerBlock)
{
	//
	// We take care of thread creation and destruction in separate enable/disable function
	// 
	// prepareToPlay is called whenever the graph is edited, not only when callbacks are
	// 	about to begin
	//
}

void SourceNode::releaseResources() {}


AudioProcessorEditor* SourceNode::createEditor()
{
	SourceNodeEditor* ed = new SourceNodeEditor(this, viewport);
	setEditor(ed);
	
	std::cout << "Creating editor." << std::endl;
	//filterEditor = new FilterEditor(this);
	return ed;

	//return 0;
}

void SourceNode::timerCallback()
{
	if (dataThread->foundInputSource() && !isEnabled)
	{
		std::cout << "Input source found." << std::endl;
		//stopTimer(); // check for input source every two seconds
		enabledState(true);
		GenericEditor* ed = (GenericEditor*) getEditor();
		viewport->updateVisibleEditors(ed, 4);
	} else if (!dataThread->foundInputSource() && isEnabled) {
		std::cout << "No input source found." << std::endl;
		enabledState(false);
		GenericEditor* ed = (GenericEditor*) getEditor();
		viewport->updateVisibleEditors(ed, 4);
	}
}

bool SourceNode::enable() {
	
	std::cout << "Source node received enable signal" << std::endl;

	if (dataThread != 0)
	{
		if (dataThread->foundInputSource())
		{
			dataThread->startAcquisition();
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}

	stopTimer();

	// bool return_code = true;

	// if (getName().equalsIgnoreCase("Intan Demo Board")) {
		
	// 	dataThread = new IntanThread();
	// 	inputBuffer = dataThread->getBufferAddress();
	// 	return_code = dataThread->threadStarted();

	// 	if (!return_code)
	// 		deleteAndZero(dataThread);

	// } else if (getName().equalsIgnoreCase("Custom FPGA")) {
	// 	dataThread = new FPGAThread();
	// 	inputBuffer = dataThread->getBufferAddress();
	// } else if (getName().equalsIgnoreCase("File Reader")) {
	// 	dataThread = new FileReaderThread();
	// 	inputBuffer = dataThread->getBufferAddress();
	// }

	// return return_code;

}

bool SourceNode::disable() {

	std::cout << "Source node received disable signal" << std::endl;

	if (dataThread != 0)
		dataThread->stopAcquisition();
	
	startTimer(2000);

	// if (dataThread != 0) {
	// 	delete dataThread;
	// 	dataThread = 0;
	// }

	return true;
}

void SourceNode::acquisitionStopped()
{
	if (!dataThread->foundInputSource()) {
		std::cout << "Source node sending signal to UI." << std::endl;
		UI->disableCallbacks();
		enabledState(false);
		GenericEditor* ed = (GenericEditor*) getEditor();
		viewport->updateVisibleEditors(ed, 4);
	}
}


void SourceNode::process(AudioSampleBuffer &outputBuffer, 
                            MidiBuffer &midiMessages,
                            int& nSamples)
{

	//std::cout << "Source node processing." << std::endl;
	//std::cout << outputBuffer.getNumChannels() << " " << outputBuffer.getNumSamples() << std::endl;

	
	 outputBuffer.clear();
	 nSamples = inputBuffer->readAllFromBuffer(outputBuffer,outputBuffer.getNumSamples());
	// //setNumSamples(numRead); // write the total number of samples
	// setNumSamples(midiMessages, numRead);
	//std::cout << numRead << std::endl;
}


