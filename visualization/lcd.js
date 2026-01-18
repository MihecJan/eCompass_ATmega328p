const WIDTH = 128;
const HEIGHT = 64;
const SCALE = 10;

const canvas = document.getElementById("lcd");
canvas.width = WIDTH * SCALE;
canvas.height = HEIGHT * SCALE;

const ctx = canvas.getContext("2d");

const ws = new WebSocket("ws://localhost:8765");
ws.binaryType = "arraybuffer";

ws.onopen = () => {
    console.log("WebSocket connected");
    const statusElement = document.getElementById("connection_status");
    statusElement.innerText = "connected";
    statusElement.style.color = "#2dff37";
};

ws.onerror = (err) => {
    console.error("WebSocket error:", err);
};

ws.onclose = () => {
    console.error("WebSocket closed");
    const statusElement = document.getElementById("connection_status");
    statusElement.innerText = "disconnected";
    statusElement.style.color = "#8a2020";
};

ws.onmessage = (event) => {
    console.log("new frame");

    const data = new Uint8Array(event.data);

    ctx.fillStyle = "#7f9297";
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    ctx.fillStyle = "#000000";

    for (let page = 0; page < 8; page++)
    {
        for (let x = 0; x < WIDTH; x++)
        {
            const byte = data[page * WIDTH + x];

            for (let bit = 0; bit < 8; bit++)
            {
                if (byte & (1 << bit))
                {
                    const y = page * 8 + bit;
                    ctx.fillRect(
                        x * SCALE,
                        y * SCALE,
                        SCALE,
                        SCALE
                    );
                }
            }
        }
    }
};
