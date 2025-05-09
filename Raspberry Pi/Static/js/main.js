
document.addEventListener('DOMContentLoaded', () => {
    const speedDisplay = document.getElementById('speed-display');
    const diagService = document.getElementById('diag-service');
    const diagParam = document.getElementById('diag-param');
    const btnDiagnostics = document.getElementById('btn-diagnostics');
    const diagResults = document.getElementById('diag-results');
    const btnCheckUpdate = document.getElementById('btn-check-update');
    const btnUpdate = document.getElementById('btn-update');
    const otaProgress = document.getElementById('ota-progress');
    const otaStatus = document.getElementById('ota-status');

    // SSE for live speed
    const speedSource = new EventSource('/speed-stream');
    speedSource.onmessage = e => {
        speedDisplay.textContent = `${e.data} km/h`;
    };

    // SSE for diagnostics results
    const diagSource = new EventSource('/diag-stream');
    diagSource.onmessage = e => {
        diagResults.textContent += e.data + '\n';
        diagResults.scrollTop = diagResults.scrollHeight;
    };

    // SSE for OTA progress
    const otaSource = new EventSource('/ota-progress');
    otaSource.onmessage = e => {
        const progress = parseInt(e.data, 10);
        otaProgress.value = progress;
        otaStatus.textContent = `Update: ${progress}%`;
        if (progress >= 100) {
            otaStatus.textContent = 'Update completed successfully!';
            btnUpdate.disabled = true;
        }
    };

    // Diagnostics button
    btnDiagnostics.addEventListener('click', () => {
        diagResults.textContent = '';
        const sid = diagService.value;
        const param = diagParam.value;
        fetch('/diagnostics', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ sid, param })
        });
    });

    // Check for update
    btnCheckUpdate.addEventListener('click', async () => {
        otaStatus.textContent = 'Checking for update...';
        const res = await fetch('/ota/fetch');
        const data = await res.json();
        if (data.available) {
            otaStatus.textContent = `Firmware v${data.version} available`;
            btnUpdate.disabled = false;
        } else {
            otaStatus.textContent = 'No update available';
        }
    });

    // Start update
    btnUpdate.addEventListener('click', () => {
        otaStatus.textContent = 'Starting update...';
        fetch('/ota/start', { method: 'POST' });
        btnUpdate.disabled = true;
    });
});
