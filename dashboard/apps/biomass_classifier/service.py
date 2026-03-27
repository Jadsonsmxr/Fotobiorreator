from __future__ import annotations

import csv
import sqlite3
import uuid
from datetime import datetime, timezone
from pathlib import Path

import joblib
import numpy as np
import pandas as pd
from PIL import Image, UnidentifiedImageError
from werkzeug.datastructures import FileStorage
from werkzeug.utils import secure_filename


MODULE_DIR = Path(__file__).resolve().parent
DATA_DIR = MODULE_DIR / "data"
ARTIFACTS_DIR = MODULE_DIR / "artifacts"
UPLOADS_DIR = MODULE_DIR / "uploads"

DATABASE_PATH = DATA_DIR / "biomass_classifier.sqlite3"
MODEL_PATH = ARTIFACTS_DIR / "biomass_classifier.joblib"
DEFAULT_TRAINING_DATASET_PATH = DATA_DIR / "training_dataset.csv"
MANUAL_TRAINING_DATASET_PATH = DATA_DIR / "manual_training_dataset.csv"

FEATURE_COLUMNS = ["mean_green_index", "mean_gray", "std_color", "mean_hue"]
VALID_CLASSES = ("Baixa", "Media", "Alta", "Sedimentacao")
MODEL_BUNDLE_CACHE: dict[str, object] | None = None
MODEL_BUNDLE_MTIME: float | None = None


def ensure_storage() -> None:
    DATA_DIR.mkdir(parents=True, exist_ok=True)
    ARTIFACTS_DIR.mkdir(parents=True, exist_ok=True)
    UPLOADS_DIR.mkdir(parents=True, exist_ok=True)

    with sqlite3.connect(DATABASE_PATH) as connection:
        connection.execute(
            """
            CREATE TABLE IF NOT EXISTS biomass_classification_records (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                created_at TEXT NOT NULL,
                image_path TEXT NOT NULL,
                mean_green_index REAL NOT NULL,
                mean_gray REAL NOT NULL,
                std_color REAL NOT NULL,
                mean_hue REAL NOT NULL,
                predicted_status TEXT NOT NULL,
                manual_status TEXT,
                confidence REAL NOT NULL
            )
            """
        )


def get_connection() -> sqlite3.Connection:
    connection = sqlite3.connect(DATABASE_PATH)
    connection.row_factory = sqlite3.Row
    return connection


def save_uploaded_image(upload: FileStorage) -> Path:
    original_name = secure_filename(upload.filename or "")
    suffix = Path(original_name).suffix.lower() or ".jpg"
    filename = f"{uuid.uuid4().hex}{suffix}"
    image_path = UPLOADS_DIR / filename
    upload.save(image_path)
    return image_path


def extract_features(image_path: str | Path) -> dict[str, float]:
    image = Image.open(image_path).convert("RGB")
    rgb_array = np.array(image, dtype=np.float32)

    red_channel = rgb_array[:, :, 0]
    green_channel = rgb_array[:, :, 1]
    blue_channel = rgb_array[:, :, 2]

    rgb_sum = red_channel + green_channel + blue_channel + 1e-6
    green_index = green_channel / rgb_sum
    mean_green_index = float(green_index.mean())

    gray = 0.21 * red_channel + 0.72 * green_channel + 0.07 * blue_channel
    mean_gray = float(gray.mean())
    std_color = float(gray.std())

    hsv_image = image.convert("HSV")
    hsv_array = np.array(hsv_image, dtype=np.float32)
    mean_hue = float(hsv_array[:, :, 0].mean() / 255.0 * 360.0)

    return {
        "mean_green_index": mean_green_index,
        "mean_gray": mean_gray,
        "std_color": std_color,
        "mean_hue": mean_hue,
    }


def train_model(
    csv_path: str | Path = DEFAULT_TRAINING_DATASET_PATH,
    model_path: str | Path = MODEL_PATH,
):
    global MODEL_BUNDLE_CACHE, MODEL_BUNDLE_MTIME

    from sklearn.model_selection import cross_val_score
    from sklearn.preprocessing import LabelEncoder
    from xgboost import XGBClassifier

    csv_path = Path(csv_path)
    if not csv_path.exists():
        raise FileNotFoundError(f"Dataset de treino não encontrado: {csv_path}")

    dataset = pd.read_csv(csv_path)
    if dataset.empty:
        raise ValueError("O dataset de treino está vazio.")

    missing_columns = [column for column in FEATURE_COLUMNS + ["status"] if column not in dataset.columns]
    if missing_columns:
        raise ValueError(f"Colunas ausentes no dataset de treino: {', '.join(missing_columns)}")

    features = dataset[FEATURE_COLUMNS].values
    label_encoder = LabelEncoder()
    target = label_encoder.fit_transform(dataset["status"])

    model = XGBClassifier(
        n_estimators=100,
        max_depth=3,
        learning_rate=0.1,
        use_label_encoder=False,
        eval_metric="mlogloss",
        random_state=42,
    )
    model.fit(features, target)

    if len(dataset) >= 12 and dataset["status"].nunique() > 1:
        cross_validation_scores = cross_val_score(model, features, target, cv=3, scoring="accuracy")
        print(
            "[biomass_classifier] acurácia CV-3: "
            f"{cross_validation_scores.mean():.2f} ± {cross_validation_scores.std():.2f}"
        )

    joblib.dump({"model": model, "label_encoder": label_encoder}, model_path)
    MODEL_BUNDLE_CACHE = {"model": model, "label_encoder": label_encoder}
    MODEL_BUNDLE_MTIME = Path(model_path).stat().st_mtime
    return model, label_encoder


