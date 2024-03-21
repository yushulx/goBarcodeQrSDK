let imageFile = document.getElementById('image_file');
let overlayCanvas = document.getElementById('overlay_canvas');

let img = new Image();
let globalPoints;

overlayCanvas.addEventListener('dragover', function (event) {
    event.preventDefault();
    event.dataTransfer.dropEffect = 'copy';
}, false);

overlayCanvas.addEventListener('drop', function (event) {
    event.preventDefault();
    if (event.dataTransfer.files.length > 0) {
        let file = event.dataTransfer.files[0];
        if (file.type.match('image.*')) {
            let reader = new FileReader();
            reader.onload = function (e) {
                loadImage2Canvas(e.target.result);
            };
            reader.readAsDataURL(file);
        } else {
            alert("Please drop an image file.");
        }
    }
}, false);

function loadImage2Canvas(base64Image) {
    imageFile.src = base64Image;
    img.src = base64Image;
    img.onload = function () {
        let width = img.width;
        let height = img.height;

        overlayCanvas.width = width;
        overlayCanvas.height = height;

        detect();
    };

}

document.addEventListener('paste', (event) => {
    const items = (event.clipboardData || event.originalEvent.clipboardData).items;

    for (index in items) {
        const item = items[index];
        if (item.kind === 'file') {
            const blob = item.getAsFile();
            const reader = new FileReader();
            reader.onload = (event) => {
                loadImage2Canvas(event.target.result);
            };
            reader.readAsDataURL(blob);
        }
    }
});

function toggleLoading(isLoading) {
    if (isLoading) {
        document.getElementById("loading-indicator").style.display = "flex";
    }
    else {
        document.getElementById("loading-indicator").style.display = "none";
    }
}

document.getElementById("pick_file").addEventListener("change", function () {
    let currentFile = this.files[0];
    if (currentFile == null) {
        return;
    }
    var fr = new FileReader();
    fr.onload = function () {
        loadImage2Canvas(fr.result);
    }
    fr.readAsDataURL(currentFile);
});

async function detect() {
    toggleLoading(true);

    let detection_result = document.getElementById('detection_result');
    detection_result.innerHTML = "";
    let context = overlayCanvas.getContext('2d');
    context.clearRect(0, 0, overlayCanvas.width, overlayCanvas.height);

    const base64Image = img.src;

    const requestBody = {
        image: base64Image
    };

    try {
        const response = await fetch('/upload', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(requestBody),
        });

        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }

        const barcodes = await response.json();
        detection_result.value = `Found ${barcodes.length} barcode(s)\n`;
        barcodes.forEach(barcode => {
            detection_result.value += `\nText: ${barcode.Text}, Format: ${barcode.Format}`;
            detection_result.value += `\nCoordinates: (${barcode.X1}, ${barcode.Y1}), (${barcode.X2}, ${barcode.Y2}), (${barcode.X3}, ${barcode.Y3}), (${barcode.X4}, ${barcode.Y4})`;
            detection_result.value += "\n--------------";
            // Draw overlay
            context.beginPath();
            context.strokeStyle = '#ff0000';
            context.lineWidth = 2;
            context.moveTo(barcode.X1, barcode.Y1);
            context.lineTo(barcode.X2, barcode.Y2);
            context.lineTo(barcode.X3, barcode.Y3);
            context.lineTo(barcode.X4, barcode.Y4);
            context.lineTo(barcode.X1, barcode.Y1);
            context.stroke();

            context.font = '18px Verdana';
            context.fillStyle = '#ff0000';
            let x = [barcode.X1, barcode.X2, barcode.X3, barcode.X4];
            let y = [barcode.Y1, barcode.Y2, barcode.Y3, barcode.Y4];
            x.sort(function (a, b) {
                return a - b;
            });
            y.sort(function (a, b) {
                return b - a;
            });
            let left = x[0];
            let top = y[0];

            context.fillText(barcode.Text, left, top + 50);
        });


    } catch (error) {
        console.error('Error:', error);
        detection_result.innerHTML = "Failed to detect barcode";
    }

    toggleLoading(false);
}


