# -*- encoding: utf-8 -*-
"""
Biomass classification module integrated into the Flask dashboard.
"""

from flask import Blueprint

blueprint = Blueprint(
    "biomass_classifier_blueprint",
    __name__,
    url_prefix="/api/biomass-classifier",
)
