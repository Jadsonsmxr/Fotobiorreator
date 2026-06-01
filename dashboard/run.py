# -*- encoding: utf-8 -*-
"""
Copyright (c) 2019 - present AppSeed.us
"""

import os
from pathlib import Path
from sys import exit

from dotenv import load_dotenv
from flask_migrate import Migrate
from flask_minify import Minify

load_dotenv(Path(__file__).resolve().parent / ".env")

from apps.config import config_dict
from apps import create_app, db
from apps.extensions import socketio
from apps.mqtt.client import start_mqtt

# WARNING: Don't run with debug turned on in production!
DEBUG = os.getenv("DEBUG", "False") == "True"

# The configuration
get_config_mode = "Debug" if DEBUG else "Production"

try:
    app_config = config_dict[get_config_mode.capitalize()]
except KeyError:
    exit("Error: Invalid <config_mode>. Expected values [Debug, Production] ")

app = create_app(app_config)
socketio.init_app(app)

Migrate(app, db)

if not DEBUG:
    Minify(app=app, html=True, js=False, cssless=False)

if DEBUG:
    app.logger.info("DEBUG            = " + str(DEBUG))
    app.logger.info("Page Compression = " + ("FALSE" if DEBUG else "TRUE"))
    app.logger.info("DBMS             = " + app_config.SQLALCHEMY_DATABASE_URI)


def bootstrap_runtime_services() -> None:
    if app.extensions.get("realtime_bootstrap_done"):
        return

    app.extensions["realtime_bootstrap_done"] = True
    start_mqtt(app)

    from apps.websocket import emitir_periodicamente

    socketio.start_background_task(emitir_periodicamente, app)


if os.getenv("SKIP_RUNTIME_BOOTSTRAP") != "1":
    bootstrap_runtime_services()

if __name__ == "__main__":
    socketio.run(app, host="0.0.0.0", port=5005, debug=DEBUG, use_reloader=False)
