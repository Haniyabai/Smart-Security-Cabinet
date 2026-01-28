# Smart-Security-Cabinet

The **Smart Cabinet** is an interactive automation system designed to monitor cabinet status, detect activity, and provide user control through a mobile application.  
* This system enhances security, convenience, and automation using sensors and Bluetooth communication.
## Components Used
- Arduino UNO  
- Ultrasonic Sensor  
- Servo Motor  
- HC-05 Bluetooth Module  
- Laser Module  
- LDR Sensor  
- Buzzer  
- OLED Display  
- MIT App Inventor Mobile App
## System Features
- Cabinet Status Monitoring (Open/Closed)
- Motion & Presence Detection
- Laser–LDR Intrusion Detection
- Mobile App Control via Bluetooth
- Live OLED Display Feedback
- Alarm Notification using Buzzer
- Auto Servo Door Control
## Working Principle
* Ultrasonic sensor measures distance inside the cabinet.
* If the distance changes, system detects cabinet activity.
* LDR detects laser interruption → triggers security event.
* Servo operates gate/cabinet door on command or sensor logic.
* Bluetooth receives commands from MIT App Inventor app.
* OLED display shows real-time system status.
* Buzzer alerts on intrusion or abnormal activity.
