# gui_app.py

from flask          import Flask, render_template, request, jsonify, Response, stream_with_context
from threading      import Thread
import queue, os, requests, time

from CAN_Bus import CanBus
from Sensor_Listener import xSensorQueue
from Diagnostics_Client import DiagnosticsClient
from OTA_Client import OTAClient
from Server_client import ServerClient
from Utils import (
    SPEED_ID,
    DTC_ID_TEMP,
    DTC_ID_DIST,
    SID_DIAG_SESSION_CTRL,
    SID_SECURITY_ACCESS,
    SID_READ_DATA_BY_ID,
    SID_READ_DTC,
    SID_CLEAR_DTC,
    SID_REQUEST_DOWNLOAD,
)

app = Flask(__name__)

# Initialize components
can           = CanBus()
listener      = SensorListener(channel='can0', bitrate=500000, queue_size=100)
diag_client   = DiagnosticsClient(can)
ota_client    = OTAClient(can)
server_client = ServerClient()

# Start sensor listener thread
listener.start()

# Queues for streaming events
speed_queue = queue.Queue()
temp_queue  = queue.Queue()
dist_queue  = queue.Queue()
dtc_queue   = queue.Queue()
diag_queue  = queue.Queue()
ota_queue   = queue.Queue()

# Dispatcher: route CAN messages to appropriate queues
def sensor_dispatcher():
    while True:
        msg = listener.can.recv()
        if not msg:
            continue
        arb_id, data = msg
        if arb_id == SPEED_ID:
            speed = data[0] | (data[1] << 8)
            speed_queue.put(speed)

# Start dispatcher thread
Thread(target=sensor_dispatcher, daemon=True).start()\ nThread(target=sensor_dispatcher, daemon=True).start()

# Routes
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/speed-stream')
def speed_stream():
    return Response(stream_with_context(sse_generator(speed_queue)), mimetype='text/event-stream')

@app.route('/temp-stream')
def temp_stream():
    return Response(stream_with_context(sse_generator(temp_queue)), mimetype='text/event-stream')

@app.route('/dist-stream')
def dist_stream():
    return Response(stream_with_context(sse_generator(dist_queue)), mimetype='text/event-stream')

@app.route('/dtc-stream')
def dtc_stream():
    return Response(stream_with_context(sse_generator(dtc_queue)), mimetype='text/event-stream')

@app.route('/diag-stream')
def diag_stream():
    return Response(stream_with_context(sse_generator(diag_queue)), mimetype='text/event-stream')

@app.route('/ota-progress')
def ota_progress():
    return Response(stream_with_context(sse_generator(ota_queue)), mimetype='text/event-stream')

@app.route('/diagnostics', methods=['POST'])
def diagnostics():
    body = request.json or {}
    sid   = int(body.get('sid', '0'), 16)
    param = body.get('param', '0')

    if sid == SID_DIAG_SESSION_CTRL:
        ok = diag_client.start_session(int(param))
        diag_queue.put(f"Session set: {ok}")

    elif sid == SID_SECURITY_ACCESS:
        ok = diag_client.send_password(int(param))
        diag_queue.put(f"Security unlocked: {ok}")

    elif sid == SID_READ_DATA_BY_ID:
        identifier = int(param, 16)
        data = diag_client.read_data_by_id(identifier) or b'\x00\x00'
        if identifier == SENSOR_TEMP_ID:
            temp_queue.put(data[0] | (data[1] << 8))
        elif identifier == SENSOR_DIST_ID:
            dist_queue.put(data[0] | (data[1] << 8))
        else:
            dtc_queue.put(data)

    elif sid == SID_READ_DTC:
        dtcs = diag_client.read_dtc() or []
        dtc_queue.put(dtcs)

    elif sid == SID_CLEAR_DTC:
        ok = diag_client.clear_dtc()
        diag_queue.put(f"Clear DTC: {ok}")

    elif sid == SID_REQUEST_DOWNLOAD:
        ok = diag_client.request_download()
        diag_queue.put(f"Download mode: {ok}")

    return ('', 204)

@app.route('/ota/fetch')
def ota_fetch():
    available, version, url = server_client.check_for_update()
    return jsonify(available=available, version=version, url=url)

@app.route('/ota/start', methods=['POST'])
def ota_start():
    body    = request.json or {}
    version = body.get('version')
    url     = body.get('url')
    if not version or not url:
        return ('Missing version or url', 400)

    path = server_client.download_firmware(version, url)
    if not path:
        ota_queue.put("Download failed")
        return ('', 500)

    def run_upload():
        for prog in ota_client.upload(path):
            ota_queue.put(prog)
    Thread(target=run_upload, daemon=True).start()
    return ('', 204)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, threaded=True)
