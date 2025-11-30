// Get DOM elements
const wifiForm = document.getElementById('wifiForm');
const ssidInput = document.getElementById('ssid');
const passwordInput = document.getElementById('password');
const messageDiv = document.getElementById('message');
const networkList = document.getElementById('networkList');
const refreshBtn = document.getElementById('refreshBtn');

// Show message to user
function showMessage(text, type = 'info') {
    messageDiv.textContent = text;
    messageDiv.className = 'message show ' + type;
}

// Hide message
function hideMessage() {
    messageDiv.className = 'message';
}

// Get signal strength category
function getSignalStrength(rssi) {
    if (rssi > -50) return 'strong';
    if (rssi > -70) return 'medium';
    return 'weak';
}

// Load available networks
async function loadNetworks() {
    networkList.innerHTML = '<p class="loading"><span class="spinner"></span></p>';
    
    try {
        const response = await fetch('/networks');
        if (!response.ok) throw new Error('Failed to fetch networks');
        
        const networks = await response.json();

        // Sort by signal strength
        networks.sort((a, b) => b.rssi - a.rssi);

        if (networks.length === 0) {
            networkList.innerHTML = '<p class="loading">No networks found</p>';
            return;
        }
        
        networkList.innerHTML = '';
        networks.forEach(network => {
            const item = document.createElement('div');
            item.className = 'network-item';
            
            const nameSpan = document.createElement('span');
            nameSpan.className = 'network-name';
            nameSpan.textContent = network.ssid;
            
            const signalSpan = document.createElement('span');
            signalSpan.className = 'network-signal ' + getSignalStrength(network.rssi);
            signalSpan.textContent = network.rssi + ' dBm';
            
            item.appendChild(nameSpan);
            item.appendChild(signalSpan);
            
            item.addEventListener('click', () => {
                ssidInput.value = network.ssid;
                passwordInput.focus();
            });
            
            networkList.appendChild(item);
        });
    } catch (error) {
        networkList.innerHTML = '<p class="loading">Error loading networks</p>';
        console.error('Error:', error);
    }
}

// Handle form submission
wifiForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    
    const ssid = ssidInput.value.trim();
    const password = passwordInput.value;
    
    if (!ssid) {
        showMessage('Please enter a network name', 'error');
        return;
    }
    
    showMessage('Connecting to ' + ssid + '...', 'info');
    
    try {
        const response = await fetch('/connect', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ ssid, password })
        });
        
        const result = await response.json();
        
        if (result.success) {
            showMessage('Connected successfully! Redirecting...', 'success');
            setTimeout(() => {
                window.location.href = '/';
            }, 2000);
        } else {
            showMessage('Connection failed: ' + (result.message || 'Unknown error'), 'error');
        }
    } catch (error) {
        showMessage('Error connecting: ' + error.message, 'error');
        console.error('Error:', error);
    }
});

// Refresh networks button
refreshBtn.addEventListener('click', loadNetworks);

// Load networks on page load
loadNetworks();

// Auto-refresh networks every 30 seconds
setInterval(loadNetworks, 30000);
