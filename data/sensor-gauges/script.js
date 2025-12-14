lucide.createIcons();

let x = 0;

let temperatureChart;
let humidityChart;
let pressureChart;
let altitudeChart;

function setData(temperatureC, temperatureF, humidity, pressure, altitude) {
    let temperature = document.getElementsByClassName("unit-c")[0].style.display === "block" ? temperatureC.toFixed(2) : temperatureF.toFixed(2);
    humidity = humidity.toFixed(2);
    pressure = pressure.toFixed(2);
    altitude = altitude.toFixed(2);
    x++;

    temperatureChart && temperatureChart.series(0).points.add({ y: temperature, x: x }, { shift: true });
    humidityChart && humidityChart.series(0).points.add({ y: humidity, x: x }, { shift: true });
    pressureChart && pressureChart.series(0).points.add({ y: pressure, x: x }, { shift: true });
    altitudeChart && altitudeChart.series(0).points.add({ y: altitude, x: x }, { shift: true });

    document.getElementById("temperature-c").textContent = temperatureC.toFixed(2);
    document.getElementById("temperature-f").textContent = temperatureF.toFixed(2);
    document.getElementById("humidity").textContent = humidity;
    document.getElementById("pressure").textContent = pressure;
    document.getElementById("altitude").textContent = altitude;
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

        temperatureChart = JSC.chart('gauge-temperature', {
            defaultSeries_type: 'gauge linear',
            legend_visible: false,
            yAxis: { scale_range: [0, 50] },
            defaultPoint_label_visible: true,
            series: [
                {
                    type: 'column subtle',
                    name: 'Temperature (°C)',
                    points: [[0, 0]]
                }
            ]
        });

        humidityChart = JSC.chart('gauge-humidity', {
            defaultSeries_type: 'gauge circular',
            legend_visible: false,
            yAxis: { scale_range: [0, 100] },
            defaultPoint_label_visible: true,
            series: [
                {
                    type: 'column subtle',
                    name: 'Humidity (%)',
                    points: [[0, 0]]
                }
            ]
        });

        pressureChart = JSC.chart('gauge-pressure', {
            defaultSeries_type: 'gauge circular',
            legend_visible: false,
            yAxis: { scale_range: [900, 1100] },
            defaultPoint_label_visible: true,
            series: [
                {
                    type: 'column subtle',
                    name: 'Pressure (hPa)',
                    points: [[0, 0]]
                }
            ]
        });

        altitudeChart = JSC.chart('gauge-altitude', {
            defaultSeries_type: 'gauge linear',
            legend_visible: false,
            yAxis: { scale_range: [0, 4000] },
            defaultPoint_label_visible: true,
            series: [
                {
                    type: 'column subtle',
                    name: 'Altitude (m)',
                    points: [[0, 0]]
                }
            ]
        });
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

// var chart = JSC.chart(
//     'chartDiv',
//     {
//         debug: true,
//         legend_position: 'inside top right',
//         yAxis_formatString: 'c',
//         defaultSeries: { type: 'line', opacity: 1 },
//         defaultPoint: {
//             label_text: '%yValue',
//             marker: {
//                 type: 'circle',
//                 size: 11,
//                 outline: { color: 'white', width: 2 }
//             }
//         },
//         xAxis_overflow: 'hidden',
//         margin_right: 20,
//         animation: { duration: 400 },
//         xAxis: { scale_type: 'time' },
//         series: [
//             {
//                 name: 'Purchases',
//                 points: [
//                     ['0', 29.9]
//                 ]
//             }
//         ]
//     },
//     start
// );

// function start() {
//     setInterval(() => chart && chart.series(0).points.add({ y: Math.random() * 200, x: x++ }, { shift: true }), 800);
// }