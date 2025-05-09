# gui_app.py

from flask import Flask, render_template, request, jsonify, stream_with_context, Response
from threading import Thread
import queue
import time

from CAN_Bus import CanBus
from Sensor_Listener import xSensorQueue
from Diagnostics_Client import DiagnosticsClient
from OTA_Client import OTAClient
from Utils import DEFAULT_TIMEOUT

app = Flask(__name__)

# Shared CAN bus
can = CanBus()
# Clients
diag_client = DiagnosticsClient(can)
ota_client = OTAClient(can)

# Queues for SSE
speed_queue = queue.Queue()
diag_queue = queue.Queue()
ota_queue = queue.Queue()

# Background: forward sensor speeds to SSE
def speed_publisher():
    while True:
        msg = xSensorQueue.get()
        arb_id, data = msg
        if arb_id == SPEED_ID:
            speed = data[0] | (data[1] << 8)
            speed_queue.put(speed)

# Background: forward diag responses to SSE
def diag_publisher():
    while True:
        # results are pushed into diag_queue by client functions
        data = diag_queue.get()
        yield f"data: {data}\n\n"

# Background: forward ota progress to SSE
def ota_publisher():
    while True:
        progress = ota_queue.get()
        yield f"data: {progress}\n\n"

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/diagnostics', methods=['POST'])
def diagnostics():
    body = request.json
    sid = int(body.get('sid'), 16)
    param = body.get('param')
    # Call appropriate service
    if sid == SID_DIAG_SESSION_CTRL:
        ok = diag_client.start_session(int(param))
        diag_queue.put(f"Session set: {ok}")
    elif sid == SID_SECURITY_ACCESS:
        ok = diag_client.send_password(int(param))
        diag_queue.put(f"Security: {ok}")
    elif sid == SID_READ_DATA_BY_ID:
        data = diag_client.read_data_by_id(int(param,16))
        diag_queue.put(f"Data: {data}")
    elif sid == SID_READ_DTC:
        dtcs = diag_client.read_dtc()
        diag_queue.put(f"DTCs: {dtcs}")
    elif sid == SID_CLEAR_DTC:
        ok = diag_client.clear_dtc()
        diag_queue.put(f"Clear DTC: {ok}")
    elif sid == SID_REQUEST_DOWNLOAD:
        # Trigger OTA via bootloader
        diag_client.start_session(SESSION_PROGRAMMING)
        diag_queue.put("Update requested")
    return ('', 204)

@app.route('/speed-stream')
def speed_stream():
    def event_stream():
        while True:
            speed = speed_queue.get()
            yield f"data: {speed}\n\n"
    return Response(stream_with_context(event_stream()), mimetype='text/event-stream')

@app.route('/diag-stream')
def diag_stream():
    return Response(stream_with_context(diag_publisher()), mimetype='text/event-stream')

@app.route('/ota-progress')
def ota_progress():
    return Response(stream_with_context(ota_publisher()), mimetype='text/event-stream')

@app.route('/ota/fetch')
def ota_fetch():
    # placeholder: check server for new firmware
    # return {"available": True, "version": "1.2.3
