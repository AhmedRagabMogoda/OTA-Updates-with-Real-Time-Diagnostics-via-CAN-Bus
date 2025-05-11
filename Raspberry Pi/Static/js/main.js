// main.js

document.addEventListener('DOMContentLoaded', () => {
    const speedDisplay = document.getElementById('speed-display');

    const diagSection = document.getElementById('diag-section');
    const diagService = document.getElementById('diag-service');
    const diagParam = document.getElementById('diag-param');
    const btnDiagnostics = document.getElementById('btn-diagnostics');
    const diagResults = document.getElementById('diag-results');

    const tempDisplay = document.getElementById('temp-display');
    const distDisplay = document.getElementById('dist-display');
    const dtcDisplay = document.getElementById('dtc-display');

    const btnCheckUpdate = document.getElementById('btn-check-update');
    const btnUpdate = document.getElementById('btn-update');
    const otaProgress = document.getElementById('ota-progress');
    const otaStatus = document.getElementById('ota-status');

    let diagTimeoutHandle = null;

    // Helper to show diagnostics panel and reset hide-timer
    function showDiagnostics() {
        diagSection.classList.remove('hidden');
        if (diagTimeoutHandle) clearTimeout(diagTimeoutHandle);
        diagTimeoutHandle = setTimeout(() => {
            diagSection.classList.add('hidden');
        }, 30000); // hide after 30s inactivity
    }

    // SSE: Speed
    new EventSource('/speed-stream').onmessage = e => {
        speedDisplay.textContent = `${e.data} km/h`;
    };

    // SSE: Diagnostics raw results
    new EventSource('/diag-stream').onmessage = e => {
        diagResults.textContent += e.data + '\n';
        diagResults.scrollTop = diagResults.scrollHeight;
        showDiagnostics();
    };

    // SSE: Temperature
    new EventSource('/temp-stream').onmessage = e => {
        tempDisplay.textContent = e.data;
        showDiagnostics();
    };

    // SSE: Distance
    new EventSource('/dist-stream').onmessage = e => {
        distDisplay.textContent = e.data;
        showDiagnostics();
    };

    // SSE: DTCs
    new EventSource('/dtc-stream').onmessage = e => {
        dtcDisplay.textContent = e.data;
        showDiagnostics();
    };

    // SSE: OTA progress
    new EventSource('/ota-progress').onmessage = e => {
        const p = parseInt(e.data, 10);
        otaProgress.value = p;
        otaStatus.textContent = `Update: ${p}%`;
        if (p >= 100) {
            otaStatus.textContent = 'Update completed successfully!';
            btnUpdate.disabled = true;
        }
    };

    // Diagnostics button
    btnDiagnostics.addEventListener('click', () => {
        fetch('/diagnostics', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                sid: diagService.value,
                param: diagParam.value
            })
        });
        showDiagnostics();
    });

    // Check for update
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

    // Start update
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
