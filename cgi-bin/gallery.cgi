#!/bin/bash

UPLOAD_DIR="./www/html/upload-folder/"

cat <<EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>File Gallery</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            text-align: center;
            padding-top: 20px;
        }
        .gallery img {
            width: 200px;
            height: 150px;
            margin: 10px;
            border-radius: 5px;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
            cursor: pointer;
        }
        .documents {
            margin: 20px 0;
        }
        .documents a {
            display: block;
            margin: 10px;
            text-decoration: none;
            color: #333;
            font-size: 18px;
        }
        .documents a:hover {
            color: #42b883;
        }
        .button-container {
            margin-top: 30px;
        }
        .button-container a {
            display: inline-block;
            padding: 10px 20px;
            background-color: #42b883;
            color: white;
            text-decoration: none;
            border-radius: 5px;
            transition: background-color 0.3s ease;
        }
        .button-container a:hover {
            background-color: #3aa374;
        }
        .fullscreen {
            width: 100%;
            height: 100%;
            object-fit: contain;
        }
    </style>
    <script>
        function openFullscreen(element) {
            if (element.requestFullscreen) {
                element.requestFullscreen();
            } else if (element.mozRequestFullScreen) { // Firefox
                element.mozRequestFullScreen();
            } else if (element.webkitRequestFullscreen) { // Chrome, Safari, and Opera
                element.webkitRequestFullscreen();
            } else if (element.msRequestFullscreen) { // IE/Edge
                element.msRequestFullscreen();
            }
            element.classList.add('fullscreen');
        }

        document.addEventListener('DOMContentLoaded', function() {
            const images = document.querySelectorAll('.gallery img');
            images.forEach(function(img) {
                img.addEventListener('click', function() {
                    openFullscreen(img);
                });
            });
        });
    </script>
</head>
<body>
    <h1>File Gallery</h1>
    <div class="gallery">
EOF

for file in "$UPLOAD_DIR"/*; do
    if [[ -f "$file" ]]; then
        ext="${file##*.}"
        if [[ "$ext" == "jpg" || "$ext" == "jpeg" || "$ext" == "png" || "$ext" == "gif" ]]; then
            filename=$(basename "$file")
            echo "      <img src=\"/upload-folder/$filename\" alt=\"$filename\">"
        fi
    fi
done

cat <<EOF
    </div>
    <h2>Documents</h2>
    <div class="documents">
EOF

for file in "$UPLOAD_DIR"/*; do
    if [[ -f "$file" ]]; then
        ext="${file##*.}"
        if [[ "$ext" == "txt" || "$ext" == "pdf" || "$ext" == "doc" || "$ext" == "docx" ]]; then
            filename=$(basename "$file")
            echo "      <a href=\"/upload-folder/$filename\">$filename</a>"
        fi
    fi
done

cat <<EOF
    </div>
    <div class="button-container">
        <a href="/index.html">Go back to Home</a>
    </div>
</body>
</html>
EOF
