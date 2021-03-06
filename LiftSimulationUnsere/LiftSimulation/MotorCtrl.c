/*
 * MotorCtrl.c
 *
 * Created: 09.12.2019 09:52:43
 *  Author: xxx
 */ 

#include "AppIncludes.h"
#include "LiftSimulationCommon.h"

void Await_DoorClosed(Message* msg);
void Await_DoorOpen(Message* msg);
void Message_ElevatorReady(Message* msg);

MotorController _motorCtrl =
{
	.start = Floor2,
	.target = Floor0,
	.fsm  = { .Next = 0, .CurrentState = MotorCtrl_Initializing, .RxMask = 0xFF },
};

void NotifyCalibrationDone(uint8_t currentPos, uint8_t targetPostion)
{
	FloorType floor = (FloorType)currentPos/16;
	SetDisplay(floor);
	if( ((currentPos %floor) == 0 ) && floor == Floor0 )
	{
		SendEvent(SignalSourceEnvironment, LiftCalibrated, currentPos, targetPostion);
	}
}

void MotorCtrl_Initializing(Message* msg)
{
	if( msg->Id == LiftStarted)
	{
		CalibrateElevatorPosition(NotifyCalibrationDone);
		return;
	}
	if( msg->Id == LiftCalibrated )
	{
		SetDisplay(Floor0);
		SetState(&_motorCtrl.fsm, MotorCtrl_Stopped);
		SendEvent(SignalSourceApp, Message_ElevatorReady, Floor0, 0);
	}
}

void OnElevatorPositionChanged(uint8_t currentPos, uint8_t targetPos)
{
	SendEvent(SignalSourceElevator, Message_PosChanged, currentPos, targetPos);
}


void MotorCtrl_Stopped(Message* msg)
{
	if( msg->Id == Message_MoveTo && msg->MsgParamLow < 4)
	{
		_motorCtrl.target = (FloorType)msg->MsgParamLow;
		SetDoorState(DoorClosed, _motorCtrl.start);
		SetState(&_motorCtrl.fsm, Await_DoorClosed);
	}
}


void MotorCtrl_Moving(Message* msg)
{
	SetDisplay((FloorType)msg->MsgParamLow / POS_STEPS_PER_FLOOR);
	if( msg->Id == Message_PosChanged && msg->MsgParamLow == msg->MsgParamHigh)
	{
		_motorCtrl.target = (FloorType)msg->MsgParamLow / POS_STEPS_PER_FLOOR;
		SetState(&_motorCtrl.fsm, Await_DoorOpen);
		_motorCtrl.start = msg->MsgParamLow / POS_STEPS_PER_FLOOR;
		SetDoorState(DoorOpen, _motorCtrl.target);
		
	}	
}

void Await_DoorOpen(Message* msg)
{
	if(msg->Id == LiftDoorEvent && msg->MsgParamLow == DoorOpen)
	{
		Usart_PutChar(_motorCtrl.start);
		SetState(&_motorCtrl.fsm, MotorCtrl_Stopped);
		Message_ElevatorReady(msg);
	}
}

void Await_DoorClosed(Message* msg)
{
	if (ReadDoorState((FloorType)_motorCtrl.start) == DoorClosed)
	{
		SetState(&_motorCtrl.fsm, MotorCtrl_Moving);
		MoveElevator(_motorCtrl.target * POS_STEPS_PER_FLOOR, OnElevatorPositionChanged );
	}
}