def load_model_bundle(model_path: str | Path = MODEL_PATH) -> dict:
    global MODEL_BUNDLE_CACHE, MODEL_BUNDLE_MTIME

    model_path = Path(model_path)
    if not model_path.exists():
        train_model(DEFAULT_TRAINING_DATASET_PATH, model_path)

    current_mtime = model_path.stat().st_mtime
    if MODEL_BUNDLE_CACHE is not None and MODEL_BUNDLE_MTIME == current_mtime:
        return MODEL_BUNDLE_CACHE

    MODEL_BUNDLE_CACHE = joblib.load(model_path)
    MODEL_BUNDLE_MTIME = current_mtime
    return MODEL_BUNDLE_CACHE


def predict_image(image_path: str | Path) -> dict[str, object]:
    bundle = load_model_bundle()
    model = bundle["model"]
    label_encoder = bundle["label_encoder"]

    features = extract_features(image_path)
    model_input = np.array([[features[column] for column in FEATURE_COLUMNS]])

    predicted_index = model.predict(model_input)[0]
    predicted_probabilities = model.predict_proba(model_input)[0]

    predicted_status = label_encoder.inverse_transform([predicted_index])[0]
    probabilities = {
        class_name: float(probability)
        for class_name, probability in zip(label_encoder.classes_, predicted_probabilities)
    }

    return {
        "status": predicted_status,
        "probabilidades": probabilities,
        "features": features,
    }


def create_prediction_record(upload: FileStorage) -> dict[str, object]:
    if not upload or not upload.filename:
        raise ValueError("Nenhuma imagem foi enviada.")

    image_path = save_uploaded_image(upload)

    try:
        result = predict_image(image_path)
    except FileNotFoundError:
        raise
    except UnidentifiedImageError as error:
        image_path.unlink(missing_ok=True)
        raise ValueError("Arquivo de imagem inválido.") from error
    except Exception:
        image_path.unlink(missing_ok=True)
        raise

    features = result["features"]
    probabilities = result["probabilidades"]
    confidence = max(probabilities.values())
    created_at = datetime.now(timezone.utc).isoformat()

    with get_connection() as connection:
        cursor = connection.execute(
            """
            INSERT INTO biomass_classification_records (
                created_at,
                image_path,
                mean_green_index,
                mean_gray,
                std_color,
                mean_hue,
                predicted_status,
                confidence
            )
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
            """,
            (
                created_at,
                str(image_path),
                features["mean_green_index"],
                features["mean_gray"],
                features["std_color"],
                features["mean_hue"],
                result["status"],
                confidence,
            ),
        )
        record_id = cursor.lastrowid

    return {
        "id": record_id,
        "status": result["status"],
        "confianca": round(confidence, 3),
        "probabilidades": {key: round(value, 3) for key, value in probabilities.items()},
        "features": {key: round(value, 4) for key, value in features.items()},
    }


def save_manual_label(record_id: int, manual_status: str) -> dict[str, object]:
    if manual_status not in VALID_CLASSES:
        raise ValueError(f"status_real deve ser um de: {', '.join(VALID_CLASSES)}")

    with get_connection() as connection:
        row = connection.execute(
            "SELECT id FROM biomass_classification_records WHERE id = ?",
            (record_id,),
        ).fetchone()

        if not row:
            raise LookupError("Registro não encontrado.")

        connection.execute(
            """
            UPDATE biomass_classification_records
            SET manual_status = ?
            WHERE id = ?
            """,
            (manual_status, record_id),
        )

    return {"ok": True, "id": record_id, "status_real": manual_status}


def get_prediction_history(limit: int = 20) -> list[dict[str, object]]:
    safe_limit = max(1, min(limit, 100))
    with get_connection() as connection:
        rows = connection.execute(
            """
            SELECT *
            FROM biomass_classification_records
            ORDER BY id DESC
            LIMIT ?
            """,
            (safe_limit,),
        ).fetchall()

    return [dict(row) for row in rows]


def retrain_model_from_manual_labels(min_samples: int = 8) -> dict[str, object]:
    with get_connection() as connection:
        labeled_records = connection.execute(
            """
            SELECT mean_green_index, mean_gray, std_color, mean_hue, manual_status AS status
            FROM biomass_classification_records
            WHERE manual_status IS NOT NULL
            """
        ).fetchall()

    if len(labeled_records) < min_samples:
        raise ValueError(
            f"Poucos dados rotulados ({len(labeled_records)}). Precisamos de pelo menos {min_samples}."
        )

    with MANUAL_TRAINING_DATASET_PATH.open("w", newline="", encoding="utf-8") as csv_file:
        writer = csv.DictWriter(csv_file, fieldnames=FEATURE_COLUMNS + ["status"])
        writer.writeheader()
        for row in labeled_records:
            writer.writerow(dict(row))

    train_model(MANUAL_TRAINING_DATASET_PATH, MODEL_PATH)

    labeled_dataset = pd.DataFrame([dict(row) for row in labeled_records])
    return {
        "ok": True,
        "amostras": len(labeled_dataset),
        "classes": labeled_dataset["status"].value_counts().to_dict(),
    }


ensure_storage()
