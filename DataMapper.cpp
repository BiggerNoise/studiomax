#include "stdafx.h"
#include "DataMapper.h"
#include <sstream>
using namespace std ;

static const DWORD DATA_ENTRY_LSB =	0x00002600;
static const DWORD DATA_ENTRY_MSB = 0x00000600;
static const DWORD NRPN_NUMBER_MSB = 0x00006300 ;
static const DWORD NRPN_NUMBER_LSB = 0x00006200;

// #define DEVELOPER_TRACE

namespace {
	inline short NRPNToValue(short MSB, short LSB)
	{
		return (MSB<<7) + LSB ;
	}
	const int CLOSE_THRESHOLD = 400 ;
	inline bool ValuesAreClose(long val1, long val2)
	{
		if( val1 > val2 )
			return (val1 - CLOSE_THRESHOLD) < val2 ;
		else
			return (val2 - CLOSE_THRESHOLD) < val1 ;
	}
}
DataMapper::DataMapper() : 
m_knobs(18), 
m_buttons(9), 
m_currentFaderValue(9, -5000), 
m_hardwareInPort(0),
m_softwareOutPort(0),
m_hardwareOutPort(0)
{
	// static code the knob setup for now
	std::vector<Knob>::iterator knob = m_knobs.begin() ;
	int controllerNumber = 1 ;
	for(; knob != m_knobs.end() ; ++knob, ++controllerNumber )
	{
		(*knob).value = 0 ;
		(*knob).channel = 0 ;
		(*knob).controllerNumber = controllerNumber ;
	}
}
DataMapper::~DataMapper()
{
	CloseSoftwareTranslationPorts() ;
	CloseHardwareTranslationPorts() ;
}
bool DataMapper::MapProgramToStudioMix(int inIdx, int outIdx)
{
	CloseSoftwareTranslationPorts() ;
	MMRESULT res = ::midiInOpen(&m_softwareInPort, inIdx, reinterpret_cast<DWORD>(DataMapper::MonitorFunction), reinterpret_cast<DWORD>(this), CALLBACK_FUNCTION ) ;
	if( res != MMSYSERR_NOERROR )
	{
		stringstream str ;
		str << "Unable to open Software Input Port." << "Driver open returned: " << res << ends ;
		AfxMessageBox(str.str().c_str()) ;
		return false ;
	}
		
	::midiInStart(m_softwareInPort) ;
	res = ::midiOutOpen(&m_hardwareOutPort, outIdx, 0, 0, CALLBACK_NULL) ;
	if( res != MMSYSERR_NOERROR )
	{
		stringstream str ;
		str << "Unable to open Hardware Output Port." << "Driver open returned: " << res << ends ;
		AfxMessageBox(str.str().c_str()) ;
		CloseHardwareTranslationPorts() ;
		return false ;
	}
	return true ;
}
bool DataMapper::MapStudioMixToProgram(int inIdx, int outIdx)
{
	CloseHardwareTranslationPorts() ;
	MMRESULT res = ::midiInOpen(&m_hardwareInPort, inIdx, reinterpret_cast<DWORD>(DataMapper::MonitorFunction), reinterpret_cast<DWORD>(this), CALLBACK_FUNCTION ) ;
	if( res != MMSYSERR_NOERROR )
	{
		stringstream str ;
		str << "Unable to open Hardware Input Port." << "Driver open returned: " << res << ends ;
		AfxMessageBox(str.str().c_str()) ;
		return false ;
	}
		
	::midiInStart(m_hardwareInPort) ;
	res = ::midiOutOpen(&m_softwareOutPort, outIdx, 0, 0, CALLBACK_NULL) ;
	if( res != MMSYSERR_NOERROR )
	{
		stringstream str ;
		str << "Unable to open Software Output Port." << "Driver open returned: " << res << ends ;
		AfxMessageBox(str.str().c_str()) ;
		CloseHardwareTranslationPorts() ;
		return false ;
	}
	return true ;
}
void DataMapper::CloseHardwareTranslationPorts()
{
	if(m_hardwareInPort) // close old
	{
		::midiInClose(m_hardwareInPort) ;
		m_hardwareInPort = 0 ;
	}
	if(m_softwareOutPort)
	{
		::midiOutClose(m_softwareOutPort) ;
		m_softwareOutPort = 0 ;
	}
}
void DataMapper::CloseSoftwareTranslationPorts()
{
	if(m_softwareInPort) // close old
	{
		::midiInClose(m_softwareInPort) ;
		m_softwareInPort = 0 ;
	}
	if(m_hardwareOutPort)
	{
		::midiOutClose(m_hardwareOutPort) ;
		m_hardwareOutPort = 0 ;
	}
}

