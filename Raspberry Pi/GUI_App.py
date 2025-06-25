from flask import Flask, render_template, request, jsonify, Response, stream_with_context
from threading import Thread
import queue, os, time, datetime

from CAN_Bus import CanBus
from Sensor_Listener import SensorListener
from Diagnostics_Client import DiagnosticsClient
from OTA_Client import OTAClient
from Server_Client import ServerClient
from Utils import SPEED_ID, POS_NEG_MESSAGES, DTC_DESCRIPTIONS


app = Flask(__name__)

# Initialize components
can            = CanBus()
listener       = SensorListener(channel='can0', bitrate=500000, queue_size=100)
diag_client    = DiagnosticsClient(can)
ota_client     = OTAClient(can)
server_client  = ServerClient()
listener.start()

# Queues for SSE
speed_queue = queue.Queue()
temp_queue  = queue.Queue()
dist_queue  = queue.Queue()
dtc_queue   = queue.Queue()
diag_queue  = queue.Queue()
ota_queue   = queue.Queue()

# Dispatch real-time speed into queue
def sensor_dispatcher():
    while True:
        msg = listener.can.recv()
        if not msg: continue
        arb_id, data = msg
        if arb_id == SPEED_ID:
            speed = data[0] | (data[1] << 8)
            speed_queue.put(speed)
Thread(target=sensor_dispatcher, daemon=True).start()

# Generic SSE generator
def sse_generator(q):
    while True:
        yield f"data: {q.get()}\n\n"

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

@app.route('/diag-stream')
def diag_stream():
    return Response(stream_with_context(sse_generator(diag_queue)), mimetype='text/event-stream')

@app.route('/ota-progress')
def ota_progress():
    return Response(stream_with_context(sse_generator(ota_queue)), mimetype='text/event-stream')

@app.route('/dtc-stream')
def dtc_stream():
    """
    Sends human-friendly DTC messages as JSON array of strings.
    """
    def gen():
        while True:
            entries = dtc_queue.get()  # list of dicts
            # stringify each entry
            lines = [
                f"{e['description']} at {e['time']} on {e['date']}"
                for e in entries
            ]
            yield f"data: {lines}\n\n"
    return Response(stream_with_context(gen()), mimetype='text/event-stream')

@app.route('/diagnostics', methods=['POST'])
def diagnostics():
    body = request.json or {}
    sid  = int(body.get('sid','0'), 16)
    param = body.get('param','0')

    # Dispatch to client and capture success flag
    if sid == 0x10:
        ok = diag_client.start_session(int(param))
    elif sid == 0x27:
        ok = diag_client.send_password(int(param))
    elif sid == 0x22:
        hi = int(param,16)
        ok = diag_client.read_data_by_id(hi) is not None
    elif sid == 0x19:
        dtc_codes = diag_client.read_dtc() or []
        # build entries with description & timestamp
        entries = []
        now = datetime.datetime.now()
        tstr = now.strftime("%H:%M:%S")
        dstr = now.strftime("%Y-%m-%d")
        for code in dtc_codes:
            desc = DTC_DESCRIPTIONS.get(code, "Unknown fault")
            entries.append({'description': desc, 'time': tstr, 'date': dstr})
        dtc_queue.put(entries)
        ok = True
    elif sid == 0x14:
        ok = diag_client.clear_dtc()
    elif sid == 0x34:
        ok = diag_client.request_download()
    else:
        ok = False

    # Push a friendly message
    msg = POS_NEG_MESSAGES.get(sid, {}).get(ok, f"Service 0x{sid:02X} {'succeeded' if ok else 'failed'}")
    diag_queue.put(msg)
    return ('',204)

@app.route('/ota/fetch')
def ota_fetch():
    available, version, url = server_client.check_for_update()
    return jsonify(available=available, version=version, url=url)

@app.route('/ota/start', methods=['POST'])
def ota_start():
    body = request.json or {}
    version = body.get('version')
    url     = body.get('url')
    if not version or not url:
        return ('Missing version or url', 400)

    path = server_client.download_firmware(version,url)
    if not path:
        ota_queue.put("Download failed")
        return ('',500)

    def run_upload():
        for prog in ota_client.upload(path):
            ota_queue.put(f"Update progress: {prog}%")
    Thread(target=run_upload, daemon=True).start()
    return ('',204)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, threaded=True)
