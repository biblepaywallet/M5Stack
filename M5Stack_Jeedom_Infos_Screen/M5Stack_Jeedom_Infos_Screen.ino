/*
 * IoT Display for Jeedom based on M5Stack dev platform.
 *
 *    Required hardware:
 * M5Stack core.
 *    Required library:
 * M5Stack : https://github.com/m5stack/M5Stack
 *
 *    Behavior:
 * The M5Stack will get the information for the Jeedom Server at a specified
 * rate (QUERY_TIME) and display them. The LCD is set to timeout after a certain
 * time set by LCD_TIMEOUT to keep the LCD from suffering of Burn-in image or
 * premature death of the backlight. You can still wake the LCD by pushing
 * the B button (middle) and it will shut itself off after the same amount of time.
 *
 * WIFI_HOSTNAME: The name broadcasted on the network
 * WIFI_SSID: Wifi network name.
 * WIFI_PASS: Wifi password.
 * JEEDOM_IP: The address of the Jeedom server.
 * JEEDOM_API: Your Jeedom API key.
 * QUERY_TIME: Time between GET command to Jeedom server (in seconds).
 * ID_TEMP: The command ID for the temperature.
 * ID_HUMI: The command ID for the humidity.
 * ID_LAMP: The command ID for the lamp luminosity.
 * LCD_BRIGHT: LCD Brightness (from 0 to 100%).
 * LCD_TIMEOUT: Time to keep the LCD alive (in seconds).
 * USE_BG: Use the background image on the MicroSD card.
 * BG_BLANK: Name of the blank background (for WiFi connexion).
 * BG_USE: Name of the used background (for values with icons).
*/

#define	WIFI_HOSTNAME		"hostname"
#define	WIFI_SSID			"wifi_ssid"
#define	WIFI_PASS			"wifi_pass"
#define	JEEDOM_IP			"jeedom_ip"
#define	JEEDOM_API			"jeedom_api"
#define QUERY_TIME			1
#define	ID_TEMP				639
#define	ID_HUMI				640
#define	ID_LAMP				53
#define	LCD_BRIGHT			100
#define	LCD_TIMEOUT			60
#define	USE_BG				1
#define	BG_BLANK			"/background.jpg"
#define	BG_USE				"/background_use.jpg"

#include <WiFi.h>
#include <M5Stack.h>
#include <HTTPClient.h>
HTTPClient http;

int		lcd = 1;
long	ptime = 0;
long	ltime = 0;
float	temp[] = {0, 0};
float	humi[] = {0, 0};
float	lamp[] = {0, 0};
String	BASEURL = "http://" + String(JEEDOM_IP) + "/core/api/jeeApi.php?apikey=" + String(JEEDOM_API) + "&type=cmd&id=";

void wifi_connexion()
{
	int x = 0;
	(USE_BG) ? M5.Lcd.drawJpgFile(SD, BG_BLANK) : M5.Lcd.fillScreen(BLACK);
	M5.Lcd.setTextSize(1);
	M5.Lcd.drawCentreString("Connecting to SSID:", 160, 40, 4);
	M5.Lcd.drawCentreString(WIFI_SSID, 160, 70, 4);
	M5.Lcd.setTextColor(WHITE);
	M5.Lcd.setTextSize(2);
	M5.Lcd.setCursor(0, 120);
	while (WiFi.status() != WL_CONNECTED && x++ <= 182)
	{
		M5.Lcd.print(".");
		delay(500);
	}
	if (WiFi.status() == WL_CONNECTED)
	{
		(USE_BG) ? M5.Lcd.drawJpgFile(SD, BG_BLANK) : M5.Lcd.fillScreen(BLACK);
		M5.Lcd.setTextSize(1);
		M5.Lcd.drawCentreString("CONNECTED", 160, 100, 4);
		delay(1000);
		update_values(1);
		print();
	}
	else
		wifi_connexion();
}

void setup()
{
	M5.begin();
	M5.Lcd.setBrightness(LCD_BRIGHT);
	WiFi.begin(WIFI_SSID, WIFI_PASS);
	WiFi.setHostname(WIFI_HOSTNAME);
	wifi_connexion();
}

void wake_me_up_before_you_go_go(int wake, int button)
{
	if (wake && !lcd && (ltime = millis()))
	{
		lcd = 1;
		if (button)
			print();
		for (int x = 0; x < LCD_BRIGHT; x++){
			M5.Lcd.setBrightness(x);
			delay(10);
		}
	}
	else if (lcd && (millis() - ltime > (LCD_TIMEOUT * 1000)))
	{
		lcd = 0;
		for (int x = LCD_BRIGHT; x >= 0; x--){
			M5.Lcd.setBrightness(x);
			delay(10);
		}
	}
}

int variation(float *n, int x, int y)
{
	uint16_t color = WHITE;
	String s = "=";
	if (n[0] > n[1] && (color = BLUE))
		s = "-";
	else if (n[0] < n[1]  && (color = RED))
		s  = "+";
	M5.Lcd.setTextColor(color);
	M5.Lcd.setTextSize(2);
	if (color != WHITE)
		M5.Lcd.drawCentreString(s, x, y - 10, 4);
	M5.Lcd.setTextSize(1);
	M5.Lcd.setTextColor(WHITE);
	if (color != WHITE)
		M5.Lcd.drawCentreString(String(n[1] - n[0]), x, y + 30, 2);
	return ((color == WHITE) ? (1) : (0));
}

int print()
{
	int offset = 0;
	(USE_BG) ? M5.Lcd.drawJpgFile(SD, BG_USE) : M5.Lcd.fillScreen(BLACK);
	M5.Lcd.setTextSize(1);
	offset = variation(temp, 95, 15);
	M5.Lcd.drawCentreString(String(temp[1]), (offset) ? 170 : 190, 15, 7);
	offset = variation(humi, 95, 85);
	M5.Lcd.drawCentreString(String(humi[1]), (offset) ? 170 : 190, 85, 7);
	offset = variation(lamp, 95, 155);
	M5.Lcd.drawCentreString(String((int)lamp[1]), (offset) ? 170 : 190, 155, 7);
	temp[0] = temp[1];
	humi[0] = humi[1];
	lamp[0] = lamp[1];
	return (1);
}

int update_values(int init)
{
	int tag = 0;
	http.begin(BASEURL + String(ID_TEMP));
	http.GET();
	if (((temp[1] = http.getString().toFloat()) != temp[0]))
		tag = 1;
	http.end();
	http.begin(BASEURL + String(ID_HUMI));
	http.GET();
	if (((humi[1] = http.getString().toFloat()) != humi[0]))
		tag = 1;
	http.end();
	http.begin(BASEURL + String(ID_LAMP));
	http.GET();
	if (((lamp[1] = http.getString().toFloat()) != lamp[0]))
		tag = 1;
	http.end();
	if (init)
	{
		temp[0] = temp[1];
		humi[0] = humi[1];
		lamp[0] = lamp[1];
	}
	return (tag);
}

void loop()
{
	if (WiFi.status() != WL_CONNECTED)
		wifi_connexion();
	wake_me_up_before_you_go_go(M5.BtnB.wasPressed() ? 1 : 0, 1);
	if (millis() - ptime > 1000)
	{
		if (update_values(0) && print())
			wake_me_up_before_you_go_go(1, 0);
		ptime = millis();
	}
	M5.update();
	delay(10);
}
