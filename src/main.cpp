#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char *ssid = "";
const char *password = "";

#define LED_TIMER 500

// servo
#include <Servo.h>
Servo servo1;

int servo1Angle = 90;
int servo1PotPin = 0;
int servo1PotRead;

// bridge-h
int ENA = 4; // D2
int IN1 = 14; // D3
int IN2 = 13; // D4

void motorGoFront(int speed) {
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	analogWrite(ENA, speed);
}

void motorGoBack(int speed) {
	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, LOW);
	analogWrite(ENA, speed);
}

void motorStop() {
	analogWrite(ENA, 0);
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
}

void blinkLED() {
	static boolean ledstate = 0;
	static long unsigned int timer = 0;
	if(millis() >= timer) {
		timer = millis() + LED_TIMER;
		ledstate = !ledstate;
		digitalWrite(LED_BUILTIN, ledstate);

	}
}

void testTwo() {

	// turn on motors

	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);

	// accelerate from zero to maximum speed

	for (int i = 0; i < 256; i++)

	{
		analogWrite(ENA, i);
		delay(50);
		//blinkLED();
	}

	// decelerate from maximum speed to zero

	for (int i = 255; i >= 0; --i)
	{
		analogWrite(ENA, i);
		delay(50);
		//blinkLED();
	}

	// now turn off motors

	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
}

void setup() {

	pinMode(LED_BUILTIN, OUTPUT);
	
	pinMode(ENA, OUTPUT);
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);

	Serial.begin(115200);
	Serial.println("Booting");

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("Connection Failed! Rebooting...");
		delay(5000);
		ESP.restart();
	}

	// Port defaults to 8266
	// ArduinoOTA.setPort(8266);
	// Hostname defaults to esp8266-[ChipID]
	// ArduinoOTA.setHostname("myesp8266");
	// No authentication by default
	// ArduinoOTA.setPassword("admin");
	// Password can be set with it's md5 value as well
	// MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
	// ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

	ArduinoOTA.onStart([]() {

		String type;

		if (ArduinoOTA.getCommand() == U_FLASH) {
			type = "sketch";
		} else {
			// U_FS
			type = "filesystem";
		}

		// NOTE: if updating FS this would be the place to unmount FS using FS.end()
		Serial.println("Start updating " + type);
	});

	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});

	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});

	ArduinoOTA.onError([](ota_error_t error) {

		Serial.printf("Error[%u]: ", error);

		if (error == OTA_AUTH_ERROR) {
			Serial.println("Auth Failed");
		} else if (error == OTA_BEGIN_ERROR) {
			Serial.println("Begin Failed");
		} else if (error == OTA_CONNECT_ERROR) {
			Serial.println("Connect Failed");
		} else if (error == OTA_RECEIVE_ERROR) {
			Serial.println("Receive Failed");
		} else if (error == OTA_END_ERROR) {
			Serial.println("End Failed");
		} });

	ArduinoOTA.begin();

	Serial.println("Ready");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	// servo
	servo1.attach(12); //D6
	servo1.write(90);
}

void loop() {
	ArduinoOTA.handle();

	servo1PotRead = analogRead(servo1PotPin); // reads the value of the potentiometer (value between 0 and 1023)
	servo1Angle = map(servo1PotRead, 0, 1023, 0, 180);  // scale it to use it with the servo (value between 0 and 180)
	Serial.print(servo1PotRead);
	Serial.print("\t");
	Serial.println(servo1Angle);

	servo1.write(servo1Angle);	
	delay(50);

	//testTwo();
	motorGoFront(127);
	//blinkLED();
}