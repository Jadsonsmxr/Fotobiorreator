# -*- encoding: utf-8 -*-

from flask import jsonify, request
from flask_login import login_required

from apps.biomass_classifier import blueprint
from apps.biomass_classifier.service import (
    create_prediction_record,
    get_prediction_history,
    retrain_model_from_manual_labels,
    save_manual_label,
)


@blueprint.route("/predict", methods=["POST"])
@login_required
def predict_biomass():
    image = request.files.get("imagem")
    if image is None:
        return jsonify({"detail": "Envie uma imagem no campo 'imagem'."}), 400

    roi_values = {
        "x": request.form.get("roi_x", type=float),
        "y": request.form.get("roi_y", type=float),
        "width": request.form.get("roi_width", type=float),
        "height": request.form.get("roi_height", type=float),
    }
    roi = None if any(value is None for value in roi_values.values()) else roi_values

    try:
        payload = create_prediction_record(image, roi=roi)
    except FileNotFoundError as error:
        return jsonify({"detail": str(error)}), 503
    except ValueError as error:
        return jsonify({"detail": str(error)}), 400
    except Exception:
        return jsonify({"detail": "Não foi possível processar a imagem enviada."}), 500

    return jsonify(payload)


@blueprint.route("/labels", methods=["POST"])
@login_required
def save_biomass_label():
    record_id = request.form.get("id", type=int)
    manual_status = request.form.get("status_real", type=str)

    if record_id is None or not manual_status:
        return jsonify({"detail": "Informe 'id' e 'status_real'."}), 400

    try:
        payload = save_manual_label(record_id, manual_status)
    except ValueError as error:
        return jsonify({"detail": str(error)}), 400
    except LookupError as error:
        return jsonify({"detail": str(error)}), 404

    return jsonify(payload)


@blueprint.route("/history", methods=["GET"])
@login_required
def biomass_history():
    limit = request.args.get("limit", default=20, type=int)
    return jsonify(get_prediction_history(limit=limit))


@blueprint.route("/retrain", methods=["POST"])
@login_required
def retrain_biomass_model():
    try:
        payload = retrain_model_from_manual_labels()
    except ValueError as error:
        return jsonify({"detail": str(error)}), 400
    except FileNotFoundError as error:
        return jsonify({"detail": str(error)}), 404
    except Exception:
        return jsonify({"detail": "Não foi possível retreinar o modelo agora."}), 500

    return jsonify(payload)
