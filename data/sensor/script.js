lucide.createIcons();

function setData(temperatureC, temperatureF, humidity, pressure, altitude) {
    document.getElementById("temperature-c").textContent = temperatureC.toFixed(2);
    document.getElementById("temperature-f").textContent = temperatureF.toFixed(2);
    document.getElementById("humidity").textContent = humidity.toFixed(2);
    document.getElementById("pressure").textContent = pressure.toFixed(2);
    document.getElementById("altitude").textContent = altitude.toFixed(2);
}

function setSensorStatus(isOnline, ipAddress) {
    const statusElement = document.getElementById("sensor-status");
    if (isOnline) {
        statusElement.textContent = `Online (${ipAddress}) - Data refreshes every second.`;
        statusElement.classList.add("online");
    } else {
        statusElement.textContent = `Offline`;
        statusElement.classList.remove("online");
    }
}

function setUnit(unit) {
    switch (unit) {
        case "C":
            Array.from(document.getElementsByClassName("unit-c")).forEach(el => el.style.display = "block");
            Array.from(document.getElementsByClassName("unit-f")).forEach(el => el.style.display = "none");
            break;
        case "F":
            Array.from(document.getElementsByClassName("unit-c")).forEach(el => el.style.display = "none");
            Array.from(document.getElementsByClassName("unit-f")).forEach(el => el.style.display = "block");
            break;
    }
}

document.getElementById("unit-select").addEventListener("change", (e) => {
    setUnit(e.target.value);
    localStorage.setItem("unit", e.target.value);
});

setUnit(localStorage.getItem("unit") || "C");
document.getElementById("unit-select").value = localStorage.getItem("unit") || "C";

setSensorStatus(false, "");


if (!!window.EventSource) {
    let source = new EventSource('/events');

    source.addEventListener('open', () => {
        setSensorStatus(true, window.location.hostname);
    });

    source.addEventListener('error', (e) => {
        if (e.target.readyState != EventSource.OPEN) {
            setSensorStatus(false, "");
        }
    });

    source.addEventListener('readings', (e) => {
        const readings = JSON.parse(e.data);
        setData(readings.temperatureC, readings.temperatureF, readings.humidity, readings.pressure, readings.altitude);
    });
}