void CALLBACK DataMapper::MonitorFunction( HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 )
{
	if(	MIM_DATA == wMsg )
	{
		DataMapper* pMapper = reinterpret_cast<DataMapper*>(dwInstance) ;
		if( hMidiIn == pMapper->m_hardwareInPort )
			pMapper->MonitorData(CALLBACK_HARDWARE_TO_SOFTWARE, dwParam1, dwParam2) ;
		else if( hMidiIn == pMapper->m_softwareInPort )
			pMapper->MonitorData(CALLBACK_SOFTWARE_TO_HARDWARE, dwParam1, dwParam2) ;
	}
	return ;
}
void DataMapper::DataThrough(HMIDIOUT port, DWORD dwParam)
{
#ifdef DEVELOPER_TRACE
	stringstream str ;
	str << (port == m_softwareOutPort ? "HW: " : "SW: ") << "Bypass Data:" << hex << long(dwParam) << "\n" << ends ;
	::OutputDebugString(str.str().c_str()) ;
#endif

	::midiOutShortMsg(port, dwParam) ;
}
void DataMapper::NRPNDataThrough(HMIDIOUT port, const Message& msg)
{
#ifdef DEVELOPER_TRACE
	stringstream str ;
	str << (port == m_softwareOutPort ? "HW: " : "SW: ") << "NRPN Through:" << hex 
		<< short(msg.nrpnMSB) << ", " 
		<< short(msg.nrpnLSB) << ", " 
		<< short(msg.dataMSB) << ", " 
		<< short(msg.dataLSB) << "\n" << ends ;
	::OutputDebugString(str.str().c_str()) ;
#endif
	
	::midiOutShortMsg(port,  (msg.nrpnMSB << 16) | NRPN_NUMBER_MSB | 0xBF ) ;
	::midiOutShortMsg(port,  (msg.nrpnLSB << 16) | NRPN_NUMBER_LSB | 0xBF ) ;
	::midiOutShortMsg(port,  (msg.dataMSB << 16) | DATA_ENTRY_MSB | 0xBF ) ;
	::midiOutShortMsg(port,  (msg.dataLSB << 16) | DATA_ENTRY_LSB | 0xBF ) ;
}

