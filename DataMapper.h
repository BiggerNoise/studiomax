#ifndef DataMapper_H_B6F3FC10_E5B2_11d3_854D_00A0CC3EABDD
#define DataMapper_H_B6F3FC10_E5B2_11d3_854D_00A0CC3EABDD

#pragma once
#include <vector>
#include <mmsystem.h>

struct Knob
{
	Knob() : value(0), channel(0), controllerNumber(0) {}
	int value ;
	BYTE channel ;
	BYTE controllerNumber ;
	DWORD MakeAMessage() ;
};
struct Button
{
	Button() : engaged(false) {}
	bool engaged ;
};
struct Message
{
	Message() : nrpnMSB(0), nrpnLSB(0), dataMSB(0), dataLSB(0) {}
		
	BYTE nrpnMSB ;
	BYTE nrpnLSB ;
	BYTE dataMSB ;
	BYTE dataLSB ;
};

class DataMapper
{
public:
	DataMapper() ;
	~DataMapper() ;
	bool MapStudioMixToProgram(int inIdx, int outIdx) ;
	bool MapProgramToStudioMix(int inIdx, int outIdx) ;

private:
	void CloseHardwareTranslationPorts() ;
	void CloseSoftwareTranslationPorts() ;
	static void CALLBACK MonitorFunction( HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 ) ;
	enum Direction {GOING_UP, GOING_DOWN} ;

	enum DataType { CALLBACK_HARDWARE_TO_SOFTWARE, CALLBACK_SOFTWARE_TO_HARDWARE } ;
	void MonitorData( DataType type, DWORD dwParam1, DWORD dwParam2 ) ;
	// Send a non-NRPN message on its way.
	void DataThrough(HMIDIOUT port, DWORD dwParam) ;
	// Send an NRPN message that we weren't interested in on its way.
	void NRPNDataThrough(HMIDIOUT port, const Message& msg) ;
	void DigestHardwareMessage() ;
	void DigestSoftwareMessage() ;
	// First pre-digested stage
	void OnController(int controllerNumber /*0-35*/, Direction whatsHappening) ;
	void OnKnob(int knobNumber, Direction whatsHappening) ;

private:
	std::vector<Knob> m_knobs ;
	std::vector<Button> m_buttons ;
	std::vector<long> m_currentFaderValue ;
	Message m_currentMessageFromHardware ;
	Message m_currentMessageFromSoftware ;
	HMIDIIN m_hardwareInPort ;
	HMIDIOUT m_softwareOutPort ;

	HMIDIIN m_softwareInPort ;
	HMIDIOUT m_hardwareOutPort ;
};


#endif //#ifndef DataMapper_H_B6F3FC10_E5B2_11d3_854D_00A0CC3EABDD
