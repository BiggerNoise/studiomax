#include "stdafx.h"
#include "DataMapper.h"
#include <sstream>
using namespace std ;


DataMapper::DataMapper() : m_knobs(18), m_buttons(9), m_inPort(0)
{
	m_currentMessage.nrpnMSB = 0 ;
	m_currentMessage.nrpnLSB = 0 ;
	m_currentMessage.dataMSB = 0 ;
	m_currentMessage.dataLSB = 0 ;
}
DataMapper::~DataMapper()
{
	CloseShop() ;
}
void DataMapper::StartMapping(int inIdx, int outIdx)
{
	CloseShop() ;
	MMRESULT res = ::midiInOpen(&m_inPort, inIdx, reinterpret_cast<DWORD>(DataMapper::MonitorFunction), reinterpret_cast<DWORD>(this), CALLBACK_FUNCTION ) ;
	::midiInStart(m_inPort) ;
}
void DataMapper::CloseShop()
{
	if(m_inPort) // close old
	{
		::midiInClose(m_inPort) ;
		m_inPort = 0 ;
	}
}

void CALLBACK DataMapper::MonitorFunction( HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 )
{
	if(	MIM_DATA == wMsg )
		reinterpret_cast<DataMapper*>(dwInstance)->MonitorData(dwParam1, dwParam2) ;
	return ;
}
void DataMapper::MonitorData( DWORD dwParam1, DWORD dwParam2 )
{
	int channel = (0x0000000F & dwParam1) + 1 ;
	if( channel != 16 )
		return ;

	string function ;
	switch( 0x000000F0 & dwParam1)
	{
	case 0x80: function = "Note Off" ; break ;
	case 0x90: function = "Note On" ; break ;
	case 0xA0: function = "Poly Aft" ; break ;
	case 0xB0: function = "Ctrl/Mode Change" ; break ;
	case 0xC0: function = "Prog Change" ; break ;
	case 0xD0: function = "Chn Aft" ; break ;
	case 0xE0: function = "Pitch Wheel" ; break ;
	default: function = "Other" ; break ;
	}
	int value = (0x00FF0000 & dwParam1) / 0x00010000 ;
	string controller ;
	bool fire = false ;
	switch(0x0000FF00 & dwParam1)
	{
	case 0x6200: 
		controller = "NRPN MSB"; 
		m_currentMessage.nrpnMSB = value ;
		break ;
	case 0x6300: 
		controller = "NRPN LSB"; 
		m_currentMessage.nrpnLSB = value ;
		break ;
	case 0x0600: 
		controller = "DATA MSB"; 
		m_currentMessage.dataMSB = value ;
		break ;
	case 0x2600: 
		controller = "DATA LSB"; 
		m_currentMessage.dataLSB = value ;
		fire = true ;
		break ;
	default:
		controller = "Other" ;
	}

#if 0
	stringstream str ;
	str << "MIDI Message: Channel: " << channel 
		<< " Function: " << function
		<< " Controller: " << controller
		<< " Value: " << value 
		<< ". Raw Data ==>" << hex
		<< " Param1: " << dwParam1 
		<< " Param2: " << dwParam2 
		<< "\n" << ends ;
	::OutputDebugString(str.str().c_str()) ;
#endif
	if(fire)
		DigestMessage() ;
}
void DataMapper::DigestMessage()
{
#if 0
	stringstream str ;
	str << "StudioMix Message: "
		<< hex
		<< " NRPN: " << long(m_currentMessage.nrpnMSB) << ", " << long(m_currentMessage.nrpnLSB)
		<< " DATA: " << long(m_currentMessage.dataMSB) << ", " << long(m_currentMessage.dataLSB)
		<< "\n" << ends ;
	::OutputDebugString(str.str().c_str()) ;
#endif
	int controllerNumber = 0 ;
	switch( m_currentMessage.nrpnLSB )
	{
	case 0x3: // Top row knobs 1-8
		controllerNumber = m_currentMessage.nrpnMSB - 0x74 ;
		break ;
	case 0xB: // bottom row knobs 1-8
		controllerNumber = 9 +(m_currentMessage.nrpnMSB - 0x5C) ;
		break ;
	case 0x13: // buttons 1-8
		controllerNumber = 18 +(m_currentMessage.nrpnMSB - 0x44) ;
		break ;
	case 0x23: // Everything on the master strip
		controllerNumber = (m_currentMessage.nrpnMSB - 0x14)*9 + 8 ;
		break ;
	default:
		return ;
	}
	Direction whatsHappening = m_currentMessage.dataMSB ? GOING_DOWN : GOING_UP ;
	OnController(controllerNumber, whatsHappening) ;
}
void DataMapper::OnController(int controllerNumber /*0-26*/, Direction whatsHappening)
{
	// stringstream str ;
	// str << "StudioMix Controller: " << controllerNumber << (whatsHappening==GOING_UP ? " Going Up" : " Going Down") << "\n" << ends ;
	// ::OutputDebugString(str.str().c_str()) ;
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
	
	
	stringstream str ;
	str << "StudioMix Knob: " << knobNumber 
		<< (whatsHappening==GOING_UP ? " Going Up" : " Going Down") 
		<< (m_buttons[buttonNumber].engaged ? " (fine) " : " (normal) ")
		<< "Value: " << long(m_knobs[knobNumber].value)
		<< "\n" << ends ;
	::OutputDebugString(str.str().c_str()) ;

}

//	enum Direction {GOING_UP, GOING_DOWN} ;
