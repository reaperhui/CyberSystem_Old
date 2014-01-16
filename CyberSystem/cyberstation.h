#ifndef CyberStation_H
#define CyberStation_H



#include "inlib.h"
#include "RobonautControl.h"

#include <vhtIOConn.h>
#include <vhtTracker.h>
#include <vht6DofDevice.h>
#include <vhtGenHandModel.h>
#include <vhtCyberGlove.h>
#include <vhtCyberGrasp.h>
#include <vhtBaseException.h>

#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QThread>

struct CaliData
{
	double raw_pos[3];
	double real_pos[3];
	double raw_ori[3];
	double real_ori[3];
};

class CyberStation
{
private:
	typedef std::vector<CaliData> tracali_type;
	// rightTracker
	vhtIOConn *m_pRightTrackerConn;
	vhtTracker *m_pRightTracker;
	vht6DofDevice *m_pRightRcvr;
	// leftTracker
	vhtIOConn *m_pLeftTrackerConn;
	vhtTracker *m_pLeftTracker;
	vht6DofDevice *m_pLeftRcvr;
	// rightGrasp
	vhtIOConn *m_pRightGraspConn;
	vhtCyberGrasp *m_pRightGrasp;
	// leftGrasp
	vhtIOConn *m_pLeftGraspConn;
	vhtCyberGrasp *m_pLeftGrasp;
	// rightGlove
	vhtIOConn *m_pRightGloveConn;
	vhtCyberGlove *m_pRightGlove;
	// leftGlove
	vhtIOConn *m_pLeftGloveConn;
	vhtCyberGlove *m_pLeftGlove;


	// raw pose data and real pose data for tracker
	double rTrackerRawPose[6];
//	double rTrackerRealPose[6];
	double lTrackerRawPose[6];
//	double lTrackerRealPose[6];
	double rTrans[6];
	double lTrans[6];

	double m_rTraCoef[6];
	double m_lTraCoef[6];
	

	// raw data and real data for glove
	double m_rGloveRawData[5][4];
//	double m_rGloveRealData[5][3];
	double m_lGloveRawData[5][4];
//	double m_lGloveRealData[5][3];
	// calibration data for glove
 	double m_GesOneData[5][4];
	double m_GesTwoData[5][4];
	double m_GesThrData[5][4];
	double m_GesFourData[5][4];
// 	double m_TouchGrasp[5][4];
 	double m_GloCaliK[5][4];
 	double m_GloCaliB[5][4]; 

public:
	CyberStation();
	~CyberStation();
	// connect devices
	void RHandConn();
	void LHandConn();
	void RTraConn();
	void LTraConn();
	// display information for connection
	void DisplayMessage(const char *msg, vhtBaseException *e);

	// cyber glove calibration information 
	void GetGesOneData();
	void GetGesTwoData();
	void GetGesThrData();
	void GetGesFourData();
	// calculate two coefficient for cyber glove calibration
	void CalGloCoef();
	// get real cyberglove data 
	void RealGloData(bool);
	// get data from glove
	void GetRGloData();
	void GetLGloData();
	// decide which data(raw or real) to display
	QString GloDisData(bool);

	// cyber tracker calibraton information
	void GetRRawTraData(double arr[], int arr_size = 6);
	void GetLRawTraData(double arr[], int arr_size = 6);
	void CalTrackerCoef(tracali_type, tracali_type);
	void GetRealTraData();

	void GetRTraData();
	void GetLTraData();
	// calculate coefficient for cyber tracker calibration
	void CalTraCoef(bool, QString, QString, QString, QString, QString, QString);
	// decide which data(raw or real) to display
	QString RTraDisData(bool);
	QString LTraDisData(bool);

};
#endif