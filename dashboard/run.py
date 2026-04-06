# -*- encoding: utf-8 -*-
"""
Copyright (c) 2019 - present AppSeed.us
"""

import os
from pathlib import Path

from dotenv import load_dotenv

from   flask_migrate import Migrate
from   flask_minify  import Minify
from   sys import exit

load_dotenv(Path(__file__).resolve().parent / ".env")

from apps.config import config_dict
from apps import create_app, db

from apps.extensions import socketio

from apps.mqtt.client import start_mqtt
# WARNING: Don't run with debug turned on in production!
DEBUG = (os.getenv('DEBUG', 'False') == 'True')

# The configuration
get_config_mode = 'Debug' if DEBUG else 'Production'

try:

    # Load the configuration using the default values
    app_config = config_dict[get_config_mode.capitalize()]

except KeyError:
    exit('Error: Invalid <config_mode>. Expected values [Debug, Production] ')

app = create_app(app_config)


# Create tables & Fallback to SQLite
# with app.app_context():
    
#     try:
#         db.create_all()
#     except Exception as e:

#         print('> Error: DBMS Exception: ' + str(e) )

#         # fallback to SQLite
#         basedir = os.path.abspath(os.path.dirname(__file__))
#         app.config['SQLALCHEMY_DATABASE_URI'] = SQLALCHEMY_DATABASE_URI = 'sqlite:///' + os.path.join(basedir, 'db.sqlite3')

#         print('> Fallback to SQLite ')
#         db.create_all()

# Apply all changes
Migrate(app, db)

if not DEBUG:
    Minify(app=app, html=True, js=False, cssless=False)
    
if DEBUG:
    app.logger.info('DEBUG            = ' + str(DEBUG)             )
    app.logger.info('Page Compression = ' + 'FALSE' if DEBUG else 'TRUE' )
    app.logger.info('DBMS             = ' + app_config.SQLALCHEMY_DATABASE_URI)
# Start MQTT Client

if __name__ == "__main__":
    socketio.init_app(app)
    # app.run()
    # from apps.websocket import start_websocket_test
    # start_websocket_test(socketio)

    start_mqtt(app)
    from apps.websocket import emitir_periodicamente
    socketio.start_background_task(emitir_periodicamente, app)
    socketio.run(app, host="127.0.0.1", port=5000, debug=True, use_reloader=False)
    