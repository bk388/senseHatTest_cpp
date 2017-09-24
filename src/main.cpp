//============================================================================
// Name        : main.cpp
// Author      : Bence
// Version     :
// Copyright   : 
// Description : Main function for the project piVR
//============================================================================

#include "RTIMULib.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int main(int argc, char* argv[]) {
    RTIMUSettings *settings = new RTIMUSettings("RTIMULib");

    RTIMU *imu = RTIMU::createIMU(settings);
    RTPressure *pressure = RTPressure::createPressure(settings);
    RTHumidity *humidity = RTHumidity::createHumidity(settings);

    if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
        printf("No IMU found\n");
        exit(1);
    }

    imu->IMUInit();

    imu->setSlerpPower(0.02);
    imu->setGyroEnable(true);
    imu->setAccelEnable(true);
    imu->setCompassEnable(true);

    while(1) {
        usleep(imu->IMUGetPollInterval() * 1000);
        while (imu->IMURead()) {
            RTIMU_DATA imuData = imu->getIMUData();
            //printf("%s\n", RTMath::displayDegrees("", imuData.fusionPose));
	    printf("roll: %f\n", imuData.fusionPose.x());
        }
    }
}

