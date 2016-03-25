import paho.mqtt.client as paho
import time
import json
from collections import defaultdict
from datetime import datetime
import sqlalchemy
from database import *


mqtt_ip = "10.7.14.187"
data = defaultdict(list)
#engine = sqlalchemy.create_engine('sqlite:///data.db', echo=True)
engine = sqlalchemy.create_engine('sqlite:///data.db')
Base.metadata.create_all(engine)
Session = sqlalchemy.orm.sessionmaker(bind=engine)
session = Session()
temperature_type = session.query(DataType).filter_by(name='temperature').first()
humidity_type = session.query(DataType).filter_by(name='humidity').first()


def on_message(c, userdata, msg):
    dt = datetime.fromtimestamp(msg.timestamp)
    parts = msg.topic.split('/')
    if len(parts) == 2:
        if parts[0] == 'Wohnung':
            try:
                msg_str=msg.payload.decode('utf-8')
                obj = json.loads(msg_str)
                session.add_all([DataPoint(obj['sensor'], dt, int(obj['temperature'])*temperature_type.conversion_factor, temperature_type.id),
                                DataPoint(obj['sensor'], dt, int(obj['humidity'])*humidity_type.conversion_factor, humidity_type.id)])
                session.commit()
                print(dt, obj)
            except Exception as ex:
                print(ex)
                print(msg_str)
        else:
            print('Unknown topic part', parts[0])
    else:
        print('Unexpected topic', msg.topic)


def on_connect(c, userdata, flasg, rc):
    print("Connected to broker (rc={})".format(paho.connack_string(rc)))
    client.subscribe("Wohnung/Wohnzimmer")


def on_disconnect(client, userdata, rc):
    if rc != 0:
        print("Disconnected unexpectedly...")


if __name__ == "__main__":
    if temperature_type is None:
        temperature_type = DataType('temperature', 100, '°C')
        session.add(temperature_type)

    if humidity_type is None:
        humidity_type = DataType('humidity', 100, '%')
        session.add(humidity_type)

    session.commit()

    client = paho.Client(client_id="mobix220")
    client.username_pw_set("mobix220", "123456")
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_disconnect = on_disconnect
    client.connect(mqtt_ip, 1883, 60)
    client.loop_start()
    try:
        while True:
            time.sleep(0.1)
    except KeyboardInterrupt:
        pass
    client.loop_stop()
