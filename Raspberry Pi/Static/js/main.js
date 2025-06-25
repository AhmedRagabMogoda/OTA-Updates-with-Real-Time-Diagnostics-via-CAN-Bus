document.addEventListener('DOMContentLoaded', () => {
    const speedDisplay = document.getElementById('speed-display');
    const diagService = document.getElementById('diag-service');
    const diagParamSelect = document.getElementById('diag-param-select');
    const diagParamInput = document.getElementById('diag-param-input');
    const btnDiagnostics = document.getElementById('btn-diagnostics');
    const diagResults = document.getElementById('diag-results');
    const tempDisplay = document.getElementById('temp-display');
    const distDisplay = document.getElementById('dist-display');
    const dtcCard = document.getElementById('dtc-card');
    const dtcDisplay = document.getElementById('dtc-display');
    const btnCheckUpdate = document.getElementById('btn-check-update');
    const btnUpdate = document.getElementById('btn-update');
    const otaProgress = document.getElementById('ota-progress');
    const otaStatus = document.getElementById('ota-status');
    let dtcHideTimeout = null;

    // Human‐readable sub‐function maps
    const subOptions = {
        '0x10': [  // Session Control
            { value: '0x00', text: 'Session Default' },
            { value: '0x01', text: 'Session Sensor' },
            { value: '0x02', text: 'Session Control' },
            { value: '0x03', text: 'Session Programming' }
        ],
        '0x22': [  // Read Data By ID (example IDs)
            { value: '0xF190', text: 'Read Temperature DID' },
            { value: '0xF191', text: 'Read Distance DID' }
        ],
        // Add other services here if they need dropdown params
    };

    // Show the right param widget: dropdown for mapped services,
    // password input for security, or none.
    function refreshParamWidget() {
        const svc = diagService.value;
        if (svc === '0x27') {
            // Security Access → show password input
            diagParamSelect.classList.add('hidden');
            diagParamInput.classList.remove('hidden');
            diagParamInput.value = '';
        } else if (subOptions[svc]) {
            // Mapped sub‐function dropdown
            diagParamInput.classList.add('hidden');
            diagParamSelect.classList.remove('hidden');
            // Populate the dropdown
            diagParamSelect.innerHTML = '';
            subOptions[svc].forEach(opt => {
                const el = document.createElement('option');
                el.value = opt.value;
                el.textContent = opt.text;
                diagParamSelect.appendChild(el);
            });
        } else {
            // No parameter needed
            diagParamSelect.classList.add('hidden');
            diagParamInput.classList.add('hidden');
        }
    }

    diagService.addEventListener('change', refreshParamWidget);
    refreshParamWidget();  // initialize on load

    // --- SSE handlers (speed, diag log, temp, dist, dtc, ota) ---
    new EventSource('/speed-stream').onmessage = e => {
        speedDisplay.textContent = `${e.data} km/h`;
    };
    new EventSource('/diag-stream').onmessage = e => {
        diagResults.textContent += e.data + '\n';
        diagResults.scrollTop = diagResults.scrollHeight;
    };
    new EventSource('/temp-stream').onmessage = e => {
        tempDisplay.textContent = `${e.data} °C`;
    };
    new EventSource('/dist-stream').onmessage = e => {
        distDisplay.textContent = `${e.data} cm`;
    };
    new EventSource('/dtc-stream').onmessage = e => {
        const msgs = JSON.parse(e.data);
        dtcDisplay.textContent = msgs.join('\n');
        dtcCard.classList.remove('hidden');
        clearTimeout(dtcHideTimeout);
        dtcHideTimeout = setTimeout(() => dtcCard.classList.add('hidden'), 60_000);
    };
    new EventSource('/ota-progress').onmessage = e => {
        const p = parseInt(e.data, 10);
        otaProgress.value = p;
        otaStatus.textContent = `Update: ${p}%`;
        if (p >= 100) {
            otaStatus.textContent = 'Update completed successfully!';
            btnUpdate.disabled = true;
        }
    };

    // --- Button handlers ---
    btnDiagnostics.addEventListener('click', () => {
        let param;
        if (diagService.value === '0x27') {
            param = diagParamInput.value;
        } else if (!diagParamSelect.classList.contains('hidden')) {
            param = diagParamSelect.value;
        } else {
            param = '';  // no param needed
        }

        fetch('/diagnostics', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                sid: diagService.value,
                param: param
            })
        });
    });

    btnCheckUpdate.addEventListener('click', async () => {
        otaStatus.textContent = 'Checking...';
        const res = await fetch('/ota/fetch');
        const data = await res.json();
        if (data.available) {
            otaStatus.textContent = `Firmware v${data.version} available`;
            btnUpdate.disabled = false;
            btnUpdate.dataset.url = data.url;
            btnUpdate.dataset.version = data.version;
        } else {
            otaStatus.textContent = 'No update available';
        }
    });

    btnUpdate.addEventListener('click', () => {
        otaStatus.textContent = 'Starting update...';
        btnUpdate.disabled = true;
        fetch('/ota/start', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                version: btnUpdate.dataset.version,
                url: btnUpdate.dataset.url
            })
        });
    });
});
