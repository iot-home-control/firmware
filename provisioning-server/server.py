from flask import Flask, request, make_response, send_from_directory

from ZeroconfService import ZeroconfService

zeroconf_service = ZeroconfService(name="ESP8266 Provisioning", port=5000, stype="_esp8266_provision._tcp")
app = Flask(__name__)

"""
X-Esp8266-Sta-Mac: 18:FE:34:98:C2:82
X-Esp8266-Version: 1
X-Esp8266-Mode: sketch
X-Esp8266-Sdk-Version: 1.5.2(7eee54f4)
User-Agent: ESP8266-http-Update
X-Esp8266-Ap-Mac: 1A:FE:34:98:C2:82
X-Esp8266-Sketch-Size: 262900
X-Esp8266-Chip-Size: 4194304
X-Esp8266-Free-Space: 2879488
"""

modules = ["18:FE:34:98:C2:82"]
esp_headers = [
    "X-Esp8266-Sta-Mac",
    "X-Esp8266-Version",
    "X-Esp8266-Mode",
    "X-Esp8266-Sdk-Version",
    "X-Esp8266-Ap-Mac",
    "X-Esp8266-Sketch-Size",
    "X-Esp8266-Chip-Size",
    "X-Esp8266-Free-Space"
]


@app.route("/firmware.bin")
def handle_firmware_update():

    for h in esp_headers:
        if h not in request.headers:
            return make_response("403 Not allowed", 403)

    if request.headers["X-Esp8266-Sta-Mac"] not in modules:
        return make_response("403 Not allowed", 403)

     if False: # request.headers["X-Esp8266-Version"] < 0:
         return make_response("304 Not Modified", 304)
     else:
        return send_from_directory("", "firmware.bin", mimetype="application/octet-stream", as_attachment=True)


if __name__ == "__main__":
    zeroconf_service.publish()
    app.debug = True
    app.run(host="0.0.0.0")
