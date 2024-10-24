#!/usr/bin/env python3

import os
import cgi
import cgitb

name_env = os.environ.get('name')

if name_env:
    name = name_env
else:
    form = cgi.FieldStorage()
    name = form.getvalue('name', 'Guest')
 
html_content = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>CGI Response</title>
    <style>
        body {{
            font-family: 'Arial', sans-serif;
            background-color: #f4f4f4;
            text-align: center;
            padding-top: 70px;
        }}
        h1 {{
            font-size: 2.5rem;
            color: #333;
        }}
        p {{
            font-size: 1.2rem;
            color: #666;
            margin-bottom: 20px;
        }}
        a {{
            color: #42b883;
            text-decoration: none;
            font-size: 1rem;
        }}
        a:hover {{
            text-decoration: underline;
        }}
    </style>
</head>
<body>
    <h1>Hello, {name}!</h1>
    <p>Thank you for submitting the form.</p>
    <p><a href="../index.html">Go back to Home</a></p>
</body>
</html>
"""

print(html_content)