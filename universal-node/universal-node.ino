#include <ESP8266WiFi.h>

#include <DHT.h>

#include <NeoPixelBus.h>
#include <RgbColor.h>

#include <MQTT.h>
#include <PubSubClient.h>

#define pixelCount 63

//const char* ssid     = "Freifunk";
//const char* password = "";
const char* ssid     = "1084059";
const char* password = "2415872658287010";
String client_id;
const char* mqtt_password = "foo";
IPAddress server(10,1,0,15);
WiFiClient wclient;
PubSubClient client(wclient, server);
NeoPixelBus leds = NeoPixelBus(pixelCount, 0);
uint16_t effectState = 0;

//DHT dht(2,DHT22,0);

class DHTReader
{
private:
  unsigned long prev_millis;
  unsigned long time;
  DHT dht;
public:
  DHTReader(unsigned char pin, unsigned long speed): dht(pin,DHT22,0), time(speed)
  {
    dht.begin(); 
  }
  void update()
  {
    unsigned long curr_millis = millis();
    if(curr_millis-prev_millis>=time)
    {
      prev_millis=curr_millis;
      float temp=dht.readTemperature();
      float humi=dht.readHumidity();
      Serial.print(temp);
      Serial.print(" Humidity: ");
      Serial.println(humi);
      
      String s="{sensor:\"node-10011266\", temperature:"+String(temp)+" , humidity: "+String(humi)+"}";
      if(client.connected())
        client.publish("Wohnung/Wohnzimmer",s);
    }
  }
};

DHTReader reader(2,10000);

void SetRandomSeed()
{
  uint32_t seed;
  
  // random works best with a seed that can use 31 bits
  // analogRead on a unconnected pin tends toward less than four bits
  seed = analogRead(0);
  delay(1);
  
  for (int shifts = 3; shifts < 31; shifts += 3)
  {
    seed ^= analogRead(0) << shifts;
    delay(1);
  }
  
  // Serial.println(seed);
  randomSeed(seed);
}

void FadeInFadeOutRinseRepeat(uint8_t peak)
{
  if (effectState == 0)
  {
    for (uint8_t pixel = 0; pixel < pixelCount; pixel++)
    {
      uint16_t time = random(800,1000);
      leds.LinearFadePixelColor(time, pixel, RgbColor(random(peak), random(peak), random(peak)));
    }
  }
  else if (effectState == 1)
  {
    for (uint8_t pixel = 0; pixel < pixelCount; pixel++)
    {
      uint16_t time = random(600,700);
      leds.LinearFadePixelColor(time, pixel, RgbColor(0, 0, 0));
    }
  }
  effectState = (effectState + 1) % 2; // next effectState and keep within the number of effectStates
  
}

void mqtt_cb(const MQTT::Publish &pub)
{
  String topic=pub.topic();
  Serial.print("Got message in topic '");
  Serial.print(topic);
  Serial.print("': ");
  Serial.print(pub.payload_string());
  Serial.print(" (");
  Serial.print(pub.payload_len());
  Serial.println(")");
  if(topic=="Control/LED")
  {
    uint32_t length=pub.payload_len();
    if(length>=1)
    {
      uint8_t *payload=pub.payload();
      uint8_t cmd=*payload;
      payload++;
      length--;
      if(cmd=='A')
      {
        if(length==3)
        {
          RgbColor color(
            *(payload+0),
            *(payload+1),
            *(payload+2)
          );
          leds.ClearTo(color);
          leds.Show();
        }
      }
      else if(cmd=='B')
      {
        if(length==4)
        {
          uint8_t speed=*payload;
          payload++;
          RgbColor color(
            *(payload+0),
            *(payload+1),
            *(payload+2)
          );
          leds.FadeTo(10*speed,color);
        }
      }
      else if(cmd='C')
      {
        leds.ClearTo(RgbColor(0));
        leds.Show();
      }
    }
  }
}

void setup() {
  client_id="node-"+String(ESP.getChipId());

  Serial.begin(115200);
  delay(10);
  Serial.println();

  SetRandomSeed();

  Serial.print("Client-Id: ");
  Serial.println(client_id);
  Serial.print("Connecting to ");
  Serial.print(ssid);

  leds.Begin();
  //leds.ClearTo(0,0,0);
  leds.Show();
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Connecting to MQTT");
  if(client.connect(MQTT::Connect(client_id).set_auth(client_id,mqtt_password)))
  {
    Serial.println("Connected to MQTT");
    client.set_callback(mqtt_cb);
    client.subscribe(MQTT::Subscribe()
      .add_topic("Control/Global")
      .add_topic("Control/LED")
    );
  }
}

void loop() {
  client.loop();
  reader.update();
  
  //FadeInFadeOutRinseRepeat(192);
  //PickRandom(128);
  
  // start animating
  leds.StartAnimating();
  
  // wait until no more animations are running
  while (leds.IsAnimating())
  {
    leds.UpdateAnimations();
    leds.Show();
    delay(31); // ~30hz change cycle
  }
}

