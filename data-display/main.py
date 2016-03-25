#!/usr/bin/env python3

from flask import Flask, render_template, make_response
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
from matplotlib.dates import DateFormatter
import matplotlib.pylab as plt
from io import BytesIO
import datetime
import random
import config
from shared import db
from database import DataType, DataPoint

app = Flask(__name__)
app.config.from_object(config)
db.init_app(app)


@app.route('/img/data_recent.png')
def data_recent_img():
    #plt.xkcd()
    fig = Figure()
    a_temp = fig.add_subplot(111)
    a_temp.set_ylabel('Temperature [°C]', color='b')
    a_temp.set_ylim([-10, 50])
    a_humi = a_temp.twinx()
    a_humi.set_ylabel('Humidity [%]', color='r')
    a_humi.set_ylim([0, 100])
    x_temp = []
    x_humi = []
    y_temp = []
    y_humi = []
    dt=datetime.datetime.nowdate-datetime.time.timedelta(hours=1)
    dps_temp = db.session.query(DataPoint).filter(data_type=1, timestamp>=dt)
    dps_humi = db.session.query(DataPoint).filter(data_type=2, timestamp>=dt)

    for p in dps_temp:
        x_temp.append(p.timestamp)
        y_temp.append(p.data/100)
    for p in dps_humi:
        x_humi.append(p.timestamp)
        y_humi.append(p.data/100)
    a_temp.plot_date(x_temp, y_temp, 'b-')
    a_humi.plot_date(x_humi, y_humi, 'r-')
    a_temp.xaxis.set_major_formatter(DateFormatter('%Y-%m-%d %H:%M:%S'))
    a_humi.xaxis.set_major_formatter(DateFormatter('%Y-%m-%d %H:%M:%S'))
    fig.autofmt_xdate()
    FigureCanvas(fig)
    png_output = BytesIO()
    fig.savefig(png_output, format='png')
    response = make_response(png_output.getvalue())
    response.headers['Content-Type'] = 'image/png'
    return response


@app.route('/')
def index():
    return render_template('index.html')

if __name__ == '__main__':
    app.run(debug=True)