void DataMapper::MonitorData( DataType type,  DWORD dwParam1, DWORD dwParam2 )
{
	if( (0x000000FF & dwParam1) != 0xBF ) // only care about Ctrl/Mode Change on channel 16
	{
		DataThrough( type == CALLBACK_HARDWARE_TO_SOFTWARE ? m_softwareOutPort : m_hardwareOutPort, dwParam1) ;
		return ;
	}
	Message& msg = type == CALLBACK_HARDWARE_TO_SOFTWARE ? m_currentMessageFromHardware : m_currentMessageFromSoftware ;

	int value = (0x00FF0000 & dwParam1) >> 16 ; //shift down to value position

	bool fire = false ;
	switch(0x0000FF00 & dwParam1)
	{
	case NRPN_NUMBER_MSB: 
		msg.nrpnMSB = value ;
		break ;
	case NRPN_NUMBER_LSB: 
		msg.nrpnLSB = value ;
		break ;
	case DATA_ENTRY_MSB: 
		msg.dataMSB = value ;
		break ;
	case DATA_ENTRY_LSB: 
		msg.dataLSB = value ;
		fire = true ;
		break ;
	}
	if(fire)
	{
		if(type == CALLBACK_HARDWARE_TO_SOFTWARE)
			DigestHardwareMessage() ;
		else
			DigestSoftwareMessage() ;
	}
}
void DataMapper::DigestSoftwareMessage()
{
	switch( m_currentMessageFromSoftware.nrpnMSB )
	{
	case 0x23: // Everything on the master strip
		if( m_currentMessageFromSoftware.nrpnLSB == 0x17 )
		{ // the master fader...
			long incomingValue = NRPNToValue(m_currentMessageFromSoftware.dataMSB, m_currentMessageFromSoftware.dataLSB) ;
			// If the values are too close, then squash the message....
			if(! ValuesAreClose(m_currentFaderValue[8], incomingValue) )
			{
				m_currentFaderValue[8] = incomingValue ;
				NRPNDataThrough(m_hardwareOutPort,m_currentMessageFromSoftware) ;
			}
			else
			{
#ifdef DEVELOPER_TRACE
				stringstream str ;
				str << "Supressing Fader Message on Master Fader.  Stored Fader Value: " 
					<< m_currentFaderValue[8]
					<< ".  Incoming Value: "
					<< incomingValue << "\n" << ends ;
				::OutputDebugString(str.str().c_str()) ;
#endif
			}
		}
		else
			NRPNDataThrough(m_hardwareOutPort,m_currentMessageFromSoftware) ;
		
		break ;
	case 0x1B:
		{
			// The channel faders
			long incomingValue = NRPNToValue(m_currentMessageFromSoftware.dataMSB, m_currentMessageFromSoftware.dataLSB) ;
			if(! ValuesAreClose(m_currentFaderValue[m_currentMessageFromSoftware.nrpnLSB-0x2C], incomingValue) )
			{
				m_currentFaderValue[m_currentMessageFromSoftware.nrpnLSB-0x2C] = incomingValue ;
				NRPNDataThrough(m_hardwareOutPort,m_currentMessageFromSoftware) ;
			}
			else
			{
#ifdef DEVELOPER_TRACE
				stringstream str ;
				str << "Supressing Fader Message on Fader #"<< m_currentMessageFromSoftware.nrpnLSB-0x2C << ".  Stored Fader Value: " 
					<< m_currentFaderValue[m_currentMessageFromSoftware.nrpnLSB-0x2C]
					<< ".  Incoming Value: "
					<< incomingValue << "\n" << ends ;
				::OutputDebugString(str.str().c_str()) ;
#endif
			}

			break ;
		}

	default:
		NRPNDataThrough(m_hardwareOutPort,m_currentMessageFromSoftware) ;
	}
	return ;
}
void DataMapper::DigestHardwareMessage()
{
	int controllerNumber = 0 ;
	switch( m_currentMessageFromHardware.nrpnMSB )
	{
	case 0x3: // Top row knobs 1-8
		controllerNumber = m_currentMessageFromHardware.nrpnLSB - 0x74 ;
		break ;
	case 0xB: // bottom row knobs 1-8
		controllerNumber = 9 +(m_currentMessageFromHardware.nrpnLSB - 0x5C) ;
		break ;
	case 0x13: // buttons 1-8
		controllerNumber = 18 +(m_currentMessageFromHardware.nrpnLSB - 0x44) ;
		break ;
	case 0x23: // Everything on the master strip
		if( m_currentMessageFromHardware.nrpnLSB < 0x17 )
			controllerNumber = (m_currentMessageFromHardware.nrpnLSB - 0x14)*9 + 8 ;
		else
		{ // the master fader...
			long incomingValue = NRPNToValue(m_currentMessageFromHardware.dataMSB, m_currentMessageFromHardware.dataLSB) ;
			if(! ValuesAreClose(m_currentFaderValue[8], incomingValue) )
			{
				m_currentFaderValue[8] = incomingValue ;
				NRPNDataThrough(m_softwareOutPort, m_currentMessageFromHardware) ;
			}
			return ;
		}
		break ;
	case 0x1B:
		{
			long incomingValue = NRPNToValue(m_currentMessageFromHardware.dataMSB, m_currentMessageFromHardware.dataLSB) ;
			if(! ValuesAreClose(m_currentFaderValue[m_currentMessageFromHardware.nrpnLSB-0x2C], incomingValue) )
			{
				m_currentFaderValue[m_currentMessageFromHardware.nrpnLSB-0x2C] = incomingValue ;
				NRPNDataThrough(m_softwareOutPort,m_currentMessageFromHardware) ;
			}
			return ;
		}

	default:
		NRPNDataThrough(m_softwareOutPort, m_currentMessageFromHardware) ;
		return ;
	}
	Direction whatsHappening = m_currentMessageFromHardware.dataMSB ? GOING_DOWN : GOING_UP ;
	OnController(controllerNumber, whatsHappening) ;
}
void DataMapper::OnController(int controllerNumber /*0-26*/, Direction whatsHappening)
{
	if( controllerNumber >= 18 ) //button press!
		m_buttons[controllerNumber - 18].engaged = (whatsHappening==GOING_UP) ;
 	else
		OnKnob(controllerNumber, whatsHappening) ;
}

void DataMapper::OnKnob(int knobNumber, Direction whatsHappening)
{
	int buttonNumber = knobNumber ;
	if( knobNumber >= 9 )
		buttonNumber = knobNumber - 9 ;
	
	m_knobs[knobNumber].value += ((m_buttons[buttonNumber].engaged ? 1 : 5) * ((whatsHappening==GOING_UP) ? 1 : -1)) ;
	if(whatsHappening==GOING_UP)
		m_knobs[knobNumber].value = std::_cpp_min<int>(m_knobs[knobNumber].value, 127) ;
	else
		m_knobs[knobNumber].value = std::_cpp_max<int>(m_knobs[knobNumber].value, 0) ;

	DWORD val = ::midiOutShortMsg(m_softwareOutPort, m_knobs[knobNumber].MakeAMessage()) ;
	
	
#ifdef DEVELOPER_TRACE
	stringstream str ;
	str << "StudioMix Knob: " << knobNumber 
		<< (whatsHappening==GOING_UP ? " Going Up" : " Going Down") 
		<< (m_buttons[buttonNumber].engaged ? " (fine) " : " (normal) ")
		<< "Value: " << long(m_knobs[knobNumber].value)
		<< "\n" << ends ;
	::OutputDebugString(str.str().c_str()) ;
#endif
}
DWORD Knob::MakeAMessage()
{
	DWORD msg = 0xB0 ;
	msg |= channel ;
	msg |= (static_cast<DWORD>(controllerNumber) << 8) ; //shift down to controller position & combine
	msg |= (static_cast<DWORD>(value) << 16) ; //shift down to value position & combine
	return msg ;
}